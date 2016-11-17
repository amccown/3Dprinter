/*
 * platformTroll.c
 * 
 * created on November 6, 2016
 * 
 * Alex McCown (c) all rights reserved.  
 * 
 * 
 * 
 * This is the C file for the 3D printer platform height controller
 * 
 * The primary purpose of this library is to perform height changes based on 
 * inputs given.  This is provides a higher level interface, which will be 
 * used by the module interpreting the g-code commands.  
 */

#include "platformTroll.h"
#include "Arduino.h"
#include <stdbool.h>


// This reverses the 'up' and 'down' orientation.  
const bool REVERSE_DIRECTION = true;

#define LOW false
#define HIGH true


// Constants used in calculating the number of steps required
#define SCREW_LENGTH (38)  // this is in 1/10ths of a mm
#define MAX_STEPS (2000)		// corresponding to SCREW_LENGTH


//Variables for pin information
uint8_t stp =2;
uint8_t dir =3;
uint8_t MS1 =4;
uint8_t MS2 =5;
uint8_t EN  =6;

//Variable for steps
int16_t steps_so_far;




/*
 * Helper Function 
 * 
 * Reset Easy Driver pins to default states
 */
void resetEDPins()
{
  digitalWrite(stp, LOW);       // say "don't step"
  digitalWrite(dir, LOW);       // set direction to forward
  digitalWrite(MS1, HIGH);      //Pull MS1, and MS2 high to set logic to 1/8th microstep resolution
  digitalWrite(MS2, HIGH);
  digitalWrite(EN, LOW);       //enable line
}


/*
 * Helper function
 * 
 * Low level function for stepping in the direction the board is currently set to
 * Does not alter the pin controlling the direction in order to minimize delays 
 * while looping. 
 */
void Stepper_step()
{
    digitalWrite(stp,HIGH); //Trigger one step forward
    delay(3);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
}



 
/*
 * Initialization function for the platformTroll
 * 
 * Mainly calls the reset function, sets the step to default 
 *  1/8 th in this case, and loads the pin# values for the various
 *  pins we'll need to control the stepper motor driver.  
 *  
 *  @params:  stp_pin, dir_pin, MS1_pin, MS2_pin, EN_pin.  All uint8_t's
 *  default to 2, 3, 4, 5, 6
 */
 void platformTroll_init(uint8_t stp_pin, uint8_t dir_pin, uint8_t MS1_pin, uint8_t MS2_pin, uint8_t EN_pin)
{
    stp = stp_pin;
    dir = dir_pin;
    MS1 = MS1_pin;
    MS2 = MS2_pin;
    EN  = EN_pin;

    resetEDPins();
    steps_so_far = 0;
 }


/*
 * platformTroll_shutDown();
 * 
 * Shuts down the motor power, but raising the enable flag.
 * Eventually this might do more, but for now just deactivates
 * the stepper motor driver.
 */

void platformTroll_shutDown()
{
  digitalWrite(EN, HIGH);   // low-asserted, so high means off
}



/*
 * Moves the platform Up, by a certain number of steps
 * 
 * @params:  num_of_steps.  uint16_t
 */
 void platformTroll_stepUp(uint32_t num_of_steps)
 {
    steps_so_far+= num_of_steps;          // update position marker
    digitalWrite(dir, REVERSE_DIRECTION); // Pull direction pin where it needs to be
    for(uint32_t i= 0; i<num_of_steps; i++)        // Loop the stepping
      Stepper_step();                     // call our low level function 
 }


/*
 * Moves the platform down, by a certain number of steps
 * 
 * @params:  num_of_steps.  uint16_t
 */
 void platformTroll_stepDown(uint32_t num_of_steps)
 {
    steps_so_far+= num_of_steps;            // update position marker
    digitalWrite(dir, !REVERSE_DIRECTION);  // Pull direction pin where it needs to be
    for(uint32_t i= 0; i<num_of_steps; i++)          // Loop the stepping
      Stepper_step();                       // call our low level function 
 }


 /*
  * Moves the platform Up, by the height given
  *
  * @params :  height_of_layer -- height in 1/10 of mm.
  */
 void platformTroll_layerUp(float height_of_layer)
 {
	uint32_t steps_to_go = height_of_layer;
	 steps_to_go = (steps_to_go*MAX_STEPS) / SCREW_LENGTH;
	 platformTroll_stepUp(steps_to_go);
 }


 /*
  * Moves the platform down, by the height given
  *
  * @params :  height_of_layer -- height in mm.
  * ie, layerDown(03) means go down by 0.3mm and
  * layerDown(52) would mean go down by 5.2mm
  */
 void platformTroll_layerDown(float height_of_layer)
 {
	 //first we multiply by the STEPS per length, then divide by length
	//uint32_t steps_to_go = height_of_layer;
	 //steps_to_go = (steps_to_go*MAX_STEPS) / SCREW_LENGTH;
	float steps_to_go_f = (height_of_layer*MAX_STEPS) / SCREW_LENGTH;
	uint16_t steps_to_go = steps_to_go_f;
	Serial.println(steps_to_go);
	 platformTroll_stepDown(steps_to_go); // call our step function (pun intended)
 }


/*
 * Returns the platform to it's starting position.  This is the position it was 
 * at when the _init() function was last called.  
 * 
 * @params:  NONE
 */
void platformTroll_returnToStart()
{
  digitalWrite(dir, REVERSE_DIRECTION); //Pull direction pin low to move "reverse"
  while(steps_so_far> 0)  //Loop the forward stepping
  {
    Stepper_step();       // low level function 
    steps_so_far--;
  }
  digitalWrite(dir, !REVERSE_DIRECTION); //Pull direction pin low to move "forward"
  while(steps_so_far< 0)  //Loop the forward stepping
  {
    Stepper_step();       // low level function 
    steps_so_far++;
  }
}


