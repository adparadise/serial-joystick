#!/bin/bash

identifier=atmega16u2

sudo dfu-programmer $identifier erase
sudo dfu-programmer $identifier flash ./firmware/Arduino-keyboard-0.3.hex
sudo dfu-programmer $identifier reset
