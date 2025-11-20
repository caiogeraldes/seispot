# Controlador MIDI usando um Arduino, 6 potenciômetros e 2 switches

**IMPORTANTE**: É necessário uma placa com suporte a usb nativo, como o Leonardo, Due, Micro, Zero. Ver:
[Create a MIDI Device](https://docs.arduino.cc/tutorials/generic/midi-device/)

A maior parte do trabalho é feita pela biblioteca [MIDIUSB](https://docs.arduino.cc/libraries/midiusb/).

Por conveniência, utilizo a biblioteca [ezButton](https://github.com/IPdotSetAF/EZButton).

## Componentes
 - Arduino Leonardo ou similar
 - 6x Potenciômetros 10k Ohm
 - 2 switches
 - (opcional) 2 resistores 10k Ohm[^1]

## Esquema

![Esquemática](./schema.png) 

 [^1] A princípio a biblioteca ezButton dá conta.

