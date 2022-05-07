Dollar Exchange
===============

[![stability-wip](https://img.shields.io/badge/stability-wip-lightgrey.svg)](https://github.com/mkenney/software-guides/blob/master/STABILITY-BADGES.md#work-in-progress)

This is the soul of a gadget, made for my son, to help him to check the worth
of his dollars. It connects to our wifi network, fetches the current exchange
rate, multiply that with 50 (the amount of dollars he owns), and shows the
result on a 8 digit 7-segment LED display.

Installation
------------

Set up the wifi SSID and password:

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
