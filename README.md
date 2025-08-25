# ert_cc1101_pico

Yes, it is absolutely possible to receive ERT data with a Raspberry Pi Pico and a CC1101 module, but it is a challenging intermediate-to-advanced project. You will not be simply reading a value from a library; you will be building a significant part of the signal processing chain.

Let's break down why it's possible, the challenges involved, and what you'll need to do.

Why the CC1101 and Pico are a Good Match
CC1101 Capabilities: The CC1101 is a superb, low-cost RF IC that fully supports the necessary physical layer:

It can transmit and receive in the 915 MHz band (and can be easily tuned to the exact center frequency you need).

It natively supports FSK modulation and demodulation.

You can configure its data rate, frequency deviation, receiver bandwidth, and other parameters to match the ERT standard (100 kbps, ±50 kHz deviation).

Pico Capabilities: The Raspberry Pi Pico (especially the Pico W) is more than powerful enough for this task.

Its RP2040 microcontroller has plenty of processing power to handle the data stream from the CC1101.

It has hardware PIO (Programmable I/O) blocks, which are critical for reliably reading the high-speed, timing-sensitive data output from the CC1101.

The Significant Challenges
This is not a simple "wire it up and run a library" project. The challenges are in the software and signal processing:

Lack of Complete Libraries: While there are Arduino libraries for the CC1101 and even for ERT decoding, a mature, plug-and-play solution for the Pico+CC1101+ERT combination is not readily available. You will likely need to adapt existing code or write a significant portion yourself.

Configuring the CC1101: The magic is in the configuration register settings. You must precisely configure the CC1101 to match the ERT signal:

Frequency: e.g., 915.000.000 Hz

Data Rate: 100.000 kBaud

Deviation: 50.000 kHz

RX Bandwidth: ~200-250 kHz (must be wide enough to capture the signal without excessive noise).

Sync Mode: You'll need to configure it to detect the specific preamble and sync word of ERT packets.

Bit Timing and PIO: The CC1101 will output a demodulated digital data stream. At 100 kbps, the bits are only 10 microseconds wide. Reading this reliably with a microcontroller using digitalRead() in a loop is nearly impossible due to timing jitter and other interrupts. This is where the Pico's PIO shines. You must write a PIO program to act as a precise bit sampler on the CC1101's data output pin, clocked by the CC1101's data clock output pin.

Packet Decoding (The Hardest Part): Once you have a stream of raw bits, you need to:

Find the Preamble: Look for the specific pattern of alternating 1s and 0s that marks the start of a packet.

Find the Sync Word: Confirm the unique sync word that follows the preamble.

Parse the Packet: Decode the packet structure which contains the meter ID, consumption data, tamper flags, etc., according to the ANSI C12.19 standard.

Error Checking: Perform a CRC (Cyclic Redundancy Check) to verify the data is correct.

What You'll Need
Hardware:

Raspberry Pi Pico or Pico W.

CC1101 Transceiver Module (readily available from AliExpress, eBay, etc.).

A 915 MHz antenna (a simple wire whip antenna of the correct length will work for testing).

Wiring: You'll need to connect at least VCC, GND, SI (MOSI), SO (MISO), SCK, and CSn for SPI. For reliable data reception, you must also connect the GDO0 or GDO2 pin from the CC1101 to a Pico GPIO pin. This pin is configured to output the recovered data clock and data stream.

Software/Firmware:

A CC1101 driver for the Pico to handle SPI register configuration. You can adapt an existing Arduino library.

A PIO program to read the data stream from the GDOx pin.

C/C++ code to parse the bitstream into packets and decode the ERT data.

Step-by-Step Approach
Get Basic CC1101 Communication Working: Start by writing code for the Pico to talk to the CC1101 over SPI. Verify you can read and write its configuration registers (e.g., read the PARTNUM or VERSION register to confirm communication).

Configure for ERT: Set the CC1101's registers for the correct frequency, data rate, and deviation. This will require careful study of the CC1101 datasheet.

Test Reception (LED): A good first test is to configure the CC1101 to blink a GDOx pin when it detects the preamble of any signal. If you can see an LED blink when your meter transmits, you know you're on the right track.

Implement the PIO Bit Sampler: This is the crucial step for reliable data capture. Write a PIO program that waits for the CC1101's clock pin to change and then samples the state of the data pin on each clock edge, storing the bits in a buffer.

Decode the Bits: Write the logic to take the buffer of raw bits from the PIO, search for the ERT preamble 0xFFFFFFFFFFF (or similar), find the sync word, and then chunk the remaining bits into fields.

Parse and Display: Extract the meter ID (a unique 32-bit number often printed on the meter itself) and the consumption data (in kWh).

Conclusion
Yes, it's a feasible and very rewarding project, but be prepared for a deep dive into RF configuration, precise timing, and data protocol decoding. It's an fantastic way to learn about digital signal processing on microcontrollers.

Your best resources will be:

The CC1101 Datasheet from TI.

The Pico C/C++ SDK documentation, specifically the PIO section.

Open-source projects that have done similar things (search for "Arduino CC1101 ERT" or "rtlamr" to understand the protocol structure, even if they are for other platforms).
