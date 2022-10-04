// In this NanoJ program, we will make the motor turn shortly back and forth.
// The language used for NanoJ programs is C, with a few specific extensions,
// like the mappings (see below).
// Please refer to the product manual for more details about NanoJ and the
// object dictionary.

// You can map frequently used objects to be able to read or write them
// using In.* and Out.*. Here we map the object 6041:00 as "In.StatusWord".

// Minimum required mapping for nanotec.h
map U16 Controlword as inout 0x6040:00
map U16 Statusword as input 0x6041:00
map U32 Inputs as input 0x60FD:00
map U32 Outputs as inout 0x60FE:01
map S08 ModesOfOperation as output 0x6060:00
map S08 ModesOfOperationDisplay as input 0x6061:00
map S16 AnalogInput as input 0x3220:01
map S16 TargetVelocity as output 0x6042:00  
map S32 ProfileVelocity as output 0x60FF:00   
 // Example shows register 6042 - vl Target Velocity (USER DEFINED UNITS)
 // Example shows register 60FF -  Target Velocity/Profile Velocity WHAT IS THE DIFFERENCE?? WHO KNOWS
// Additional Mapping Added given examples


int analogValue;

int stopButtonState;


int direction;   // when true, operation clockwise
bool stop;        // when true, motor stops and program ends. acts as emergency stop. Cycle Power to reset

int motorRatedCurrent;   // This will give the absolute value that everything else is calculated as a percentage of

long currentA;
long currentB;
long currentD;
long currentQ;

long A;   // For doing maths on individual current readings
long B;
long D;


long i; //
long j;  // Hold the max current from either a or b & d or q - these might need to swap after i.e. a and d or b and q

//long coilCurrent[4];

long U;   // For doing maths on current

long T;  // torque as a percentage of max current
long Q;  // Calculated torque value in uNm


// Include the definition of NanoJ functions and symbols
#include "wrapper.h"
#include "nanotec.h"



// The user() function is the entry point of the NanoJ program. It is called
// by the firmware of the controller when the NanoJ program is started.
void user(){
	
// SETUP FUNCTIONS
	sleep(2000);   // Pause on power on
	// Set mode "Profile velocity"
	
	
	// Reset our software stop incase it has been used
	od_write(0x2400, 0x0A, 00);
	
	Out.ModesOfOperation = 3;
	//od_write(0x6060, 0x00, 3);  // Why was this a 3? I dont know cant find reference to it anywhere maybe difference between profile and target velocity
	
	//Max motor Current - defined by datasheet is 4.2A - limiting to 3000mA
	od_write(0x2031, 00, 3000);
	// This equates to a max torque of WHO TF KNOWS BECAUSE ITS NOT DOCUMENTED
	
	// Setting Units! Eventually wewish for this to operate in RPM or RPs
	//https://en.nanotec.com/products/manual/PD4C_USB_EN/general%2Fuser_units.html?cHash=588ff72d5092a3a741b97978c5945e28#id121JB0S30UI
	// its going to be a combination of 
	//60A8 - Position Unit - Value B4 Mechanical Revolution
	//60A9 - speed Unit - Value 03 - second, or 47 - minute
	// ALSO takes into account Feed constant 6092, and Gear Ratio, 6091
	
	//SLOW SPEED MODE! 
	// ON reading more it seems that SLOW SPEED MODE + closed loop is required. THis code will go here
	//To use the new parameters, you must set 3210h:07h (for closed loop) Does this make sense? Not really don think that worked
	// Set bit 0 in 0x3202 - Should be done by auto-setup but isnt for whatever reason
	// We also need to set bit 7 of 3202 to turn on slow mode. this can be done in one go by writing 129 or 0x81 to 3202
	// NOTE ABOVE MAY BE WRONG, SLOW SPEED MODE IS CRAP and NOISY
	// Instead set bit 0, this is closed loop but no slow speed mode
	od_write(0x3202, 0x00, 1);
	yield();
	// Bit 15 of statusword 0x6041 should == 1 to show closed loop mode available but no way of checking internally
	
	
	// Now controller is in closed loop mode, we need to set the PID controller correctly.
	//320E:07=0 to turn integral part of velocity controller to 0
	//od_write(0x320E, 07, 0);
	// HAS LINE ABOVE CHANGED DUE TO FIRMWARE CHANGE?
	od_write(0x321B, 0x02, 0);
	
	yield();
	// Also set Motor drive parameter set - Velocity loop, integral gain (closed loop) 3210:04=500 < Dont know if different
	// Setting to 0 sucked
	od_write(0x3210, 0x04, 500);   // When this isnt set like this motor lacks power?
	
	yield();
	
	// Feed forward - I think should be set to 100% or ~0%<< I think 0 works best 320E:03=0
	//od_write(0x320E, 03, 0);
	od_write(0x3210, 0x0B, 0);
	yield();
	
	

	// Set the target velocity to 0	
	Out.ProfileVelocity = 0;
	//od_write(0x60FF, 0x00, 0);	
	yield();
	
 	// Request state "Ready to switch on" OR "ENABLE VOLTAGE"
 	InOut.Controlword = 0x6;
	//od_write(0x6040, 0x00, 0x6);
	// Wait until the requested state is reached
	do {
		yield();
		}
		while ( (In.Statusword & 0xEF) != 0x21);
		//// checking the statusword (0x6041) for the bitmask: xxxx xxxx x01x 0001

	// Request the state "Switched on"
	InOut.Controlword = 0x7;				// switch to the "switched on" state
	//od_write(0x6040, 0x00, 0x7);
	// Wait until the requested state is reached
	do {
		yield();
	} while ( (In.Statusword & 0xEF) != 0x23);
	// checking the statusword (0x6041) for the bitmask: xxxx xxxx x01x 0011
	
	
//Test Functions: Spin the motor to show software is working - Comment out for deployment

/*	
	// Write a small value to the target velocity
	Out.ProfileVelocity = 50;
	yield();
		// Request the state "Operation enabled"
	InOut.Controlword = 0xF;				// switch to the "enable operation" state and starts the velocity mode
	// Wait until the requested state is reached
	do {
	yield();
	}
	while ( (In.Statusword & 0xEF) != 0x27); 
	// checking the statusword (0x6041) for the bitmask: xxxx xxxx x01x 0111
	
	sleep(500);
	
	// Write a small value to the target velocity
	Out.ProfileVelocity = -50;	
	yield();
	
	sleep(500);
	// Set the motor back to "Switched On" i.e. ready but not active
	InOut.Controlword = 0x7;				// switch to the "switched on" state
	do {
		yield();
	} while ( (In.Statusword & 0xEF) != 0x23);
	// Reset Target velocity to 0
	Out.ProfileVelocity = 0;
	yield();
	
*/
	
	// Do some maths for calculating current draws
	motorRatedCurrent = od_read(0x2031, 0x00);
	
	

// LOOP FUNCTIONS

	//while (1){   // Loop for infiniate looping
	//Loop for control with stop button
	direction = 1;
	stop = false;
	while(stop != true){
		
		// Check the stop Button			
		if(DigitalInput(3)){
			stop = true;
		}
		
		// Use an additional IO register to provide a stop clause from labview software
		
		if (od_read(0x2400, 0x0A) == 1){
			stop = true;
		}
			
		
		
	// Read the current I values	
		//A = od_read(0x2039,0x03);
		//B = od_read(0x2039,0x04);
		T = od_read(0x6077, 0x00);   // Torque actual value as a percentage of max current.
		
		T = T/10;       //make into percentage of max torque
		
		// Do some maths if required on torque value
				
		// Write value into register for oscilloscope 
		od_write(0x2500, 0x02, T);
		yield();
			

		
		
	
	
	//Read Digital direction input
		if (DigitalInput(5)){
		direction = 1;
		} else {
		direction = -1; // This should work I really dont understand what the issue is with this shite
		}
	
	//Read the Analog Input	
		//analogValue = od_read(0x3320, 0x01);     // Returns a value from -500 to -1500
		analogValue = In.AnalogInput;              // Returns a value from 0 to 1000
                 
		A = analogValue * direction;              // Scales based on direction input

		B = A/10;			// Now has range of 0 to 100, same operation as before
		
		//analogValue = analogValue - 100;           // Rescales for joystick operation, remove if direction input used
		//Write the analog value to a register to be read by the software
		od_write(0x2500, 0x01, B);

	// Write the scaled analog value to the target velocity
		Out.ProfileVelocity = B;
		//od_write(0x60FF, 0x00, analogValue);
		yield();

		
	// If enable pin has been pressed
		//if (buttonState == 524288){
		if (DigitalInput(4)){
	// Request the state "Operation enabled"
	// Request the state "Operation enabled"
			InOut.Controlword = 0xF;	
			//od_write(0x6040, 0x00, 0xF);
	// Wait until the requested state is reached
			while ( (In.Statusword & 0xEF) != 0x27) {
				yield();
			}
		} else {
		// Request the state "Switched on" - I.E Stop the motor but dont "STOP" the motor
			// Request the state "Operation enabled"
			InOut.Controlword = 0x7;	
			//od_write(0x6040, 0x00, 0x7);
			// Wait until the requested state is reached
			while ( (In.Statusword & 0xEF) != 0x23) {
				yield();
				}
		}
		yield();		
	}
	
// EXIT CASE
	// Stop the motor
	Halt(true);
	//605D:00 = 1  ?? Doesnt seemt o work
	
	
	// Request state "Ready to switch on" OR "ENABLE VOLTAGE"
 	InOut.Controlword = 0x0; 	
	//od_write(0x6040, 0x00, 0x0);	
	do {
		yield();
		}
		while ( (In.Statusword & 0xEF) != 0x00);
		//// checking the statusword (0x6041) for the bitmask: xxxx xxxx x00x 0000
	
	
	// Stop the NanoJ program. Without this line, the firmware would
	// call user() again as soon as we return.
	// NOTE: This line can be used to stop the nanoJ software at any point, which would let labview take over control entirely
	od_write(0x2300, 0x00, 0x0);
}
