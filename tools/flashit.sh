#!/bin/bash


openocd -f interface/stlink-v2.cfg -f target/nrf51.cfg \
    -c "tcl_port disabled" \
    -c "init; reset halt; nrf51 mass_erase; reset halt;" \
    -c "program ./.pio/build/waveshare_ble400/firmware.hex; reset; exit"

