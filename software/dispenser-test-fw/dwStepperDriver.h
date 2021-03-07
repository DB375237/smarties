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

// ensure this library description is only included once
#ifndef dwStepperDriver_h
#define dwStepperDriver_h

// library interface description
class dwStepperDriver{
  public:
    typedef enum {
      FULL_STEP = 1,
      HALF_STEP = 2,
      QUARTER_STEP = 4,
      EIGHTH_STEP = 8,
      SIXTEENTH_STEP = 16
    } StepSize;

    // constructors:
    dwStepperDriver(uint16_t number_of_steps, StepSize microstep_size, uint8_t dir_pin, uint8_t step_pin, uint8_t home_pin);

    // speed setter method:
    void setRPM(uint16_t rpm);

    //move motor to home position
    bool home();

    // mover method:
    void step(int16_t number_of_steps);
    void rotate(int16_t thousands_of_turn); //e.g. 50 = 0.5 turn = 180° 
    

  private:
    uint32_t _step_delay;             // delay between steps, in microseconds, based on speed
    uint16_t _number_of_microsteps;   // total number of microsteps this motor can take
    StepSize _microstep_size;         // microsteppping size

    //motor pin numbers:
    uint8_t _dir_pin;
    uint8_t _step_pin;
    uint8_t _home_pin;
    
    void stepMotor();
};

#endif //dwStepperDriver_h
