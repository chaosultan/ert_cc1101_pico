// src/cc1101.h
#pragma once
#include "hardware/spi.h"

void cc1101_init(spi_inst_t *spi, uint csn_pin);
void cc1101_cmd_strobe(uint8_t cmd);
void cc1101_write_reg(uint8_t reg, uint8_t data);
uint8_t cc1101_read_reg(uint8_t reg);
void cc1101_configure_ert();