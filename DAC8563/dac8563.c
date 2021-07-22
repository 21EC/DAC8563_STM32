/*
 * dac8563.c
 *
 *  Created on: Jul 19, 2021
 *      Author: headcrab
 */

// SPI 8-bit mode, SW nSS, CPOL high, CPHA 1 edge
// Actually CLR pin is not used
// It seems that maximum spi speed is just 30Mbit/s when using sync mode, higher speed would result in no output

#include "dac8563.h"


// Note that when all dac channels are disabled, internal reference would turn off and won't automatically turn on.
// Besides, changing reference source would also change gain mode

void DAC8563_Write(DAC8563_Handler *device, DAC8563_Command command, DAC8563_Address address, uint16_t data)
{
  uint8_t txData[3];
  HAL_GPIO_WritePin(device->sync_port, device->sync_pin, GPIO_PIN_RESET);
  txData[0] = command | address;
  txData[1] = data >> 8;
  txData[2] = data & 0xFFU;
  HAL_SPI_Transmit(device->spi_handler, txData, 3, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(device->sync_port, device->sync_pin, GPIO_PIN_SET);
  return;
}

void DAC8563_Init(DAC8563_Handler *device, SPI_HandleTypeDef *device_spi, GPIO_TypeDef *sync_gpio_port, uint16_t sync_gpio_pin, GPIO_TypeDef *clr_gpio_port,
    uint16_t clr_gpio_pin, GPIO_TypeDef *ldac_gpio_port, uint16_t ldac_gpio_pin, DAC8563_VRefMode vmode)
{
  device->spi_handler = device_spi;
  device->sync_port = sync_gpio_port;
  device->sync_pin = sync_gpio_pin;
  device->clr_port = clr_gpio_port;
  device->clr_pin = clr_gpio_pin;
  device->ldac_port = ldac_gpio_port;
  device->ldac_pin = ldac_gpio_pin;
  device->vref_mode = vmode;
  device->daca.power_mode = Power_On;
  device->dacb.power_mode = Power_On;
  device->daca.value = 0;
  device->dacb.value = 0;
  DAC8563_Reset(device, Reset_All);
  DAC8563_VRefSwitch(device, vmode);
  return;
}

void DAC8563_Delay(uint32_t delay)
{
  uint32_t i;
  for (i = 0; i < delay; ++i)
  {
    __NOP();
  }
  return;
}

// If using internal reference source, remember to re-enable it after reset
void DAC8563_Reset(DAC8563_Handler *device, DAC8563_ResetMode mode)
{
  HAL_GPIO_WritePin(device->clr_port, device->clr_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(device->sync_port, device->sync_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(device->ldac_port, device->ldac_pin, GPIO_PIN_SET);
  DAC8563_Write(device, Reset, DAC_All_Addr, mode);
  device->daca.value = 0;
  device->dacb.value = 0;
  if (mode == Reset_All)
  {
    device->vref_mode = VRef_External;
    device->daca.gain = Gain_A_1;
    device->dacb.gain = Gain_B_1;
    device->daca.ldac = LDAC_A_Async;
    device->dacb.ldac = LDAC_B_Async;
  }

  return;
}

// Changing VRef source would affect gain settings
void DAC8563_VRefSwitch(DAC8563_Handler *device, DAC8563_VRefMode vmode)
{
  device->vref_mode = vmode;
  if(vmode == VRef_Internal)
  {
    device->daca.gain = Gain_A_2;
    device->dacb.gain = Gain_B_2;
  }
  else if (vmode == VRef_External)
  {
    device->daca.gain = Gain_A_1;
    device->dacb.gain = Gain_B_1;
  }
  DAC8563_Write(device, VRef_Control, DAC_All_Addr, vmode);
  return;
}

void DAC8563_PowerSwitch(DAC8563_Handler *device, DAC8563_PowerTarget target, DAC8563_PowerMode mode)
{
  uint16_t data;
  data = target | mode;
  DAC8563_Write(device, Power, DAC_All_Addr, data);

  // Internal reference would be turn off when all channels are disabled
  // It is unknown that what would happen to gain settings when switching from all off to other power settings
  if (device->vref_mode == VRef_Internal && device->daca.power_mode != Power_On && device->dacb.power_mode != Power_On)
  {
    DAC8563_SetVRefSource(device, VRef_Internal);
  }

  switch (target)
  {
  case DAC_A_PT:
    device->daca.power_mode = mode;
    break;
  case DAC_B_PT:
    device->dacb.power_mode = mode;
    break;
  case DAC_All_PT:
    device->daca.power_mode = mode;
    device->dacb.power_mode = mode;
    break;
  }
  return;
}

void DAC8563_GainSwitch(DAC8563_Handler *device, DAC8563_Gain gain_a_mode, DAC8563_Gain gain_b_mode)
{
  device->daca.gain = gain_a_mode;
  device->dacb.gain = gain_b_mode;
  DAC8563_Write(device, Write, Gain_Addr, gain_a_mode | gain_b_mode);
  return;
}

void DAC8563_LDACSwitch(DAC8563_Handler *device, DAC8563_LDAC ldac_a_mode, DAC8563_LDAC ldac_b_mode)
{
  device->daca.ldac = ldac_a_mode;
  device->dacb.ldac = ldac_b_mode;
  DAC8563_Write(device, LDAC, DAC_All_Addr, ldac_a_mode | ldac_b_mode);
  return;
}

void DAC8563_PulseLDAC(DAC8563_Handler *device)
{
  HAL_GPIO_WritePin(device->ldac_port, device->ldac_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(device->ldac_port, device->ldac_pin, GPIO_PIN_SET);
  return;
}
