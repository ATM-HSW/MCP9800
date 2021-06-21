/*-----------------------------------------------------------------------------*
 * Arduino Library for Microchip MCP9800/1/2/3 2-Wire High-Accuracy            *
 * Temperature Sensors                                                         *
 *                                                                             *
 * A lightweight implementation that exposes all functionality of the MCP9800  *
 * sensors. Temperatures are dealt with in the integer domain to avoid         *
 * the code size and runtime overhead associated with floating-point. Still,   *
 * it is easy enough to perform the necessary conversions should the user      *
 * wish to work in floating-point format.                                      *
 *                                                                             *
 * Temperatures read from the device's registers are returned as °C*16.        *
 * (If the device resolution is set to less than 12 bits, the corresponding    *
 * lower order bits are simply returned as zero.) Temperatures can             *
 * alternately be read as °F*10.                                               *
 *                                                                             *
 * When writing the Hysteresis and Limit-Set registers, the value must be      *
 * given as °C*2, which corresponds to the internal representation in these    *
 * registers.                                                                  *
 *                                                                             *
 * Bit masks for the control register are provided in the MCP9800.h file.      *
 *                                                                             *
 * Jack Christensen 21Mar2014 v1.0                                             *
 *                                                                             *
 * "Arduino library for Microchip MCP9800/1/2/3" by Jack Christensen           *
 * is licensed under CC BY-SA 4.0,                                             *
 * http://creativecommons.org/licenses/by-sa/4.0/                              *
 *-----------------------------------------------------------------------------*/

#include "mbed.h"
#include <MCP9800.h>

// instantiate a temperature sensor object
MCP9800::MCP9800() {
}

// given the least three significant bits (A2:0) of its I2C address (0-7)
int MCP9800::init(I2C *i2c, uint8_t devAddr) {
  m_devAddr = devAddr;
  this->m_i2c = i2c;
  
  return 0;
}

// read one of the sensor's three temperature registers.
// returns the temperature as an integer which is °C times 16.
int32_t MCP9800::readTemp(MCP9800_REGS_t reg) {
  int ret;

  if(m_i2c==nullptr)
    return 1;

  m_cmd[0] = reg;
  ret = m_i2c->write(m_devAddr, (const char*)m_cmd, 1);

  m_cmd[0] = 0;
  m_cmd[1] = 0;
  ret = m_i2c->read(m_devAddr, (char*)m_cmd, 2);
  
  return ((m_cmd[0]&0x80)?-1:1) * (((m_cmd[0]&0x7f) * 10000) + ((m_cmd[1]&0x80)?5000:0) + ((m_cmd[1]&0x40)?2500:0) + ((m_cmd[1]&0x20)?1250:0) + ((m_cmd[1]&0x10)?625:0));
}

float MCP9800::readTempF(MCP9800_REGS_t reg) {
  return readTemp(reg)/10000.0;
}


int MCP9800::writeTempx(MCP9800_REGS_t reg, int32_t value) {

  if(m_i2c==nullptr)
    return 1;

  if (reg > AMBIENT) {    // ambient temp reg is read-only
    this->m_cmd[0] = reg;
    this->m_cmd[1] = (value/10000)&0xff;
    this->m_cmd[2] = (value-(value/10000)*10000)>=5000?0x80:0x00;
    return this->m_i2c->write(m_devAddr, (const char*)m_cmd, 3);
  }

  return 0;
}

int MCP9800::writeTempx(MCP9800_REGS_t reg, float value) {
  return writeTempx(reg, (int32_t)(value*10000));
}

// read the sensor's configuration register
uint8_t MCP9800::readConfig(MCP9800_config *cfg) {
  int ret;

  if(m_i2c==nullptr)
    return 1;

  this->m_cmd[0] = CONFIG_REG;
  ret = this->m_i2c->write(m_devAddr, (const char*)m_cmd, 1);

  m_cmd[0] = 0;
  ret = this->m_i2c->read(m_devAddr, (char*)m_cmd, 1);
  
  cfg->ONE_SHOT       = m_cmd[0]&ONE_SHOT?true:false;
  cfg->ADC_RESOLUTION = (m_cmd[0]&0x60)>>5;
  cfg->FAULT_QUEUE    = (m_cmd[0]&0x18)>>3;
  cfg->ALERT_POLARITY = m_cmd[0]&ALERT_POLARITY_HIGH?true:false;
  cfg->INT_MODE       = m_cmd[0]&INTERRUPT_MODE?true:false;
  cfg->SHUTDOWN       = m_cmd[0]&SHUTDOWN?true:false;

  return this->m_cmd[0];
}

// write the sensor's configuration register
int MCP9800::writeConfig(uint8_t value) {
  int ret;

  if(m_i2c==nullptr)
    return 1;

  this->m_cmd[0] = CONFIG_REG;
  this->m_cmd[1] = value;
  return this->m_i2c->write(m_devAddr, (const char*)m_cmd, 2);
}
