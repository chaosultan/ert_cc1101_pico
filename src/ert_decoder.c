// src/ert_decoder.c
#include "ert_decoder.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"

// Declare the PIO program functions (auto-generated from .pio file)
#include "cc1101_rx.pio.h"

extern PIO pio;
extern uint sm;

// Buffer to hold raw bits before processing
#define RAW_BUFFER_SIZE 256
uint32_t raw_buffer[RAW_BUFFER_SIZE];
int buffer_index = 0;

bool ert_decode_packet(ert_packet_t *packet) {
    // 1. Check if the PIO state machine has data in its FIFO
    if (!pio_sm_is_rx_fifo_empty(pio, sm)) {
        // 2. Read the 32-bit chunk from the PIO FIFO
        raw_buffer[buffer_index++] = pio_sm_get(pio, sm);

        // 3. If our buffer has enough data, try to find a packet
        if (buffer_index >= RAW_BUFFER_SIZE) {
            buffer_index = 0; // Prevent overflow, better logic needed
        }

        // 4. **THIS IS THE COMPLEX PART YOU MUST WRITE**
        //    You need to scan raw_buffer for the ERT preamble (e.g., 0xFFFFFFFFFFF)
        //    Then find the sync word (0x543D from the CC1101 config).
        //    Then, assuming you found the start, extract bits in the correct order.
        //    This involves shifting and masking the 32-bit words in raw_buffer.
        //    Finally, calculate and check the CRC.

        // Placeholder: Simple check for demonstration.
        // A real implementation would run a proper state machine here.
        if (buffer_index > 10) { // Arbitrary check
            packet->meter_id = 0x12345678;
            packet->consumption = 1000;
            packet->tamper_flags = 0;
            packet->crc_valid = true;
            buffer_index = 0; // Reset buffer after "processing"
            return true;
        }
    }
    return false;
}