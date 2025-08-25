// src/main.c
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "cc1101.h"
#include "ert_decoder.h"

// Define your SPI and GPIO connections
#define SPI_PORT        spi0
#define SPI_RX_PIN      4
#define SPI_TX_PIN      7
#define SPI_SCK_PIN     6
#define SPI_CSN_PIN     5

// Define CC1101 GDO pins for PIO
#define CC1101_GDO0     14  // Data Output
#define CC1101_GDO2     15  // Clock Output

// PIO and State Machine
PIO pio = pio0;
uint sm = 0;

int main() {
    stdio_init_all();
    printf("Starting Pico ERT Receiver...\n");

    // 1. Initialize the SPI interface for CC1101 configuration
    spi_init(SPI_PORT, 1000 * 1000); // 1 MHz SPI for config is fine
    gpio_set_function(SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_init(SPI_CSN_PIN);
    gpio_set_dir(SPI_CSN_PIN, GPIO_OUT);
    gpio_put(SPI_CSN_PIN, 1);

    // 2. Initialize the CC1101 with ERT settings
    cc1101_init(SPI_PORT, SPI_CSN_PIN);
    cc1101_configure_ert();

    // 3. Load the PIO program and get it ready
    uint offset = pio_add_program(pio, &cc1101_rx_program);
    sm = pio_claim_unused_sm(pio, true);
    cc1101_rx_program_init(pio, sm, offset, CC1101_GDO0, CC1101_GDO2);

    // 4. Start the CC1101 in RX mode
    cc1101_cmd_strobe(CC1101_SRX);

    printf("Entering main loop. Waiting for packets...\n");

    // 5. Main loop: Check for received data and process it
    while (true) {
        ert_packet_t packet;
        if(ert_decode_packet(&packet)) { // This function checks the PIO FIFO for data
            printf("*** ERT Packet Received! ***\n");
            printf("Meter ID: %lu\n", packet.meter_id);
            printf("Consumption: %lu kWh\n", packet.consumption);
            printf("Tamper Flags: 0x%04X\n", packet.tamper_flags);
            printf("CRC Passed: %s\n", packet.crc_valid ? "Yes" : "No");
            printf("\n");
        }
        tight_loop_contents(); // Reduce power consumption a bit
    }
    return 0;
}