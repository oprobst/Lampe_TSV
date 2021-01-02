#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define OFF 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define PURPLE 4
#define CYAN 5
#define YELLOW 6
#define WHITE 7

#define FALSE 0
#define TRUE 1

#define SKIP_INTRO 0

static  uint8_t optionSelected = 1;
static volatile uint8_t gotoMenu = FALSE;

/* 
TSV LED Sign

Contains RGB LED Stripe array for diver, and stripes for  for letters 'T', 'S', 'V'.
Includes a button for selecting mode and some optional status LEDs.

Circuit Layout

Diver LED:		Letter T	Letter S	Letter V
PD6 red			PD4 red		PD0 red		PC3 red
PD5 green		PD7 blue	PD1 green	PC5 green
PD3 blue		PB0 green	PD2 blue	PC4 blue

Input button: PB6
Status LED: PC1, PC2 (additional PC0, PB1, PB2 not connected so far)

*/


void setup() {
	//init output pins
	DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2);
	DDRC |= (1<<PC1) | (1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5);
	DDRD |= (1<<PD0) | (1<<PD1) | (1<<PD2) | (1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7);
	
	// set pin for button
	PORTC = (1<<PC0);
	DDRC &= ~(1 << PINC0);
	
	//init PWMs:
	//OC2B (PD3) - Diver blue
	//OC0B (PD5) - Diver green
	//OC0A (PD6) - Diver red
	 
	TCCR2A |= (1 << COM2B1);
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR0B  |= (1 << WGM01) | (1 << WGM00);
	TCCR0A |= (1<<COM0B1);
	TCCR0A |= (1 << COM0A1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01);
	TCCR2B |= (1 << CS01);
	
	//pin change interrupt for PCINT 8 (=PC0 --> Button)
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT8);
	
	sei();
	
}

 
/*
 * Convenience method so set diver color.
 */ 
void colorDiver (uint8_t red, uint8_t green, uint8_t blue){
	OCR0A = red;
	OCR0B = green;
	OCR2B = blue;
}

/* 
 *Convenience method for letter. Use defined colors, like colorT(BLUE); 
 */
void colorT (uint8_t color){
	switch (color){
		case OFF:
			PORTB &= ~(1 << PB0);
			PORTD &= ~(1 << PD7);
			PORTD &= ~(1 << PD4);
			break;
		case BLUE:
			PORTB &= ~(1 << PB0);
			PORTD |=  (1 << PD7);
			PORTD &= ~(1 << PD4);
			break;
		case GREEN:
			PORTB |=  (1 << PB0);
			PORTD &= ~(1 << PD7);
			PORTD &= ~(1 << PD4);
			break;
		case RED:
			PORTB &= ~(1 << PB0);
			PORTD &= ~(1 << PD7);
			PORTD |=  (1 << PD4);
			break;
		case PURPLE:
			PORTB &= ~(1 << PB0);
			PORTD |=  (1 << PD7);
			PORTD |=  (1 << PD4);
			break;
		case YELLOW:
			PORTB |=  (1 << PB0);
			PORTD &= ~(1 << PD7);
			PORTD |=  (1 << PD4);
			break;
		case CYAN:
			PORTB |=  (1 << PB0);
			PORTD |=  (1 << PD7);
			PORTD &= ~ (1 << PD4);
			break;
		default: 
			PORTB |=  (1 << PB0);
			PORTD |=  (1 << PD7);
			PORTD |=  (1 << PD4);
			break;
	}
}

/* 
 *Convenience method for letter. Use defined colors, like colorS(BLUE); 
 */
void colorS (uint8_t color){
	switch (color){
		case OFF:
			PORTD &= ~(1 << PD0);
			PORTD &= ~(1 << PD1);
			PORTD &= ~(1 << PD2);
			break;
		case RED:
			PORTD |=  (1 << PD0);
			PORTD &= ~(1 << PD1);
			PORTD &= ~(1 << PD2);
			break;
		case GREEN:
			PORTD &= ~(1 << PD0);
			PORTD |=  (1 << PD1);
			PORTD &= ~(1 << PD2);
			break;
		case BLUE:
			PORTD &= ~(1 << PD0);
			PORTD &= ~(1 << PD1);
			PORTD |=  (1 << PD2);
			break;
		case PURPLE:
			PORTD |=  (1 << PD0);
			PORTD &= ~(1 << PD1);
			PORTD |=  (1 << PD2);
			break;
		case YELLOW:
			PORTD |=  (1 << PD0);
			PORTD |=  (1 << PD1);
			PORTD &= ~(1 << PD2);
			break;
		case CYAN:
			PORTD &= ~(1 << PD0);
			PORTD |=  (1 << PD1);
			PORTD |=  (1 << PD2);
			break;
		default: 
			PORTD |=  (1 << PD0);
			PORTD |=  (1 << PD1);
			PORTD |=  (1 << PD2);
			break;
	}
}


/* 
 *Convenience method for letter. Use defined colors, like colorV(BLUE); 
 */
void colorV (uint8_t color){
	switch (color){
		case OFF:
			PORTC &= ~(1 << PC4);
			PORTC &= ~(1 << PC5);
			PORTC &= ~(1 << PC3);
			break;
		case BLUE:
			 PORTC |=  (1 << PC4);
			 PORTC &= ~(1 << PC5);
			 PORTC &= ~(1 << PC3);
			 break;
		case GREEN:
			PORTC &= ~(1 << PC4);
			PORTC |=  (1 << PC5);
			PORTC &= ~(1 << PC3);
			break;
		case RED:
			PORTC &= ~(1 << PC4);
			PORTC &= ~(1 << PC5);
			PORTC |=  (1 << PC3);
			break;
		case PURPLE:
			PORTC |=  (1 << PC4);
			PORTC &= ~(1 << PC5);
			PORTC |=  (1 << PC3);
			break;
		case CYAN:
			PORTC |=  (1 << PC4);
			PORTC |=  (1 << PC5);
			PORTC &= ~(1 << PC3);
			break;
		case YELLOW:
			PORTC &= ~(1 << PC4);
			PORTC |=  (1 << PC5);
			PORTC |=  (1 << PC3);
			break;
		default: 
			PORTC |=  (1 << PC4);
			PORTC |=  (1 << PC5);
			PORTC |=  (1 << PC3);
			break;
	}
}

/*
 * Color for all letters
 */
void colorTSV (uint8_t t, uint8_t s, uint8_t v){
	colorT(t);
	colorS(s);
	colorV(v);
}
/*
 * Fancy start up animation
 */
void initSequence(){

    if (SKIP_INTRO) return;
	
	double speedLetter = 30; //ms
	double speedBlink = 350; //ms
	double fastBlink = 150; //ms


    uint8_t dRed = 0;
	uint8_t dGreen = 0;
	uint8_t dBlue = 0;

	colorDiver (0, 222, 1);
	colorTSV(CYAN, OFF, OFF);
	
	for (uint8_t i = 0; i < 20 ; i++){
	  dGreen += 1;
	  dRed += 1;
	  colorDiver (dRed, dGreen, dBlue);
	  _delay_ms(speedLetter);
	}		
	colorTSV(OFF, CYAN, OFF);
	
	for (uint8_t i = 0; i < 20 ; i++){
		dGreen += 1;
		dRed += 1;
		colorDiver (dRed, dGreen, dBlue);
		_delay_ms(speedLetter);
		if (gotoMenu) return;
	}
		
	colorTSV(OFF, OFF, CYAN);
	
	for (uint8_t i = 0; i < 40 ; i++){
		dBlue += 1;
		dRed -= 1;
		colorDiver (dRed, dGreen, dBlue);
		_delay_ms(speedLetter);
		if (gotoMenu) return;
	}
 
	
	colorTSV(CYAN, OFF, CYAN);
	
	for (uint8_t i = 0; i < 20 ; i++){		
		dGreen -= 1;
		colorDiver (dRed, dGreen, dBlue);
		_delay_ms(speedLetter);
		if (gotoMenu) return;
	}
	
	colorTSV(OFF, CYAN, CYAN);
	
	for (uint8_t i = 0; i < 20 ; i++){			
		dGreen -= 1;
		colorDiver (dRed, dGreen, dBlue);
		_delay_ms(speedLetter+speedLetter);
		if (gotoMenu) return;
	}
	
	
		
	colorTSV(CYAN, CYAN, CYAN);
	
	for (uint8_t i = 0; i < 20 ; i++){
		dRed += 1;
		colorDiver (dRed, dGreen, dBlue);
		_delay_ms(speedLetter);
		if (gotoMenu) return;
	}
		
	colorTSV(CYAN, CYAN, CYAN);
	_delay_ms(speedBlink);
	colorTSV(BLUE, BLUE, BLUE);
	_delay_ms(speedBlink);
	colorTSV(CYAN, CYAN, CYAN);
	_delay_ms(speedBlink);
	colorTSV(BLUE, BLUE, BLUE);	
	_delay_ms(speedBlink);
	for (uint8_t i = 0; i < 2; i++){
		colorTSV(RED, RED, RED);
		colorDiver (0, 255, 255);
		_delay_ms(fastBlink);		
		colorTSV(BLUE, BLUE, BLUE);
		_delay_ms(fastBlink);
		colorTSV(RED, RED, RED);
		colorDiver (dRed, dGreen, dBlue);
		_delay_ms(fastBlink);
		colorTSV(BLUE, BLUE, BLUE);
		
		_delay_ms(fastBlink);
	}
	
	for (uint8_t i = 0; i < 250 ; i++){			
		if (dRed >  5) dRed -= 1;
		if (dGreen < 250) dGreen += 1;
		if (dBlue < 250) dBlue += 1;
		colorDiver (dRed, dGreen, dBlue);
		_delay_ms(speedLetter+speedLetter);
		if (gotoMenu) return;
	}	
	
}

/*
 * this helper is used by mode2() and mode3()
void animationMode (double speed){
	uint8_t dRed = 0;
	uint8_t dGreen = 250;
	uint8_t dBlue = 250;			
	uint32_t wait = 0;
	while(!gotoMenu) {

		// fade green
		colorTSV(CYAN, GREEN, YELLOW);
		for (uint8_t i = 0; i < 250 ; i++){
			dBlue -= 1;
			colorDiver (dRed, dGreen, dBlue);
			while (wait++ < speed) _delay_ms(1);
			wait = 0;
			if (gotoMenu) return;
		}
		// fade yellow
		colorTSV(GREEN, YELLOW, PURPLE);
		for (uint8_t i = 0; i < 250 ; i++){
			dRed += 1;
			colorDiver (dRed, dGreen, dBlue);
			while (wait++ < speed) _delay_ms(1);
			wait = 0;
			if (gotoMenu) return;
		}		
		// fade purple
		colorTSV(YELLOW, PURPLE, BLUE);
		for (uint8_t i = 0; i < 250 ; i++){
			dBlue  += 1;
			dGreen -= 1;
			colorDiver (dRed, dGreen, dBlue);
			while (wait++ < speed) _delay_ms(1);
			wait = 0;
			if (gotoMenu) return;
		}
		// fade blue
		colorTSV(PURPLE, BLUE, CYAN);
		for (uint8_t i = 0; i < 250 ; i++){			
			dRed -= 1;
			colorDiver (dRed, dGreen, dBlue);
			while (wait++ < speed) _delay_ms(1);
			wait = 0;
			if (gotoMenu) return;
		}
		// fade cyan
		colorTSV(BLUE, CYAN, GREEN);
		for (uint8_t i = 0; i < 250 ; i++){
			dGreen  += 1;			
			colorDiver (dRed, dGreen, dBlue);
			while (wait++ < speed) _delay_ms(1);
			wait = 0;
			if (gotoMenu) return;
		}	
		
	}
}

/*
 * Default mode: from cyan to green and back within ca 1/2 hour. Text stays blue.
 */ 
void mode1 (){
	
	while(!gotoMenu) {
		double speed = 35;//ms
		uint8_t dRed = 5;
		uint8_t dGreen = 250;
		colorTSV(BLUE, BLUE, BLUE);
		 
		for (uint8_t i = 250; i > 0 ; i--){			
			colorDiver (dRed, dGreen, i);
			_delay_ms(speed);
			for (uint8_t j = 0; j < 100 ; j++){
				_delay_ms(speed);
				if (gotoMenu) return;
			}
		}
		for (uint8_t i = 0; i < 250 ; i++){		
			colorDiver (dRed, dGreen, i);
			for (uint8_t j = 0; j < 100 ; j++){
				_delay_ms(speed);
				if (gotoMenu) return;
			}
			
		}
	
	}		
}

/*
 * Mode 2: Colorful - changes colors slowly 
 */ 
void mode2 (){
	animationMode(100);
}

/*
 * Mode 3: Colorful - changes colors fast (eg. for party)
 */ 
void mode3 (){
	animationMode(10);
}


/*
 * Mode 4: Disco mode
 */ 
void mode4 (){
	
	uint8_t colorText = 1;
	while(optionSelected == 4) {
		if (++colorText > 8) colorText = 1;
		
		for (uint8_t i = 0; i < 5 ; i++){		
			colorDiver ( 255, 255,255);
			colorTSV (colorText, colorText, colorText);
			_delay_ms(30);
			colorDiver (0, 0, 0);
			colorTSV (0, 0, 0);
			_delay_ms(150);
			if (gotoMenu) return;
		}
	
	}		
}

/*
 * Change to user selected mode
 */
void changeMode (){
	gotoMenu = FALSE;
	switch (optionSelected){
		case 2:
		mode2();
		break;
		case 3:
		mode3();
		break;
		case 4:
		mode4();
		break;
		default:
		mode1();
		break;
	}
}
/*
 * Interrupt when button is pushed
 */
ISR(PCINT1_vect)
{
	gotoMenu = TRUE;
}

/*
 * Gives the user the opportunity to select the mode (workshop/party-slow/party-fast/disco)
 */
void selectMode (){
	cli();	
	gotoMenu = FALSE;
	optionSelected = 1;
	colorDiver(0,0,0);
	colorTSV(RED, OFF, OFF);
	_delay_ms(300);

	while (1){
		for (uint16_t i = 0; i < 300; i++ ){
			_delay_ms(10);
			if (!( PINC & (1<<PC0))){
				optionSelected = 2;
				colorTSV(OFF, RED, OFF);
				break;
			}
		}
		_delay_ms(300);
		if (optionSelected != 2) return;
    
	
		for (uint16_t i = 0; i< 300; i++ ){
			_delay_ms(10);
			if (!(  PINC & (1<<PC0))){
				optionSelected = 3;
				colorTSV(OFF, OFF, RED);
				break;
			}
		}
		_delay_ms(300);
		if (optionSelected != 3) return;
		
		for (uint16_t i = 0; i< 300; i++ ){
			_delay_ms(10);
			if (!(  PINC & (1<<PC0))){
				optionSelected = 4;
				colorTSV(OFF, OFF, OFF);
				colorDiver(250,0,0);
				break;
			}
		}
		_delay_ms(300);
		if (optionSelected != 4) return;
	
		for (uint16_t i = 0; i < 300; i++ ){
			_delay_ms(10);
			if (!( PINC & (1<<PC0))){
				optionSelected = 1;
				colorTSV(RED, OFF, OFF);
				colorDiver(0,0,0);
				break;
			}
		}
		_delay_ms(300);
		if (optionSelected !=1) return;
	}
}

int main(void) { 
	//initialize everything needed.
	setup();
	//show a startup sequence
	initSequence();	
	_delay_ms(1000);
	
	while (1){				
		// change the mode according to user selection (default on startup = 1)
		changeMode();		
		// changeMode will only be left when user push the button -> triggers an interrupt ending current mode.
		// give the user the option to select a new mode here:
		selectMode();
		// and enable interrupt for next selectMode() again.
		sei();
	}

}
