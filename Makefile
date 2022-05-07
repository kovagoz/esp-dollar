PORT ?= /dev/cu.usbserial-0001

idf_bin := idf.py

.PHONY: build
build:
	$(idf_bin) build

.PHONY: menuconfig
menuconfig:
	$(idf_bin) menuconfig

.PHONY: clean
clean:
	$(idf_bin) clean

.PHONY: install
install:
	$(idf_bin) flash -p $(PORT)

.PHONY: uninstall
uninstall:
	$(idf_bin) -p $(PORT) erase-flash

.PHONY: info
info:
	$(idf_bin) size

.PHONY: erase
erase:
	$(idf_bin) erase-flash

.PHONY: monitor
monitor:
	$(idf_bin) monitor -p $(PORT)
