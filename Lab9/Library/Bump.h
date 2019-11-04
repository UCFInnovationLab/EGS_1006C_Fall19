/**
 * @file      Bump.h
 * @brief     Provide low-level functions that interface bump switches the robot.
 * @details   Six switches are connected to P8.7-P8.3, P8.0<br>
 1) Hardware uses negative logic with internal pullup<br>
 2) Positioned on the front of the robot to detect collisions<br>
 3) Software returns 6-bit positive logic (1 means collision)<br>
 * @version   V1.0
 * @author    Valvano
 * @copyright Copyright 2017 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      September 4, 2017
 *
<table>
<caption id="Bump_ports8">Six Bump sensors if using CC3120 wifi booster</caption>
<tr><th>Pin  <th>Sensor
<tr><th>P8.7 <td>Bump5, left side of robot
<tr><th>P8.6 <td>Bump4
<tr><th>P8.5 <td>Bump3
<tr><th>P8.4 <td>Bump2
<tr><th>P8.3 <td>Bump1
<tr><th>P8.0 <td>Bump0, right side of robot
</table>

<table>
<caption id="Bump_ports4">Six Bump sensors if you plan to do Lab 14 edge triggered interrupts</caption>
<tr><th>Pin  <th>Sensor
<tr><th>P4.7 <td>Bump5, left side of robot
<tr><th>P4.6 <td>Bump4
<tr><th>P4.5 <td>Bump3
<tr><th>P4.3 <td>Bump2
<tr><th>P4.2 <td>Bump1
<tr><th>P4.0 <td>Bump0, right side of robot
</table>
 ******************************************************************************/

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

#define BUMP_SWITCH(d,p) !((d>>p)&0x01);

/**
 * Initialize Bump sensors<br>
 * Make P8.7-P8.3, P8.0 as inputs<br>
 * or Port 4 pins 7,6,5,3,2,0 as inputs<br>
 *
 * Activate interface pullup
 * @param none
 * @return none
 * @brief  Initialize Bump sensors
 */
void Bump_Init(void);

/**
 * Read current state of 6 switches<br>
 * Read P8.7-P8.3, P8.0 inputs<br>
 * or read Port 4 pins 7,6,5,3,2,0 inputs<br>
 * Returns a 6-bit positive logic result (0 to 63)<br>
 * bit 5 Bump5<br>
 * bit 4 Bump4<br>
 * bit 3 Bump3<br>
 * bit 2 Bump2<br>
 * bit 1 Bump1<br>
 * bit 0 Bump0
 * @param none
 * @return result is 6-bit positive logic
 * @note  result is a packed, right-justified, positive logic
 * @brief  Read current state of 6 switches
 */
uint8_t Bump_Read(void);

