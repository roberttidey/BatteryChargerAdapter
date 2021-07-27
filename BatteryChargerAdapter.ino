//12V Battery Charger Adapter
//R.J.Tidey 20 Mayy 2021
//Note text co-ordinates are by pixel in x direction and by 8 pixels in Y direction
#include <Arduino.h>

#include <ssd1306BB.h>

// default PIN configuration is as below
// VCC ---- vcc
// GND ---- gnd
// SCL ---- pb3
// SDA ---- pb1
// CTL ---- pb0
// VOLTS -- pb5
// AMPS --- pb2
// UNUSED - pb4
#define display ssd1306_string
#define BLANK "                "
#define INTERVAL 6000
#define AVERAGING 4
#define AVE_SHIFT 2

//Conversion factors from digital readings
//voltage - 10k/470R divider
#define CONV_VOLTS  21963ul
#define CONV_AMPS   20168ul
#define CONV_CHARGE 266ul
//Should be CONV_AMPS * 8 * interval(secs) / 3600
#define AMPS_THRESHOLD 25
#define VOLTS_THRESHOLD 625

#define ADC_VOLTS		A0
#define ADC_AMPS		A1
#define AMPS_ZERO		3
#define CHARGE_CTL		0
#define SSD1306_SCL		3		// PB3 SCL
#define SSD1306_SDA		1		// PB1 SDA 
#define SSDI2C_DELAY	4		// sets i2c speed
#define SSD1306_SA		0X3C	// Slave address

unsigned int volts = 0;
unsigned int amps = 0;
unsigned long charge = 0;
unsigned int chargeAcc = 0;
unsigned int sampleCounter = 0;
int ch_ctl = 0x55;
char valString[6];

void processValues() {
	volts = volts>>AVE_SHIFT;
	amps = amps>>AVE_SHIFT;
	if(amps >= AMPS_ZERO) amps -= AMPS_ZERO;
	chargeAcc += amps;
	if(sampleCounter >= (8*AVERAGING)) {
		sampleCounter = 0;
		charge += chargeAcc>>3;
		chargeAcc = 0;
	}
}

void makeValString(unsigned long val, unsigned int conv) {
	uint8_t i=0, j=0;
	unsigned long valc = val * conv;
	char temp[10];
	ultoa(valc, temp, 10);
	uint8_t tl = strlen(temp);
	for(i=0; i<5; i++) {
		if(i == 2) {
			valString[i] = '.';
			tl--; //compensate for decimal place
		} else {
			if(tl > (7 - i)) {
				valString[i] = temp[j];
				j++;
			} else {
				valString[i] = '0';
			}
		}
	}
	if(valString[0] == '0') valString[0] = ' ';
}

void displayInit() {
	delay(1000);
	SSD1306.ssd1306_init(SSD1306_SDA, SSD1306_SCL, SSD1306_SA, SSDI2C_DELAY);
	delay(500);
	SSD1306.ssd1306_fillscreen(0);
	SSD1306.ssd1306_string(0,6,(char*)BLANK);
	SSD1306.ssd1306_string(0,0,(char*)"Charger");
	SSD1306.ssd1306_string(0,2,(char*)"Volts");
	SSD1306.ssd1306_string(0,4,(char*)"Amps");
	SSD1306.ssd1306_string(0,6,(char*)"AmpHr");
}

void displayValues() {
	makeValString((unsigned long)volts, CONV_VOLTS);
	SSD1306.ssd1306_string(64,2, valString);
	makeValString((unsigned long)amps, CONV_AMPS);
	SSD1306.ssd1306_string(64,4, valString);
	makeValString(charge, CONV_CHARGE);
	SSD1306.ssd1306_string(64,6, valString);
}

void chargeControl() {
	int ctl;
	ctl = (volts < VOLTS_THRESHOLD || amps > AMPS_THRESHOLD);
	if(ctl != ch_ctl) {
		ch_ctl = ctl;
		if(ch_ctl) {
			pinMode(CHARGE_CTL, OUTPUT);
			SSD1306.ssd1306_string(96,0,(char*)"Hi");
		} else {
			pinMode(CHARGE_CTL, INPUT);
			SSD1306.ssd1306_string(96,0,(char*)"Lo");
		}
	}
}

void setup() {
	analogReference(INTERNAL);
	digitalWrite(CHARGE_CTL, 0);
	pinMode(CHARGE_CTL, INPUT);
	displayInit();
}

void loop() {
	volts += analogRead(ADC_VOLTS);
	amps += analogRead(ADC_AMPS);
	sampleCounter++;
	if((sampleCounter % AVERAGING) == 0) {
		processValues();
		displayValues();
		chargeControl();
		volts = 0;
		amps = 0;
	}
	delay(INTERVAL>>AVE_SHIFT);
}