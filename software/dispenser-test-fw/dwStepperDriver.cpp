/*
 * dwStepperDriver.h - Stepper library for Arduino
 *
 * Based on Arduino Stepper Library
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Drives a bipolar stepper motor.
 *
 *
 */

#include "Arduino.h"
#include "dwStepperDriver.h"

#define PULSE_WIDTH 5
#define DELAY_MILLI 100

/*
 * constructor.
 *
 * number_of_steps: full steps per revolution of the motor shaft
 * microstep_size:  microstepping size (i.e. number of microsteps for each full step
 * dir_pin:         pin connected to DIR signal
 * step_pin:        pin connected to STEP signal
 * home_pin:        pin connected to sensor used for homing.
 * 
 * Note:
 *  a. the code assumes sensor reads LOW in home position (e.g an open collector sensor);
 *  b. uint16_t _number_of_microsteps assumes (this->_number_of_steps) < 65K
 */
dwStepperDriver::dwStepperDriver(uint16_t number_of_steps, StepSize microstep_size, uint8_t dir_pin, uint8_t step_pin, uint8_t home_pin)
{
  this->_microstep_size = microstep_size;
  this->_number_of_microsteps = (number_of_steps * this->_microstep_size); // total number of microsteps for this motor

  // Arduino pins for the motor control connection:
  this->_dir_pin = dir_pin;
  this->_step_pin = step_pin;
  this->_home_pin = home_pin;
  
  // setup the pins on the microcontroller:
  pinMode(this->_dir_pin, OUTPUT);
  pinMode(this->_step_pin, OUTPUT);
  pinMode(this->_home_pin, INPUT_PULLUP);
  //
  this->setRPM(1);
}

/**************************************************************
 * Sets the speed in Revs Per Minute (RPM)
 **************************************************************/
void dwStepperDriver::setRPM(uint16_t rpm)
{
  //compute delay in microseconds
  this->_step_delay = (60L * 1000L * 1000L) / (this->_number_of_microsteps * rpm);
}

/**************************************************************
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 **************************************************************/
void dwStepperDriver::step(int16_t number_of_steps)
{
  uint16_t steps = abs(number_of_steps);  // how many steps to take
  
  // determine direction based on whether steps_to_mode is + or -:
  digitalWrite(this->_dir_pin, number_of_steps >= 0 ? HIGH : LOW );
  
  for(uint16_t i = 0L; i < steps; i++){
    this->stepMotor();
    delayMicroseconds(this->_step_delay);
  }
}

/**************************************************************
 * Move the motor to home position
 **************************************************************/
bool dwStepperDriver::home()
{
  uint8_t val_Home, previous_val_Home;
  uint16_t step1, step2, backtrack;
  uint16_t steps;

  //leave area near home...
  steps = 0;
  while ((steps < this->_number_of_microsteps) && (LOW == digitalRead(this->_home_pin))) {
    this->step(-1);
    steps++;
  }

  //if sensor reads LOW for an entire revolution, something must be wrong
  if(steps == this->_number_of_microsteps) return false;

  //at the end of previous while loop, home sensor must read HIGH
  delay(DELAY_MILLI);
  previous_val_Home = HIGH;
  step1 = step2 = 0;
  //bounds included to make sure we've returned to original position
  for (uint16_t i = 0; i <= this->_number_of_microsteps; i++) {
    val_Home = digitalRead(this->_home_pin);
   
    if (val_Home != previous_val_Home) {        
      //sensor read changed
      previous_val_Home = val_Home;
      if (LOW == val_Home) {
        step1 = i;
      } else {
        //i.e. HIGH == val_Home
        step2 = i;
        break;
      }
    }
    
    this->step(1);  
  }
  //if at least one of the positions was not identified, something must be wrong
  if((0 == step1) || (0 == step2)) return false;

  //number of steps to backtrack
  delay(DELAY_MILLI);
  backtrack = (step2 - step1) >> 1;
  for (uint16_t i = 0; i <= backtrack; i++) {
    this->step(-1);  
  }  
  
  return true;
}

/**************************************************************
 * rotate thousands_of_turn e.g. 500 = 0.5 turn = 180° 
 * If the number is negative, the motor moves in the reverse direction.
 **************************************************************/
void dwStepperDriver::rotate(int16_t thousands_of_turn){
  int32_t steps = ((int32_t)this->_number_of_microsteps * (int32_t)thousands_of_turn) / 1000;
  this->step((int16_t)steps);  
}

/***************************************************************
 * stepMotor()
 * advances motor one step
 * 
 * pin: microcontroller pin tied to STEP pin of A4988.     
 * 
 * Note from A4988 datasheet:
 * STEP minimum, HIGH pulse widtht 1μs
 * STEP minimum, LOW  pulse widtht 1μs
 ***************************************************************/
void dwStepperDriver::stepMotor()
{
  digitalWrite(this->_step_pin, HIGH);
  delayMicroseconds(PULSE_WIDTH);
  digitalWrite(this->_step_pin, LOW);
  delayMicroseconds(PULSE_WIDTH);
}
