/*-----------------------------------------------------------------------------*
 * Mbed OS Library for Microchip MCP9800/1/2/3 2-Wire High-Accuracy            *
 * Temperature Sensors                                                         *
 *                                                                             *
 * A lightweight implementation that exposes all functionality of the MCP9800  *
 * sensors. Temperatures are dealt with in the integer domain to avoid         *
 * the code size and runtime overhead associated with floating-point. Still,   *
 * it is easy enough to perform the necessary conversions should the user      *
 * wish to work in floating-point format.                                      *
 *                                                                             *
 * Temperatures read from the device's registers are returned as °C*10000 to   *
 * support the full resolution.                                                *
 *                                                                             *
 * When writing the Hysteresis and Limit-Set registers, the value must be      *
 * given as °C*10000.                                                          *
 *                                                                             *
 * Bit masks for the control register are provided in the MCP9800.h file.      *
 *                                                                             *
 * Jack Christensen 21Mar2014 v1.0                                             *
 * Dr.Olaf Hagendorf 05.2021                                                   *
 *                                                                             *
 * "Arduino library for Microchip MCP9800/1/2/3" by Jack Christensen           *
 * "Mbed OS library for Microchip MCP9800/1/2/3" by Dr. Olaf Hagendorf         *
 * is licensed under CC BY-SA 4.0,                                             *
 * http://creativecommons.org/licenses/by-sa/4.0/                              *
 *-----------------------------------------------------------------------------*/

#ifndef MCP9800_H_INCLUDED
#define MCP9800_H_INCLUDED

#include "mbed.h"

const int MCP9800_ADDRESS8BIT(0x90);

// register addresses
enum MCP9800_REGS_t { AMBIENT = 0, HYSTERESIS = 2, LIMITSET = 3 };
const uint8_t CONFIG_REG(1);

// bit definitions for config register
const uint8_t ONE_SHOT(0x80);
const uint8_t ADC_RES_9BITS(0x00);
const uint8_t ADC_RES_10BITS(0x20);
const uint8_t ADC_RES_11BITS(0x40);
const uint8_t ADC_RES_12BITS(0x60);
const uint8_t FAULT_QUEUE_1(0x00);
const uint8_t FAULT_QUEUE_2(0x08);
const uint8_t FAULT_QUEUE_4(0x10);
const uint8_t FAULT_QUEUE_6(0x18);
const uint8_t ALERT_POLARITY_HIGH(0x04);
const uint8_t INTERRUPT_MODE(0x02);
const uint8_t SHUTDOWN(0x01);

struct MCP9800_config {
  bool ONE_SHOT;
  uint8_t ADC_RESOLUTION;
  uint8_t FAULT_QUEUE;
  bool ALERT_POLARITY;
  bool INT_MODE;
  bool SHUTDOWN;
};

class MCP9800 {
  public:
    MCP9800();
    int init(I2C *i2c, uint8_t devAddr);
    int32_t readTemp(MCP9800_REGS_t reg);
    int writeTempx(MCP9800_REGS_t reg, int value);
    uint8_t readConfig(MCP9800_config *cfg);
    int writeConfig(uint8_t value);

  private:
    uint8_t m_devAddr;
    I2C *i2c;
    uint8_t cmd[4];
};

#endif
