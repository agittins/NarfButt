# nRF51-BTHome

This is (currently just a proof-of-concept) implementation of the
[BTHome](https://bthome.io/) protocol for nRF51xx chips (in particular
the nRF51822).

The idea is to communicate sensor data via BLE Advertisements, which
HomeAssistant will then receive and display automagically as a sensor
device.

My personal goal is to create super-affordable remote control buttons
for placing over the top of existing light switches etc for
home-automation projects.

This is a platformIO project with VSCode plugins, so opening with with
VSCode is probaby going to give the smoothest experience to try it out.

Please forgive my atrocious C++ code, I'm a greybeard SysAdmin not a
programmer :-) PR's *VERY* welcome!

# TODO's (please!)

- [ ] learn c.
- [ ] learn cpp.
- [ ] turn this into a usable basis for my own and others' projects.
- [ ] document how to build and flash a device (I think I used openOCD
  but frankly it was months ago and I am just whacking this up on GH
  now).
- [ ] Implement BTHomeV2 Encryption

I haven't chosen a license yet. I need to check the sources I pilfered
from (there are comments in the code) and choose a permissive license
that fits the requirements of the ~~stolen code~~ prior art.

