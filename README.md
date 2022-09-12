# Nanotec-Motor-Control-Firmware
 Firmware for Nanotec Stepper Motor





## Enable COM Port on motor controller

Plug motor in via USB. It will appear as removable storage drive. Modify the PD4FG.TXT file on this drive to change start up conditions.

`USB has to be disconnected and PD4C has to be power cycled after this file has been modified!`



Add these lines to the end of the file, save and reboot the motor.

```
;Set bit 20 of object 2102h:00 enabling virtual com port
2102:00=0x190001
;Deactivate the DIP switches dd4c = 1
dd4c=1

```


## Use Nanotex Plug & Drive software

Software must be opened in administration mode.

If Run and Build controls are greyed out, "Stop" Program on Controller.

When running NanoJ program from Plug & Drive, GCC will compile software and save onto removeable storage drive as:
`VMMCODE.USR`

## MOTOR FIRMWARE folder

Contains a copy of the last WORKING built file for Motor Controller. Revert to this if problems occur.

## NanoJ Programs folder

Contains uncompiled programs and libraries for NanoJ Motor Controllers


# Interfacing with the Motor 

Options for getting values out of the object dictionary during runtime

fieldbus?

Options needed


| Object 			|Use					| Index | Sub-Index | Range_min | Range_max | Value |
|---				|---					|---	|---		|---		|---		|---	|
|Torque Actual Value|						|6077	| 00		| 0			| 1000		|		|
|NanoJ Output #2	|Torque Percent (User)	|2500	| 02		| 0			| 100		|		|
|NanoJ Output #1	|Analog Input (Speed)	|2500	| 02		| 0			| 100		|		|
