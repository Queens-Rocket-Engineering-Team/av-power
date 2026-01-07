# Schematics

This directory contains all schematics involved in this iteration of the Avionics Power Deliver system.

## Backplane
- Single, large PCB
- Connects all stack "modules" (PCBs)
- Allows communication between modules
- Cleaner routing for power

## Power Module
- Facilitates connections between backplane and external devices
	- Batteries
	- Screw switches

## Screw switches
- Supports manual enabling of power to avionics via screws
- Connected through Power Module
