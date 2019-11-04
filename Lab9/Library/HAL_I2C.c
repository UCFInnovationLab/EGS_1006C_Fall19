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
// HAL_I2C.c - Hardware abstraction layer for I2C with MSP432P401R
//
//****************************************************************************

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "HAL_I2C.h"


/* I2C Master Configuration Parameter */
const eUSCI_I2C_MasterConfig i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        48000000,                               // SMCLK = 48MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 100khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void Init_I2C_GPIO()
{
    /* Select I2C function for I2C_SCL(P6.5) & I2C_SDA(P6.4) */
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P6,
            GPIO_PIN5,
            GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P6,
            GPIO_PIN4,
            GPIO_PRIMARY_MODULE_FUNCTION);
}


/***************************************************************************//**
 * @brief  Configures I2C
 * @param  none
 * @return none
 ******************************************************************************/

void I2C_init(void)
{
        /* Initialize USCI_B0 and I2C Master to communicate with slave devices*/
    I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);

    /* Disable I2C module to make changes */
    I2C_disableModule(EUSCI_B1_BASE);

    /* Enable I2C Module to start operations */
    I2C_enableModule(EUSCI_B1_BASE);

    return;
}


/***************************************************************************//**
 * @brief  Reads data from the sensor
 * @param  writeByte Address of register to read from
 * @return Register contents
 ******************************************************************************/

int I2C_read16(unsigned char writeByte)
{
    int val = 0;
    int valScratch = 0;

    /* Set master to transmit mode PL */
    I2C_setMode(EUSCI_B1_BASE,
        EUSCI_B_I2C_TRANSMIT_MODE);

    /* Clear any existing interrupt flag PL */
    I2C_clearInterruptFlag(EUSCI_B1_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Wait until ready to write PL */
    while (I2C_isBusBusy(EUSCI_B1_BASE));

    /* Initiate start and send first character */
    I2C_masterSendMultiByteStart(EUSCI_B1_BASE, writeByte);

    /* Wait for TX to finish */
    while(!(I2C_getInterruptStatus(EUSCI_B1_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0)));

    /* Initiate stop only */
    I2C_masterSendMultiByteStop(EUSCI_B1_BASE);

    /* Wait for Stop to finish */
    while(!I2C_getInterruptStatus(EUSCI_B1_BASE,
        EUSCI_B_I2C_STOP_INTERRUPT));

    /*
     * Generate Start condition and set it to receive mode.
     * This sends out the slave address and continues to read
     * until you issue a STOP
     */
    I2C_masterReceiveStart(EUSCI_B1_BASE);

    /* Wait for RX buffer to fill */
    while(!(I2C_getInterruptStatus(EUSCI_B1_BASE,
        EUSCI_B_I2C_RECEIVE_INTERRUPT0)));

    /* Read from I2C RX register */
    val = I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);

    /* Receive second byte then send STOP condition */
    valScratch = I2C_masterReceiveMultiByteFinish(EUSCI_B1_BASE);

    /* Shift val to top MSB */
    val = (val << 8);

    /* Read from I2C RX Register and write to LSB of val */
    val |= valScratch;

    /* Return temperature value */
    return (int16_t)val;
}


/***************************************************************************//**
 * @brief  Writes data to the sensor
 * @param  pointer  Address of register you want to modify
 * @param  writeByte Data to be written to the specified register
 * @return none
 ******************************************************************************/

void I2C_write16 (unsigned char pointer, unsigned int writeByte)
{
    /* Set master to transmit mode PL */
    I2C_setMode(EUSCI_B1_BASE,
        EUSCI_B_I2C_TRANSMIT_MODE);

    /* Clear any existing interrupt flag PL */
    I2C_clearInterruptFlag(EUSCI_B1_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    /* Wait until ready to write PL */
    while (I2C_isBusBusy(EUSCI_B1_BASE));

    /* Initiate start and send first character */
    I2C_masterSendMultiByteStart(EUSCI_B1_BASE,
        pointer);

    /* Send the MSB to SENSOR */
    I2C_masterSendMultiByteNext(EUSCI_B1_BASE,
        (unsigned char)(writeByte>>8));

    I2C_masterSendMultiByteFinish(EUSCI_B1_BASE,
        (unsigned char)(writeByte&0xFF));

}


void I2C_setslave(unsigned int slaveAdr)
{
    /* Specify slave address for I2C */
    I2C_setSlaveAddress(EUSCI_B1_BASE,
        slaveAdr);

    /* Enable and clear the interrupt flag */
    I2C_clearInterruptFlag(EUSCI_B1_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    return;
}
