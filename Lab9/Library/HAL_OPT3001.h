/* --COPYRIGHT--,BSD
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//****************************************************************************
//
// HAL_OPT3001.h - Prototypes of hardware abstraction layer for interfacing OPT3001
//
//****************************************************************************

#ifndef __HAL_OPT3001_H_
#define __HAL_OPT3001_H_

/*CONSTANTS*/
#define OPT3001_SLAVE_ADDRESS 0x47

#define OPT_INTERRUPT_PIN 11
#define RESULT_REG 0x00
#define CONFIG_REG 0x01
#define LOWLIMIT_REG 0x02
#define HIGHLIMIT_REG 0x03
#define MANUFACTUREID_REG 0x7E
#define DEVICEID_REG 0x7F

#define DEFAULT_CONFIG 0xCC10 // 800ms
#define DEFAULT_CONFIG_100 0xC410 // 100ms

/* CONFIG REGISTER BITS: RN3 RN2 RN1 RN0 CT M1 M0 OVF CRF FH FL L Pol ME FC1 FC0
RN3 to RN0 = Range select:
1100 by default, enables auto-range
CT = Conversion time bit
0 = 100ms conversion time
1 = 800ms conversion time (default)
M1 to M0 = Mode bits
00 = Shutdown mode
01 = Single shot mode
10 = Continuous conversion (default)
11 = Continuous conversion
OVF (Bit 8) � Overflow flag. When set the conversion result is overflown.
CRF (Bit 7) � Conversion ready flag. Sets at end of conversion. Clears by read or write of the Configuration register.
FH (Bit 6) � Flag high bit. Read only. Sets when result is higher that TH register. Clears when Config register is
read or when Latch bit is �0� and the result goes bellow TH register.
FL (Bit 5) � Flag low bit. Read only. Sets when result is lower that TL register. Clears when Config register is read
or when Latch bit is �0� and the result goes above TL register.
L (Bit 4) � Latch bit. Read/write bit. Default �1�, Controls Latch/transparent functionality of FH and FL bits. When
L = 1 the Alert pin works in window comparator mode with Latched functionality When L = 0 the Alert pin
works in transparent mode and the two limit registers provide the hysteresis.
Pol (Bit 3) � Polarity. Read/write bit. Default �0�, Controls the active state of the Alert pin. Pol = 0 means Alert
active low.
ME (Bit 2) � Exponent mask. In fixed range modes masks the exponent bits in the result register to �0000�.
FC1 to FC0 - Fault count bits. Read/write bits. Default �00� - the first fault will trigger the alert pin.
*/

void OPT3001_init(void);
unsigned long int OPT3001_getLux(void);
unsigned int OPT3001_readManufacturerId(void);
unsigned int OPT3001_readDeviceId(void);
unsigned int OPT3001_readConfigReg(void);
unsigned int OPT3001_readLowLimitReg(void);
unsigned int OPT3001_readHighLimitReg(void);

#endif /* __HAL_OPT3001_H_ */
