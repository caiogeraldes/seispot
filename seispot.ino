//
// Controlador MIDI usando um Arduino, n potenciômetros e 2 switches
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
#include "./filter.h"

// Switches para as duas séries de n/2 potenciômetros
ezButton serie1Switch(2);
ezButton serie2Switch(3);

// Caso altere o valor, corrigir os pinos analógicos da variável
// potenciometro. Se utilizar um valor ímpar, checar a linha que
// divide entre as duas séries.
const int numPots = 6;
const int potenciometro[numPots] = { A0, A1, A2, A3, A4, A5 };

int valorPotenciometro[numPots];
int valorSaida[numPots];
// int valorSaidaPrevio[numPots];

LowPass<2> lp1(7, 1e2, true);
LowPass<2> lp2(7, 1e2, true);
LowPass<2> lp3(7, 1e2, true);
LowPass<2> lp4(7, 1e2, true);
LowPass<2> lp5(7, 1e2, true);
LowPass<2> lp6(7, 1e2, true);
LowPass<2> lp[numPots] = { lp1, lp2, lp3, lp4, lp5, lp6 };

void setup() {
  Serial.begin(115200);
  for (byte i = 0; i < numPots; ++i) {
    valorPotenciometro[i] = 0;
    valorSaida[i] = 0;
    // valorSaidaPrevio[i] = 0;
  }
}

void mudancaControle(byte canal, byte controle, byte valor) {
  midiEventPacket_t evento = { 0x0B, 0xB0 | canal, controle, valor };
  MidiUSB.sendMIDI(evento);
}

void loop() {
  // Inicialização dos switches
  serie1Switch.loop();
  serie2Switch.loop();
  // Valores mínimos
  // Série 1
  int state1 = serie1Switch.getState();
  long min1 = -127;
  if (state1 == HIGH)
    min1 = 0;
  else
    min1 = -127;
  // Série 2
  int state2 = serie2Switch.getState();
  long min2 = -127;
  if (state2 == HIGH)
    min2 = 0;
  else
    min2 = -127;

  for (byte i = 0; i < numPots; ++i) {
    // Guarda os valores de saida prévios para evitar substituições desnecessárias
    // valorSaidaPrevio[i] = valorSaida[i];
    int valorNaoFiltrado = analogRead(potenciometro[i]);
    valorPotenciometro[i] = lp[i].filt(valorNaoFiltrado);
    // Mapeia as séries de acordo com o valor do switch
    if (i < (numPots / 2)) {
      valorSaida[i] = map(valorPotenciometro[i], 0, 1023, min1, 127);
    } else {
      valorSaida[i] = map(valorPotenciometro[i], 0, 1023, min2, 127);
    }
    // Apenas atualiza o valor se houver uma mudança
    // if (valorSaida[i] != valorSaidaPrevio[i]) {
    mudancaControle(0, i + 1, valorSaida[i]);
    MidiUSB.flush();
    delay(1);
    // }
  }

  for (byte i = 0; i < numPots; ++i) {
    Serial.print(valorSaida[i]);
    Serial.print(",");
    if (i == 5) {
      Serial.println();
    }
  }
}
