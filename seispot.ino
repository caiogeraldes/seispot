//
// Controlador MIDI usando um Arduino, 6 potenciômetros e 2 switches
//
// IMPORTANTE: É necessário uma placa com suporte a usb nativo, como
// o Leonardo, Due, Micro, Zero. Ver:
// https://docs.arduino.cc/tutorials/generic/midi-device/
//
// A maior parte do trabalho é feita pela biblioteca MIDIUSB.
//
// Por conveniência, utilizo a biblioteca ezButton.
//

#include <MIDIUSB.h>
#include <ezButton.h>

// Switches para as duas séries de 3 potenciômetros
ezButton serie13Switch(2);
ezButton serie46Switch(3);

const int potenciometro[6] = { A0, A1, A2, A3, A4, A5 };

int valorPotenciometro[6] = { 0, 0, 0, 0, 0, 0 };
int valorSaida[6] = { 0, 0, 0, 0, 0, 0 };

void setup() {}

void mudancaControle(byte canal, byte controle, byte valor) {
  midiEventPacket_t evento = { 0x0B, 0xB0 | canal, controle, valor };
  MidiUSB.sendMIDI(evento);
}

void loop() {
  // Inicialização dos switches
  serie13Switch.loop();
  serie46Switch.loop();
  // Valores mínimos
  // Série 1-3
  int state1 = serie13Switch.getState();
  long min1 = -127;
  if (state1 == HIGH)
    min1 = 0;
  else
    min1 = -127;
  // Série 4-6
  int state2 = serie46Switch.getState();
  long min2 = -127;
  if (state2 == HIGH)
    min2 = 0;
  else
    min2 = -127;

  for (byte i = 0; i < 6; ++i) {
    valorPotenciometro[i] = analogRead(potenciometro[i]);
    // Mapeia as séries de acordo com o valor do switch
    if (i < 3) {
      valorSaida[i] = map(valorPotenciometro[i], 0, 1023, min1, 127);
    } else {
      valorSaida[i] = map(valorPotenciometro[i], 0, 1023, min2, 127);
    }
    mudancaControle(0, i + 1, valorSaida[i]);
    MidiUSB.flush();
    delay(2);
  }
}
