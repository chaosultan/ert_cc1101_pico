// src/cc1101.c
#include "cc1101.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

static spi_inst_t *cc1101_spi;
static uint cc1101_csn_pin;

void cc1101_init(spi_inst_t *spi, uint csn_pin) {
    cc1101_spi = spi;
    cc1101_csn_pin = csn_pin;
}

void cc1101_write_reg(uint8_t reg, uint8_t data) {
    gpio_put(cc1101_csn_pin, 0);
    spi_write_blocking(cc1101_spi, &reg, 1);
    spi_write_blocking(cc1101_spi, &data, 1);
    gpio_put(cc1101_csn_pin, 1);
}

uint8_t cc1101_read_reg(uint8_t reg) {
    uint8_t data;
    reg |= 0x80; // Set read bit
    gpio_put(cc1101_csn_pin, 0);
    spi_write_blocking(cc1101_spi, &reg, 1);
    spi_read_blocking(cc1101_spi, 0, &data, 1);
    gpio_put(cc1101_csn_pin, 1);
    return data;
}

void cc1101_cmd_strobe(uint8_t cmd) {
    gpio_put(cc1101_csn_pin, 0);
    spi_write_blocking(cc1101_spi, &cmd, 1);
    gpio_put(cc1101_csn_pin, 1);
}

void cc1101_configure_ert() {
    // Software Reset
    cc1101_cmd_strobe(CC1101_SRES);
    sleep_ms(10);

    // Configure CC1101 for ERT FSK
    // THESE VALUES ARE EXAMPLE VALUES. YOU MUST CONSULT THE DATASHEET AND MAY NEED TO ADJUST.
    cc1101_write_reg(CC1101_IOCFG2, 0x0D); // GDO2 Output: Serial Data Clock (CFG::RX)
    cc1101_write_reg(CC1101_IOCFG0, 0x0E); // GDO0 Output: Serial Data (RX)

    cc1101_write_reg(CC1101_FIFOTHR, 0x47); // Adjust FIFO thresholds
    cc1101_write_reg(CC1101_PKTCTRL0, 0x32); // Infinite packet length, CRC enabled
    cc1101_write_reg(CC1101_CHANNR, 0x00);   // Channel number 0
    cc1101_write_reg(CC1101_FSCTRL1, 0x06);  // Frequency Synthesizer Control
    cc1101_write_reg(CC1101_FREQ2, 0x23);    // Frequency: 915.000 MHz
    cc1101_write_reg(CC1101_FREQ1, 0x31);    // ... (These values are for 915MHz)
    cc1101_write_reg(CC1101_FREQ0, 0x3B);    // ...
    cc1101_write_reg(CC1101_MDMCFG4, 0xCA);  // Data Rate: ~100 kBaud
    cc1101_write_reg(CC1101_MDMCFG3, 0x83);  // ...
    cc1101_write_reg(CC1101_MDMCFG2, 0x13);  // FSK, No Manchester, 16/16 sync word
    cc1101_write_reg(CC1101_MDMCFG1, 0x22);  // FEC disabled, 4 preamble bytes
    cc1101_write_reg(CC1101_MDMCFG0, 0xF8);  // Channel spacing exponent
    cc1101_write_reg(CC1101_DEVIATN, 0x50);  // Deviation: ~50 kHz
    cc1101_write_reg(CC1101_FREND1, 0xB6);   // Receive LNA gain
    cc1101_write_reg(CC1101_MCSM0, 0x18);    // Auto calibrate when going from IDLE to RX
    cc1101_write_reg(CC1101_FOCCFG, 0x16);   // Frequency Offset Compensation
    cc1101_write_reg(CC1101_BSCFG, 0x6C);    // Bit Synchronization configuration
    cc1101_write_reg(CC1101_AGCCTRL2, 0x43); // AGC settings
    cc1101_write_reg(CC1101_AGCCTRL1, 0x40); // ...
    cc1101_write_reg(CC1101_AGCCTRL0, 0x91); // ...
    cc1101_write_reg(CC1101_WORCTRL, 0xFB);  // Wake On Radio control
    cc1101_write_reg(CC1101_FSCAL3, 0xE9);   // Frequency Synthesizer Calibration
    cc1101_write_reg(CC1101_FSCAL2, 0x2A);   // ...
    cc1101_write_reg(CC1101_FSCAL1, 0x00);   // ...
    cc1101_write_reg(CC1101_FSCAL0, 0x1F);   // ...
    cc1101_write_reg(CC1101_TEST2, 0x81);    // Various test settings
    cc1101_write_reg(CC1101_TEST1, 0x35);    // ...
    cc1101_write_reg(CC1101_TEST0, 0x09);    // ...

    // Define the sync word (this is a common ERT sync word - you must verify!)
    cc1101_write_reg(CC1101_SYNC1, 0x54); // Sync word high byte
    cc1101_write_reg(CC1101_SYNC0, 0x3D); // Sync word low byte

    sleep_ms(10);
}