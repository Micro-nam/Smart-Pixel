#ifndef _RGBRING_HPP_INCLUDED_
#define _RGBRING_HPP_INCLUDED_

#include <Adafruit_NeoPixel.h>
#include <vector>

void rainbow_soft_blink(unsigned short wait);
void rainbowCycle(unsigned short wait);
void colorWipe(unsigned short wait);
class Effekt;
enum class Effekt_ID;

unsigned short EffektSpeed;
/**
 * EffektContiner, der die Effekte, Name der Effekte, dessen ID und die Funktion von dem Effekt speichert.
 * Wird bei make_action durchgelaufen und geprueft, ob der Name mit einem von hier uebereinstimmt.
 * Dann wird die ID in aktuellerEffekt gespeichert, und in loop aufgerufen
 */
std::vector<Effekt> EffektContainer;
// Zeiger auf den aktuellen Effekt, nullptr wenn aktueller Effekt nichts/nothing ist
Effekt* aktueller_Effekt;


// Wird benoetigt um in Funktion getEffektFunction ein Funktionspointer zurueckzugeben
// https://stackoverflow.com/questions/997821/how-to-make-a-function-return-a-pointer-to-a-function-c
typedef void (*EffektFunktionPointer)(unsigned short speed);

class Effekt {
public:
	Effekt(String _name, void (*_effectFunction)(unsigned short speed)): name(_name), effectFunction(_effectFunction) { }
	~Effekt() { }
	Effekt& operator=(const Effekt& ef) {
		this->name = ef.getName();
		this->effectFunction = ef.getEffektFunction();
		return *this;
	}

	String getName() 							const { return name; }
	EffektFunktionPointer getEffektFunction()	const { return effectFunction; }
	
	// Den Effekt ausfuehren
	void run(unsigned short speed) { effectFunction(speed); }
private:
	String name;
	EffektFunktionPointer effectFunction;
};

void run_Effekt() {
	if(aktueller_Effekt != nullptr){
		aktueller_Effekt->run(EffektSpeed);
	}
}

// Effekte

void colorWipe(unsigned short wait) {
	static byte pos=0;
	static byte round=1;
	switch(round){
		case 1:{
			RGB_LEDS.setPixelColor(pos, RGB_LEDS.Color(255, 0, 0));
			break;
		}
		case 2:{
			RGB_LEDS.setPixelColor(pos, RGB_LEDS.Color(0, 255, 0));
			break;
		}
		case 3:{
			RGB_LEDS.setPixelColor(pos, RGB_LEDS.Color(0, 0, 255));
			break;
		}
	}
	RGB_LEDS.show();
	++pos;
	if(pos >= RGB_LEDS.numPixels()){
		round++;
		if(round > 3){
		  round = 1;
		}
		pos = 0;
	}
	delay((wait+2)*4); // Pause erhoehen, da es sonst viel zu schnell waere
}

void rainbow_soft_blink(unsigned short wait) {
	static byte schleife{1};    // Die auszufuehrende Schleife(wird in switch-case abgefragt)
	static byte prozent_helligkeit{0};  // Speicher ab, welche Helligkeit die gerade zum hochzaehldende LED hat.
	switch(schleife){
		case 1:{
			for(byte i{0}; i < RGB_LEDS.numPixels(); i++){
				//                                          rot             gruen         blau
				RGB_LEDS.setPixelColor(i, RGB_LEDS.Color(prozent_helligkeit,  0   , 255 - prozent_helligkeit));
			}
			break;
		}

		case 2:{
			for(byte i{0}; i < RGB_LEDS.numPixels(); i++){
				RGB_LEDS.setPixelColor(i, RGB_LEDS.Color(255 - prozent_helligkeit, prozent_helligkeit, 0));
			}
			break;
		}

		case 3:{
			for(byte i{0}; i < RGB_LEDS.numPixels(); i++){
				RGB_LEDS.setPixelColor(i, RGB_LEDS.Color(0, 255 - prozent_helligkeit, prozent_helligkeit));
			}
			break;
		}
	}
	if(prozent_helligkeit >= 255){
		if(schleife < 3){
			++schleife;
		}else{
			schleife = 1;
		}
		prozent_helligkeit = 0;
	}
	prozent_helligkeit++;
	RGB_LEDS.show();
	delay(wait);
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return RGB_LEDS.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return RGB_LEDS.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return RGB_LEDS.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
void rainbowCycle(unsigned short wait) {
  static uint16_t j=0;
    for(uint16_t i=0; i< RGB_LEDS.numPixels(); i++) {
      RGB_LEDS.setPixelColor(i, Wheel( ((i * 256 / RGB_LEDS.numPixels()) + j) & 255));
    }
    if(j++>255){
      j=0;
    }
    RGB_LEDS.show();
    delay(wait);
}

// Funktionen um den RGB-Hex-Code zu konvertieren und in dezimal Werte von 0-255 umwandeln

std::size_t power(unsigned short num, unsigned short times) {
	if(times >= 1) {
		return num * power(num, times - 1);
	}
	return 1;
}

unsigned short singleHexToDec(const char hex) {
	unsigned short ret;
	if(hex >= '0' && hex <= '9') {
		ret = hex - '0';
	} else if(hex >= 'a' && hex <= 'f') {
		ret = hex - 'a' + 10;
	} else if(hex >= 'A' && hex <= 'Z') {
		ret = hex - 'A' + 10;
	}
	return ret;
}

// https://owlcation.com/stem/Convert-Hex-to-Decimal
unsigned short hexToDec(const char* hex, std::size_t size_of_num){
	unsigned short ret = 0;
	for(unsigned short i = 0; i < size_of_num; i++) {
		// Elemente von vorne nach hinten durchlaufen(num)
		ret += singleHexToDec(hex[size_of_num - 1 - i]) * power(16, i);	// -1, weil 0 das erste element ist
	}
	return ret;
}


// https://www.developintelligence.com/blog/2017/02/rgb-to-hex-understanding-the-major-web-color-codes/
// Adafruit_NeoPixel::Color()
static uint32_t RGBHexToColor(const char* rgb_hex) {
	byte red;
	byte green;
	byte blue;

	// Erstes Zeichein ignorieren - ist das #
	red = hexToDec(rgb_hex + 1, 2);
	green = hexToDec(rgb_hex + 3, 2);
	blue = hexToDec(rgb_hex + 5, 2);

	return Adafruit_NeoPixel::Color(red, green, blue);
}

#endif // _RGBRING_HPP_INCLUDED_