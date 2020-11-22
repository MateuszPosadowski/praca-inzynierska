/*
 * display.h
 *
 *  Created on: Nov 12, 2020
 *      Author: mateu
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stm32f1xx_hal.h"
#include "main.h"

enum display_direction {horizontal, vertical};

struct display_config
{
    SPI_HandleTypeDef * spi;
    GPIO_TypeDef * reset_port;  uint16_t reset_pin;
    GPIO_TypeDef * bl_port;     uint16_t bl_pin;
    GPIO_TypeDef * dc_port;     uint16_t ce_pin;
    GPIO_TypeDef * ce_port;     uint16_t dc_pin;
};

void display_setup(struct display_config * cfg);
void display_set_dxy(struct display_config * cfg,
                     enum display_direction direction,
                     uint8_t column, uint8_t row);
void display_write_cmd(struct display_config * cfg, uint8_t cmd);
void display_write_data(struct display_config * cfg, uint8_t data);

#endif /* INC_DISPLAY_H_ */
