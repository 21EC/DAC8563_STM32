/*
 * dac8563.h
 *
 *  Created on: Jul 19, 2021
 *      Author: headcrab
 */

#ifndef DAC8563_DAC8563_H_
#define DAC8563_DAC8563_H_

#include "main.h"

typedef enum
{
  Write = 0x0U,
  SW_Update = 0x8U,
  Write_Update_All = 0x10U,
  Write_Update_Target = 0x18U,
  Power = 0x20U,
  Reset = 0x28U,
  LDAC = 0x30U,
  VRef_Control = 0x38U
} DAC8563_Command;

typedef enum
{
  DAC_A_Addr = 0x0U,
  DAC_B_Addr = 0x1U,
  Gain_Addr = 0x2U,
  DAC_All_Addr = 0x7U
} DAC8563_Address;

typedef enum
{
  VRef_External = 0U,
  VRef_Internal = 1U
} DAC8563_VRefMode;

typedef enum
{
  Power_On = 0x00U,
  Power_Off_1K = 0x10U,
  Power_Off_100K = 0x20U,
  Power_Off_HiZ = 0x30U
} DAC8563_PowerMode;

typedef enum
{
  DAC_A_PT = 0x1U,
  DAC_B_PT = 0x2U,
  DAC_All_PT = 0x3U
} DAC8563_PowerTarget;

typedef enum
{
  Gain_A_2 = 0x0U,
  Gain_A_1 = 0x1U,
  Gain_B_2 = 0x0U,
  Gain_B_1 = 0x2U
} DAC8563_Gain;

typedef enum
{
  LDAC_A_Async = 0x0U,
  LDAC_A_Sync = 0x1U,
  LDAC_B_Async = 0x0U,
  LDAC_B_Sync = 0x2U
} DAC8563_LDAC;

typedef struct
{
    uint16_t value;
    DAC8563_PowerMode power_mode;
    DAC8563_Gain gain;
    DAC8563_LDAC ldac;
} DAC8563_DACStatus;

typedef struct
{
    SPI_HandleTypeDef *spi_handler;
    GPIO_TypeDef *sync_port;
    uint16_t sync_pin;
    GPIO_TypeDef *clr_port;
    uint16_t clr_pin;
    GPIO_TypeDef *ldac_port;
    uint16_t ldac_pin;

    DAC8563_VRefMode vref_mode;
    DAC8563_DACStatus daca;
    DAC8563_DACStatus dacb;
} DAC8563_Handler;

typedef enum
{
  Reset_DAC_Only = 0U,
  Reset_All = 1U
} DAC8563_ResetMode;

void DAC8563_Write(DAC8563_Handler *device, DAC8563_Command command, DAC8563_Address address, uint16_t data);
void DAC8563_Init(DAC8563_Handler *device, SPI_HandleTypeDef *device_spi, GPIO_TypeDef *sync_gpio_port, uint16_t sync_gpio_pin, GPIO_TypeDef *clr_gpio_port,
    uint16_t clr_gpio_pin, GPIO_TypeDef *ldac_gpio_port, uint16_t ldac_gpio_pin, DAC8563_VRefMode vmode);
void DAC8563_Delay(uint32_t delay);
void DAC8563_Reset(DAC8563_Handler *device, DAC8563_ResetMode mode);
void DAC8563_VRefSwitch(DAC8563_Handler *device, DAC8563_VRefMode vmode);
void DAC8563_PowerSwitch(DAC8563_Handler *device, DAC8563_PowerTarget target, DAC8563_PowerMode mode);
void DAC8563_GainSwitch(DAC8563_Handler *device, DAC8563_Gain gain_a_mode, DAC8563_Gain gain_b_mode);
void DAC8563_LDACSwitch(DAC8563_Handler *device, DAC8563_LDAC ldac_a_mode, DAC8563_LDAC ldac_b_mode);
void DAC8563_PulseLDAC(DAC8563_Handler *device);

#endif /* DAC8563_DAC8563_H_ */
