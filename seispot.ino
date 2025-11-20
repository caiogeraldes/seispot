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

// Switches para as duas séries de n/2 potenciômetros
ezButton serie1Switch(2);
ezButton serie2Switch(3);

// Caso altere o valor, corrigir os pinos analógicos da variável
// potenciometro. Se utilizar um valor ímpar, checar a linha que
// divide entre as duas séries.
const int numPots = 6;
const int potenciometro[numPots] = { A0, A1, A2, A3, A4, A5 };

// Usado para diminuir saltos de valores, quanto maior mais lerdo, mas mais
// suave a transição.
const int numLeituras = 10;

int valorPotenciometro[numPots];
int valorSaida[numPots];
int valorSaidaPrevio[numPots];
int total = 0;

void setup() {
  Serial.begin(115200);
  for (byte i = 0; i < numPots; ++i) {
    valorPotenciometro[i] = 0;
    valorSaida[i] = 0;
    valorSaidaPrevio[i] = 0;
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
    valorSaidaPrevio[i] = valorSaida[i];
    // Gera uma média de 10 leituras separadas por 1ms para diminuir os saltos
    // de valores lidos por cada potenciômetro
    total = 0;
    for (byte j = 0; j < numLeituras; ++j) {
      total += analogRead(potenciometro[i]);
      delay(1);
    }
    valorPotenciometro[i] = total / numLeituras;
    // Mapeia as séries de acordo com o valor do switch
    if (i < (numPots / 2)) {
      valorSaida[i] = map(valorPotenciometro[i], 0, 1023, min1, 127);
    } else {
      valorSaida[i] = map(valorPotenciometro[i], 0, 1023, min2, 127);
    }
    // Apenas atualiza o valor se houver uma mudança
    if (valorSaida[i] != valorSaidaPrevio[i]) {
      Serial.print("potenciometro: ");
      Serial.print(i);
      Serial.print("\t valor: ");
      Serial.println(valorSaida[i]);
      mudancaControle(0, i + 1, valorSaida[i]);
      MidiUSB.flush();
      delay(1);
    }
  }
}
