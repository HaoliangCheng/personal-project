/*
 * mt_project_motor.cpp
 *
 * Created: 4/14/2023 2:40:22 PM
 * Author : hac177
 */ 
#include <avr/io.h>

void gostraight( void );
void turnleft( void );
void turnright( void );

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write_to_address(unsigned char address);
void i2c_read_from_address(unsigned char address);
void i2c_write_data(unsigned char data);
unsigned char i2c_read_data(unsigned char ack);
void turnleft1( void );
void turnright1( void );
void step_CW( void );
void step_CCW( void );
void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);


char sensorvalue1 = 0; //straight sensor
char sensorvalue2 = 0; //left sensor
char sensorvalue3 = 0; //right sensor

char Tsensorvalue1 = 0; //straight Tsensor
char Tsensorvalue2 = 0; //left Tsensor
char Tsensorvalue3 = 0; //right Tsensor
int phase_step = 1; // We will use wave stepping, so the steps are numbered 1-4, then will repeat.

int main(void)
{
	DDRD = 0xFF;// Make PD4 forward and PD5 reverse output bits for left motor, and make PD7 forward and PD6 reverse output bits for right motor;
	// Set up ADC;
	DDRC =   0b01001000;   // define all Port C bits as input bit 0 sensor 1, bit 1 sensor 2, bit 2 sensor 3; bit 3 and 6 are water pump output; bit 4 and 5  are I
	PRR = 0x00;  // clear Power Reduction ADC bit (0) in PRR register;
	PORTC = PORTB | 0b00110000;  // set internal pull-up resistors on SCL and SDA lines (PC5 and PC4) for I2C bus
	//PORTC |= 0b00001000;
	i2c_init();
	i2c_start();
	//write to MCP9808 to set resolution to 0.5C
	i2c_write_to_address(0x18);   //A2-A0 are connected to ground and W/r is cleared to indicate writing
	i2c_write_data(0x08);		    //select register pointer for Resolution register on temp module
	i2c_write_data(0x00);			//feed resolution register value of 0x00 to indicate +-0.5C
	i2c_stop();
	
	i2c_init();
	i2c_start();
	//write to MCP9808 to set resolution to 0.5C
	i2c_write_to_address(0x19);   //A2-A0 are connected to ground and W/r is cleared to indicate writing
	i2c_write_data(0x08);		    //select register pointer for Resolution register on temp module
	i2c_write_data(0x00);			//feed resolution register value of 0x00 to indicate +-0.5C
	i2c_stop();
	
	i2c_init();
	i2c_start();
	//write to MCP9808 to set resolution to 0.5C
	i2c_write_to_address(0x1a);   //A2-A0 are connected to ground and W/r is cleared to indicate writing
	i2c_write_data(0x08);		    //select register pointer for Resolution register on temp module
	i2c_write_data(0x00);			//feed resolution register value of 0x00 to indicate +-0.5C
	i2c_stop();
	
	ADCSRA = 0b10000111; // 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;  // 0x87 // 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)

    while (1) 
    {   
		
 		i2c_start();
 		i2c_write_to_address(0x18);  // address 0x10 is chosen as an arbitrary secondary address for writing in this example
 		i2c_write_data(0x05);
 		i2c_start();
 		i2c_read_from_address(0x18); // address 0x11 is chosen as an arbitrary secondary address for reading in this example
 		char upper=i2c_read_data(1);
		char lower=i2c_read_data(0);
		Tsensorvalue1 = (upper&0x1f)*16+lower/16;  
 		i2c_stop();
 		
 		i2c_start();
 		i2c_write_to_address(0x19);  // address 0x10 is chosen as an arbitrary secondary address for writing in this example
 		i2c_write_data(0x05);
 		i2c_start();
 		i2c_read_from_address(0x19); // address 0x11 is chosen as an arbitrary secondary address for reading in this example
 		Tsensorvalue2 = (i2c_read_data(1)&0x1f)*16+i2c_read_data(0)/16;  
		i2c_stop();
 	
 		i2c_start();
 		i2c_write_to_address(0x1a);  // address 0x10 is chosen as an arbitrary secondary address for writing in this example
 		i2c_write_data(0x05);
 		i2c_start();
 		i2c_read_from_address(0x1a); // address 0x11 is chosen as an arbitrary secondary address for reading in this example
 		Tsensorvalue3 = (i2c_read_data(1)&0x1f)*16+i2c_read_data(0)/16;  
 		i2c_stop();
		
		
		
 		if ((Tsensorvalue1>Tsensorvalue2)&&(Tsensorvalue1>Tsensorvalue3)&&(Tsensorvalue1>26)){
 			 while(Tsensorvalue1>25){
 				//water pump
 				PORTC= PORTC|0b00001000;
 				//read temperature
 				i2c_start();
 				i2c_write_to_address(0x18);  // Address 0x10 is chosen as an arbitrary secondary address for writing in this example
 				i2c_write_data(0x05);
 				i2c_start();
 				i2c_read_from_address(0x18); // Address 0x11 is chosen as an arbitrary secondary address for reading in this example
 				Tsensorvalue1 = (i2c_read_data(1)&0x1f)*16+i2c_read_data(0)/16;
 				i2c_stop();
 			 }
			  PORTC= 0b00000000;
			  wait(1000,2);
 			 return 0;
 		}else if((Tsensorvalue2>Tsensorvalue1)&&(Tsensorvalue2>Tsensorvalue3)&&(Tsensorvalue2>26)){
 			 turnleft1();
 			 while(Tsensorvalue2>25){
 				 //water pump
 				 PORTC= PORTC|0b00001000;
 				 //read temperature
 				 i2c_start();
 				 i2c_write_to_address(0x19);  // Address 0x10 is chosen as an arbitrary secondary address for writing in this example
 				 i2c_write_data(0x05);
				 i2c_start();
 				 i2c_read_from_address(0x19); // Address 0x11 is chosen as an arbitrary secondary address for reading in this example
 				 Tsensorvalue2 = (i2c_read_data(1)&0x1f)*16+i2c_read_data(0)/16;
 				 i2c_stop();
 			 }
			 PORTC= 0b00000000;
			 wait(1000,2);
			 turnright1();
 			 return 0;
 		}else if((Tsensorvalue3>Tsensorvalue1)&&(Tsensorvalue3>Tsensorvalue1)&&(Tsensorvalue3>26)){
 			 turnright1();
			 while(Tsensorvalue3>25){
 				 //water pump
 				 PORTC= PORTC|0b00001000;
				 //read temperature
 				 i2c_start();
 				 i2c_write_to_address(0x1a);  // Address 0x10 is chosen as an arbitrary secondary address for writing in this example
 				 i2c_write_data(0x05);
 				 i2c_start();
 				 i2c_read_from_address(0x1a); // Address 0x11 is chosen as an arbitrary secondary address for reading in this example
				 Tsensorvalue3 = (i2c_read_data(1)&0x1f)*16+i2c_read_data(0)/16;
 				 i2c_stop();
 			 }
			 PORTC= 0b00000000;
			 wait(1000,2);
			 turnleft1();
 			 return 0;
 		}
		

		while(Tsensorvalue1<26&&Tsensorvalue2<26&&Tsensorvalue3<26){
		
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
		
 		if (sensorvalue1<90){   //no front item 
 			gostraight();
   		}else if(sensorvalue2<30){  //no left item
   			turnleft();
			wait(1500,2);
  		}else if (sensorvalue3<30){   //no right item
   			turnright();
			wait(1500,2);
  	    }else{}
		
	    i2c_start();
 		i2c_write_to_address(0x18);  // address 0x10 is chosen as an arbitrary secondary address for writing in this example
 		i2c_write_data(0x05);
 		i2c_start();
 		i2c_read_from_address(0x18); // address 0x11 is chosen as an arbitrary secondary address for reading in this example
 		char upper=i2c_read_data(1);
		char lower=i2c_read_data(0);
		Tsensorvalue1 = (upper&0x1f)*16+lower/16;  
 		i2c_stop();
 		
 		i2c_start();
 		i2c_write_to_address(0x19);  // address 0x10 is chosen as an arbitrary secondary address for writing in this example
 		i2c_write_data(0x05);
 		i2c_start();
 		i2c_read_from_address(0x19); // address 0x11 is chosen as an arbitrary secondary address for reading in this example
 		Tsensorvalue2 = (i2c_read_data(1)&0x1f)*16+i2c_read_data(0)/16;  
		i2c_stop();
 	
 		i2c_start();
 		i2c_write_to_address(0x1a);  // address 0x10 is chosen as an arbitrary secondary address for writing in this example
 		i2c_write_data(0x05);
 		i2c_start();
 		i2c_read_from_address(0x1a); // address 0x11 is chosen as an arbitrary secondary address for reading in this example
 		Tsensorvalue3 = (i2c_read_data(1)&0x1f)*16+i2c_read_data(0)/16;  
 		i2c_stop();
	    
  	    }
		
	}
}

void turnright(){
	//two motor forward
	//for(int i=0;i<1000;i++){
	PORTD = 0b01100000;
	//wait(1,2);
	//}
}
void turnleft(){
	//for(int i=0;i<1000;i++){
	PORTD = 0b10010000;
	//wait(1,2);
	//}
}
void gostraight(){
	//for(int i=0;i<500;i++){
	PORTD = 0b10100000;
	//}
}

void i2c_init(void) { // initialize i2c
	TWSR = 0b00000000; // prescaler is set to 1
	TWBR = 72; // Put 72 into TWBR to define SCL frequency as 100kHz for 16MHz oscillator
	TWCR = 0b00000100; // TWEN = 1 (enable TWI)
}
void i2c_start(void) { // send start command
	//while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for idle condition -- TWINT bit must be high to proceed -- not needed if single main is used
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //TWCR | 0b10100100;       // initiate START condition -- write 1 to TWINT to clear it and initiate action, set TWSTA bit, set TWEN bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != START) // error checking -- need to predefine START = 0x08 and ERROR() function.
	// ERROR();
}
void i2c_stop(void) { // send stop command
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); //TWCR = TWCR | 0b10010100;       // initiate STOP condition -- write 1 to TWINT to clear it and initiate action, set TWSTO bit and set TWEN bit
}
void i2c_write_to_address( unsigned char address) { //Write an address byte to the I2C2 bus in form of SLA_W (address to write to)
	unsigned char SLA_W = address<<1; // create SLA_W byte by shifting address and leaving R/W bit clear
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = SLA_W;       // Load TWDR with address plus R/W bit
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MT_SLA_ACK) // error checking -- need to predefine MT_SLA_ACK and ERROR() function depending on possible outcomes: 0x18, 0x20, or 0x38.
	//ERROR();
}
void i2c_read_from_address(unsigned char address) { //Write an address byte to the I2C bus in form of SLA_R (address to read from)
	unsigned char SLA_R = address<<1 | 1; // create SLA_R byte by shifting address and setting R/W bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = SLA_R;       // Load TWDR with address plus R/W bit
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MR_SLA_ACK) // error checking -- need to predefine MR_SLA_ACK and ERROR() function depending on possible outcomes: 0x38, 0x40, or 0x48.
	//ERROR();
}
unsigned char i2c_read_data(unsigned char ACK) { //Read a byte of data from a secondary on the I2C2 bus
	unsigned char data;
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	if (ACK) // check for whether ACK or NO_ACK should be sent upon receipt of byte from secondary
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); //TWCR = TWCR | 0b11000100;       // initiate Read with ACK -- write 1 to TWINT to clear it and initiate action, and set TWEA and TWEN
	else
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Read with NO_ACK-- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MR_SLA_ACK) // error checking -- need to predefine MR_SLA_ACK and ERROR() function depending on possible outcomes: 0x50 or 0x58.
	//ERROR();
	//If multiple bytes are to be read, this function can be repeated with proper ACK or NO_ACK until done.
	data = TWDR;  // read the received data from secondary
	return(data);
}
void i2c_write_data( unsigned char data) { //Write data byte to the I2C2 bus
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = data;       // Load TWDR with data to be sent
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MT_DATA_ACK) // error checking -- need to predefine MT_DATA_ACK and ERROR() function depending on possible outcomes: 0x28 or 0x30.
	//ERROR();
}

void turnright1(){
	for (int i=0;i<12;i++)
	{
		step_CCW(); //step_CCW()
		wait(100, 2);
	}
}
void turnleft1(){
	for (int i=0;i<12;i++)
	{
		step_CW(); //step_CCW()
		wait(100, 2);
	}
}
void step_CCW()
{
	// This function advances the motor counter-clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course. phase1a = PORTD_7, phase1b = PORTD_6
	// phase2a = PORTD_5, phase2b = PORTD_4
	switch (phase_step) {
		case 1:
		// step to 2
		PORTD = 0b00000001;
		phase_step = 2;
		break;
		case 2:
		// step to 3
		PORTD = 0b00000100;
		phase_step = 3;
		break;
		case 3:
		// step to 4;
		PORTD = 0b00000010;
		phase_step = 4;
		break;
		case 4:
		// step to 1;
		PORTD = 0b00001000;
		phase_step = 1;
	    break;  }
}// end step_CCW
void step_CW() {
	// This function advances the motor clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course. Our stepper motor phases are assigned to Port pins as follows: phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5, phase2b = PORTD_4
	switch (phase_step) {
		case 1:
		// step to 4
		PORTD = 0b00000010;
		phase_step = 4;
		break;
		case 2:
		// step to 1
		PORTD = 0b00001000;
		phase_step = 1;
		break;
		case 3:
		// step to 2;
		PORTD = 0b00000001;
		phase_step = 2;
		break;
		case 4:
		// step to 3;
		PORTD = 0b00000100;
		phase_step = 3;
	break;  }
}// end step_CW
void wait(volatile int multiple, volatile char time_choice) {
	/* This subroutine calls others to create a delay.
		 Total delay = multiple*T, where T is in msec and is the delay created by the called function.
	
		Inputs: multiple = number of multiples to delay, where multiple is the number of times an actual delay loop is called.
		Outputs: None
	*/
	
	while (multiple > 0) {
		delay_T_msec_timer0(time_choice); 
		multiple--;
	}
} // end wait()
void delay_T_msec_timer0(volatile char choice) {
    //*** delay T ms **
    /* This subroutine creates a delay of T msec using TIMER0 with prescaler on clock, where, for a 16MHz clock:
    		for Choice = 1: T = 0.125 msec for prescaler set to 8 and count of 250 (preload counter with 5)
    		for Choice = 2: T = 1 msec for prescaler set to 64 and count of 250 (preload counter with 5)
    		for Choice = 3: T = 4 msec for prescaler set to 256 and count of 250 (preload counter with 5)
    		for Choice = 4: T = 16 msec for prescaler set to 1,024 and count of 250 (preload counter with 5)
			for Choice = Default: T = .0156 msec for no prescaler and count of 250 (preload counter with 5)
	
			Inputs: None
			Outputs: None
	*/
	
	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	TCNT0 = 0;  // preload value for testing on count = 250
	// preload value for alternate test on while loop: TCNT0 = 5;  // preload load TIMER0  (count must reach 255-5 = 250)
	
	switch ( choice ) { // choose prescaler
		case 1:
			TCCR0B = 0b00000010; //1<<CS01;	TCCR0B = 0x02; // Start TIMER0, Normal mode, crystal clock, prescaler = 8
		break;
		case 2:
			TCCR0B =  0b00000011; //1<<CS01 | 1<<CS00;	TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
		break;
		case 3:
			TCCR0B = 0b00000100; //1<<CS02;	TCCR0B = 0x04; // Start TIMER0, Normal mode, crystal clock, prescaler = 256
		break; 
		case 4:
			TCCR0B = 0b00000101; //1<<CS02 | 1<<CS00; TCCR0B = 0x05; // Start TIMER0, Normal mode, crystal clock, prescaler = 1024
		break;
		default:
			TCCR0B = 0b00000001; //1<<CS00; TCCR0B = 0x01; Start TIMER0, Normal mode, crystal clock, no prescaler
		break;
	}
	
	while (TCNT0 < 0xFA); // exits when count = 250 (requires preload of 0 to make count = 250)
	// alternate test on while loop: while ((TIFR0 & (0x1<<TOV0)) == 0); // wait for TOV0 to roll over:
	// How does this while loop work?? See notes
	
	TCCR0B = 0x00; // Stop TIMER0
	//TIFR0 = 0x1<<TOV0;  // Alternate while loop: Clear TOV0 (note that this is a nonintuitive bit in that it is cleared by writing a 1 to it)
	
} // end delay_T_msec_timer0()

