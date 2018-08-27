/*
  @author: Marcelo Schmitt

  Sketch para teste de célula de carga em conjunto com placas NodeMCU.
  Testado com uma célula de carga de 1Kg de capacidade e um amplificador hx711.
  Bem estável, sempre volta retornar a medida inicial de forca após a carga ser retirada.
  Muito sensivel (mexer nos fios que ligam a celula de carga ao amplificador pode fazer alterar a medida de forca).
  Pode ser melhor ajustado alterando fator_de_calibracao apesar de o valor inicial ja ser bem razoavel.  

  Esse sketch é adaptado de Calibrate.ino da biblioteca HX711_ADC por Olav Kallhovd (https://github.com/olkal/HX711_ADC).
*/
  
#include <SPI.h>
#include <ESP8266WiFi.h>


///////////////SETUP DA CELULA DE CARGA////////////////////
// --- Mapeamento de Hardware ---
//#define  ADDO  0    //Data Out //versao para ESP01
//#define  ADSK  2    //SCK //versao para ESP01
#define  ADDO  D0    //Data Out //versao para nodeMCU (ESP12)
#define  ADSK  D2    //SCK //versao para nodeMCU (ESP12)

//Este sktech utiliza uma versão adaptada da biblioteca HX711_ADC
#include <HX711_ADC.h>

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell(D0, D2);

//#define fator_de_calibracao 696.0
#define fator_de_calibracao 1696.0

long ultima_leitura = millis();

float forca = 0.0;

//////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);

  Serial.println("Iniciando celula de carga ...");
  LoadCell.begin();
  long stabilisingtime = 5000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  LoadCell.setCalFactor(fator_de_calibracao); // user set calibration factor (float)
  Serial.println("Startup + tare is complete");
  Serial.println();
  Serial.println();

}

void loop() {
    le_celula_carga();
    delay(500);
}

void le_celula_carga() {
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in scetch will reduce effective sample rate (be carefull with delay() in loop)
  LoadCell.update();

  //get smoothed value from data set + current calibration factor
  if (millis() > ultima_leitura + 250) {
    forca = LoadCell.getData();
    Serial.print("Forca: ");
    Serial.print(forca);
    Serial.println();
    ultima_leitura = millis();
  }

  //receive from serial terminal
    if (Serial.available() > 0) {
      float i;
      char inByte = Serial.read();
      if (inByte == 't') LoadCell.tareNoDelay();
    }

  //check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
  
}

