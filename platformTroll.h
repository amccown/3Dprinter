/*
 * platformTroll.h 
 * 
 * created on November 6, 2016
 * 
 * Alex McCown (c) all rights reserved.  
 * 
 * 
 * 
 * This is the header file for the 3D printer platform height controller
 * 
 * The primary purpose of this library is to perform height changes based on 
 * inputs given.  This is provides a higher level interface, which will be 
 * used by the module interpreting the g-code commands.  
 

#ifndef platformTroll_h
#define platformTroll_h
*/


#include <stdint.h>


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
 void platformTroll_init(uint8_t stp_pin, uint8_t dir_pin, uint8_t MS1_pin, uint8_t MS2_pin, uint8_t EN_pin);


/*
 * platformTroll_shutDown();
 * 
 * Shuts down the motor power, but raising the enable flag.
 * Eventually this might do more, but for now just deactivates
 * the stepper motor driver.
 */

void platformTroll_shutDown();



/*
 * Moves the platform Up, by a certain number of steps
 * 
 * @params:  num_of_steps.  uint16_t
 */
 void platformTroll_stepUp(uint32_t num_of_steps);


/*
 * Moves the platform down, by a certain number of steps
 * 
 * @params:  num_of_steps.  uint16_t
 */
 void platformTroll_stepDown(uint32_t num_of_steps);


 /*
  * Moves teh platform Up, by the height given
  *
  * @params :  height_of_layer -- height in 1/10 of mm.
  */
 void platformTroll_layerUp(float height_of_layer);


 /*
  * Moves teh platform down, by the height given
  *
  * @params :  height_of_layer -- height in 1/10 of mm.
  */
 void platformTroll_layerDown(float height_of_layer);

/*
 * Returns the platform to it's starting position.  This is the position it was 
 * at when the _init() function was last called.  
 * 
 * @params:  NONE
 */
 void platformTroll_returnToStart();




