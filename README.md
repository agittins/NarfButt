# NarfButt! An nRF51822 BTHome Button

This is (currently a proof-of-concept) implementation of the
[BTHome](https://bthome.io/) protocol for nRF51xx chips (in particular
the nRF51822).

The idea is to create a BLE (Bluetooth Low Energy) remote
control with buttons (and perhaps other sensors) that sends data to
HomeAssistant via ESPHome Bluetooth Proxies.

Using the BTHome protocol means that no integrations are required, and
the device will show up automatically in Home Asssistant ready for
creating automations.


```
  +----------+          +-----------+           +-----------+
  | NarfButt |          |  ESPHome  |           |   Home    |    **~*~*~**
  |  Remote  | -(ble)-> | ble_proxy | -(wifi)-> | Assistant | -> * MaGiC *
  +----------+          +-----------+           +-----------+    **~*~*~**
```

My personal goal is to create super-affordable remote control buttons
for placing over the top of existing light switches etc for
home-automation projects.

## uFAQ

- Why not zigbee, zwave or wifi remotes?
  - Cost. I want 6 or so buttons for under AU$10. Tradfi buttons are
    sexy, but too expensive to replace lots of wall switches (for me,
    anyway).
  - Redundancy - zigbee networks have only one controller node, despite
    having redundant paths to that node. Using BLE Proxies means I have
    many (currently half a dozen or so) BLE receivers that forward their
    data via one of multiple wifi APs, at which point my ethernet
    network is pretty reliable. I don't know how zwave works, but see
    point 1 above.
  - Wifi's no good because of power consumption, as I want these remotes
    to be easily thrown anywhere around the house. A CR2032 in this device
    should last a year or so if I haven't left too many bugs in the
    code.
- What modules should I buy?
  - The cheapest I've found are the HOLYIOT-17051-NRF51822 which are a
    little breakout board with annoyingly small 2.0mm-pitched pins. They
    seem to work just fine. There are also some smaller modules with
    castellated edges which might be easier to mount - I plan to make a
    board layout for those too - oddly the smaller modules are a bit
    more expensive, but still quite cheap.

## Building the Firmware image

This is a platformIO project with VSCode plugins, so opening with with
VSCode is probaby going to give the smoothest experience to try it out.

Please forgive my atrocious C++ code, I'm a greybeard SysAdmin not a
programmer :-) PR's *VERY* welcome!

## Flashing

I use a clone STLink V2 flasher and OpenOCD.

This connects with just the `GND`, `3.3v`, `SWDIO` and `SWCLK` lines.

The modules need to be
mass-erased before flashing. Sometimes this part is hard to get working,
but ultimately what I've found works is:

```shell
openocd -f interface/stlink-v2.cfg -f target/nrf51.cfg \
    -c "tcl_port disabled" \
    -c "init; reset halt; nrf51 mass_erase; reset halt;" \
    -c "program ./.pio/build/waveshare_ble400/firmware.hex; reset; exit"
```

(You can just run `./tools/flashit.sh` to run exactly the above, or some
later variant that I've forgotten to update here).

Currently I'm having issues running openocd interactively, it seems to
disconnect after a few seconds, but using the command above so it all
happens immediately is working fine for me.

## TODO's (please!)

- [ ] learn c.
- [ ] learn cpp.
- [ ] add buttons, sleep mode etc.
- [ ] turn this into a usable basis for my own and others' projects.
- [x] document how to build and flash a device (I think I used openOCD
  but frankly it was months ago and I am just whacking this up on GH
  now).
- [ ] Implement BTHomeV2 Encryption
- [ ] Fix module width in PCB design
- [ ] Try to take over the world.
  [Narf!](https://pinkyandthebrain.fandom.com/wiki/Narf)
- [ ] Choose a license. I haven't chosen a license yet. I need to check the sources I pilfered
  from (there are comments in the code) and choose a permissive license
  that fits the requirements of the ~~stolen code~~ prior art.

## Credits

- Huge thanks to [Jean-Matthiue
  DECHRISTE](https://www.iot-experiments.com/nrf51822-and-ble400/) who
  built something pretty similar hardware-wise using the Arduino stack
  and a custom phone app back in 2019.
- Nordic's [nRF51 Series current consumption
  guide](https://devzone.nordicsemi.com/guides/hardware-design-test-and-measuring/b/nrf5x/posts/nrf51-current-consumption-guide)
- Everyone around the amazing HomeAssistant project, including ESPHome.
