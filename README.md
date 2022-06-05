Dollar Exchange
===============

[![stability-beta](https://img.shields.io/badge/stability-beta-33bbff.svg)](https://github.com/mkenney/software-guides/blob/master/STABILITY-BADGES.md#beta)

This is the soul of the gadget I make for my son to help him to check the worth
of his dollars. It connects to our wifi network, fetches the current exchange
rate, multiply that with 50 (the amount of dollars he owns), and shows the
result on a 8 digit 7-segment LED display.

Requirements
------------

### Hardware

- Some kind of ESP32 board
- MAX7219 controlled 8 digit, 7-segment LED display

### Software

- [Espressif IoT Development Framework](https://www.espressif.com/en/products/sdks/esp-idf)
- make

### Other

- API key to [APILayer](https://apilayer.com)

Circuit
-------

The program sends commands to the LED display through the SPI3 (also called VSPI)
peripheral of ESP32. The wiring of the LED display in this case looks like this:

```
+-------------+
|             |                +--------------------------+
|           23|----------------|DIN                       |
|    ESP32  18|----------------|CLK       MAX7219         |
|            5|----------------|CS                        |
|             |                +--------------------------+
+-------------+
```

Installation
------------

Get the code:

```sh
git clone git@github.com:kovagoz/esp32-dollar-exchange.git
cd esp32-dollar-exchange
git submodule init
git submodule update
```

Set up the wifi SSID / password and the API key:

```sh
make menuconfig
```

Then compile the project:

```sh
make
```

And finally upload the binaries to the esp board:

```sh
make install
```
