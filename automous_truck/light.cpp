/*
 * mt_project.cpp
 *
 * Created: 3/23/2023 9:32:25 PM
 * Author : hac177
 */ 

#include <avr/io.h>
char sensorvalue = 0; //water level sensor
int main(void)
{
	
	DDRD = 0b11000000;  // set bits PD7-PD6 as output, PD5-PD0 as input (this covers Pins 3 and 4 as needed for inputs, and 6 and 7 as needed for outputs)
	// Follows a practice of making unused pins to be input (safer)
	PORTD = 0b11000000; // set output bits on PORTD so that all outputs are off (they are wired as active low)
	
	DDRC =  0x00;  // define all Port C bits as input bit 0 sensor 
	PRR = 0x00;  // clear Power Reduction ADC bit (0) in PRR register;
	ADCSRA = 0b10000111; // 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;  // 0x87 // 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	// ============================================
	// P R O G R A M L O O P
	// ============================================
	while(1)
	{
		ADMUX = 0b01100000;
		ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & 0b00010000) == 0); // Alternate code: while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensorvalue = ADCH;
		
		if (sensorvalue>50) // Check the status of Switch 1.
		{
			PORTD = PORTD & 0b10111111; // Turn on LED 1.
			PORTD = PORTD | 0b10000000; // Turn off LED 2.
			// This line uses more Boolean arithmetic to clear pin PD6 while leaving the other bits on PORTD unchanged.  Specifically, the line computes
			// a logical AND between the existing PORTD values and a binary number that has 0 in the only bit that we want to ensure is 0, PD6.  All other bits remain
			// unchanged (i.e. this line does not affect bit PD7, the other LED).
			// The result is that since the LED on pin 6 of PORTD is wired as active low, this line causes it to turn on.
		}
		else
		{
			PORTD = PORTD | 0b01000000;  // Turn off LED 1.
			PORTD = PORTD & 0b01111111;  // Turn on LED 2
			// More Boolean arithmetic to cause PD6 to go high (a logical OR is performed between data in PORTD and a binary number that targets pin PD6),
			// which causes LED 1 to be off and other bits, specifically LED 2, are unchanged.
		}	
	}
	
	return 0;
}
// ============================================
// PD 7-4 DC motor, 3-0 step
// PC 
// ============================================
