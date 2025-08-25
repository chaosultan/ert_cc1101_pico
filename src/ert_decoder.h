// src/ert_decoder.h
#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t meter_id;
    uint32_t consumption;
    uint16_t tamper_flags;
    bool crc_valid;
} ert_packet_t;

bool ert_decode_packet(ert_packet_t *packet);