/*
 * mt_project.cpp
 *
 * Created: 3/23/2023 9:32:25 PM
 * Author : hac177
 */ 

#include <avr/io.h>

void gostraight( void );
void turnleft( void );
void turnright( void );
void turnground( void );

char sensorvalue1 = 0; //straight sensor
char sensorvalue2 = 0; //left sensor
char sensorvalue3 = 0; //right sensor

int main(void)
{
	//sensor setting
	
	//Setup//
	DDRD = 0xFF;// Make PD4 forward and PD5 reverse output bits for left motor, and make PD7 forward and PD6 reverse output bits for right motor;
	// Set up ADC;
	DDRC =  0x00;  // define all Port C bits as input bit 0 sensor 1, bit 1 sensor 2, bit 2 sensor 3;
	PRR = 0x00;  // clear Power Reduction ADC bit (0) in PRR register;
	
	ADCSRA = 0b10000111; // 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;  // 0x87 // 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	
	
	while(1)
	{   //far small, close big
		// Read analog input
		ADMUX = 0b01100000;
		ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & 0b00010000) == 0); // Alternate code: while ((ADCSRA & (1<<ADIF)) ==0); // wait for conversion to finish
		sensorvalue1 = ADCH;
		
		ADMUX = 0b01100001;
		ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & 0b00010000) == 0);
		sensorvalue2 = ADCH;
		
		ADMUX = 0b01100010;
		ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & 0b00010000) == 0);
		sensorvalue3 = ADCH;
		
		if (sensorvalue1>100){
			gostraight();
		}
		else if(sensorvalue2>100){
			turnleft();
			while(sensorvalue3>100){
				gostraight();
				ADMUX = 0b01100010;
				ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
				while ((ADCSRA & 0b00010000) == 0);
				sensorvalue3 = ADCH;
			}
			turnright();
			while(sensorvalue3>100){
				gostraight();
				ADMUX = 0b01100010;
				ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
				while ((ADCSRA & 0b00010000) == 0);
				sensorvalue3 = ADCH;
			}
			turnright();
			
			while(sensorvalue2>100){
				gostraight();
				ADMUX = 0b01100001;
				ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
				while ((ADCSRA & 0b00010000) == 0);
				sensorvalue2 = ADCH;
			}
			turnleft();
		}
		else if (sensorvalue3>100)
		{
			turnright();
			while(sensorvalue2>100){
				gostraight();
				ADMUX = 0b01100001;
				ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
				while ((ADCSRA & 0b00010000) == 0);
				sensorvalue2 = ADCH;
			}
			turnleft();
			
			while(sensorvalue2>100){
				gostraight();
				ADMUX = 0b01100001;
				ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
				while ((ADCSRA & 0b00010000) == 0);
				sensorvalue2 = ADCH;
			}
			turnleft();
			while(sensorvalue3>100){
				gostraight();
				ADMUX = 0b01100010;
				ADCSRA = ADCSRA | 0b01000000; //Alternate code: ADCSRA |= (1<<ADSC); // Start conversion
				while ((ADCSRA & 0b00010000) == 0);
				sensorvalue3 = ADCH;
			}
			turnright();
			
		}
		else
		{
			
		}
	} // end main while
} // end main

void turnright(){
	//two motor forward
	PORTD = 0b01100000;
}

void turnleft(){
	
	PORTD = 0b10010000;
	
}

void gostraight(){
	
	PORTD = 0b10100000;
	
}



