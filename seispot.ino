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

template<int order>  // order is 1 or 2
class LowPass {
private:
  float a[order];
  float b[order + 1];
  float omega0;
  float dt;
  bool adapt;
  float tn1 = 0;
  float x[order + 1];  // Raw values
  float y[order + 1];  // Filtered values

public:
  LowPass(float f0, float fs, bool adaptive) {
    // f0: cutoff frequency (Hz)
    // fs: sample frequency (Hz)
    // adaptive: boolean flag, if set to 1, the code will automatically set
    // the sample frequency based on the time history.

    omega0 = 6.28318530718 * f0;
    dt = 1.0 / fs;
    adapt = adaptive;
    tn1 = -dt;
    for (int k = 0; k < order + 1; k++) {
      x[k] = 0;
      y[k] = 0;
    }
    setCoef();
  }

  void setCoef() {
    if (adapt) {
      float t = micros() / 1.0e6;
      dt = t - tn1;
      tn1 = t;
    }

    float alpha = omega0 * dt;
    if (order == 1) {
      a[0] = -(alpha - 2.0) / (alpha + 2.0);
      b[0] = alpha / (alpha + 2.0);
      b[1] = alpha / (alpha + 2.0);
    }
    if (order == 2) {
      float alphaSq = alpha * alpha;
      float beta[] = { 1, sqrt(2), 1 };
      float D = alphaSq * beta[0] + 2 * alpha * beta[1] + 4 * beta[2];
      b[0] = alphaSq / D;
      b[1] = 2 * b[0];
      b[2] = b[0];
      a[0] = -(2 * alphaSq * beta[0] - 8 * beta[2]) / D;
      a[1] = -(beta[0] * alphaSq - 2 * beta[1] * alpha + 4 * beta[2]) / D;
    }
  }

  float filt(float xn) {
    // Provide me with the current raw value: x
    // I will give you the current filtered value: y
    if (adapt) {
      setCoef();  // Update coefficients if necessary
    }
    y[0] = 0;
    x[0] = xn;
    // Compute the filtered values
    for (int k = 0; k < order; k++) {
      y[0] += a[k] * y[k + 1] + b[k] * x[k];
    }
    y[0] += b[order] * x[order];

    // Save the historical values
    for (int k = order; k > 0; k--) {
      y[k] = y[k - 1];
      x[k] = x[k - 1];
    }

    // Return the filtered value
    return y[0];
  }
};

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
