#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// UART MICRO
#define TXD 17
#define RXD 16

// HX711
#define DT 32
#define SCK 33
HX711 scale;
#define escala 64000.0f
#define BOTCAL 2
#define pesoMin 0.010
#define pesoMax 10000.0

// LCD 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2); // endereço comum é 0x27, mas pode ser 0x3F

HardwareSerial SerialUart(1);

float umidade, temperatura, peso, CO2;
String temp, umid, pe, gas;

void setup() {
  // UART
  Serial.begin(115200);
  SerialUart.begin(115200, SERIAL_8N1, RXD, TXD);
  Serial.println("ESP32 A iniciada");

  // HX711
  scale.begin(DT, SCK);
  scale.set_scale(escala);
  delay(2000);
  scale.tare();
  Serial.println("Tara Realizada!");

  // LCD
  lcd.init();        // Inicializa LCD
  lcd.backlight();   // Liga a luz de fundo
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  delay(1000);
  lcd.clear();

  //BOTAO
  pinMode(BOTCAL, INPUT_PULLUP);
}

void loop() {
  static unsigned long tempoUltimaAtualizacao = 0;
  static unsigned long tempoUltimaLeituraPeso = 0;
  static unsigned long tempoUltimaImpressaoPeso = 0;

  const unsigned long intervaloPeso = 500;   // leitura rápida do peso
  const unsigned long intervaloImpressao = 300; // atualização no LCD mais rápida
  const unsigned long intervaloGeral = 5000; // temperatura, umidade, CO2

  // --- LEITURA DO PESO ---
  if (millis() - tempoUltimaLeituraPeso >= intervaloPeso) {
    if (scale.is_ready()) {
      peso = scale.get_units(5) * 100.0;
      peso = (peso / 3) * 10.0;
      if (peso <= pesoMin) {
        scale.tare();
        peso = 0;
      } else if (peso >= pesoMax) {
        scale.tare();
        peso = 0;
      }
    }
    tempoUltimaLeituraPeso = millis();
  }

  // --- IMPRESSÃO DO PESO NO LCD (separada) ---
  if (millis() - tempoUltimaImpressaoPeso >= intervaloImpressao) {
    lcd.setCursor(11, 1);
    lcd.print("     ");    // limpa espaço
    lcd.setCursor(11, 1);
    lcd.print(peso, 1);    // imprime peso atualizado
    tempoUltimaImpressaoPeso = millis();
  }

  // --- BLOCO GERAL A CADA 5s ---
  if (millis() - tempoUltimaAtualizacao >= intervaloGeral) {
    // Fórmula aplicada novamente antes do envio

    temperatura = random(150, 350) / 10.0;
    umidade = random(400, 900) / 10.0;
    CO2 = random(800, 1500);

    temp = String(temperatura, 1);
    umid = String(umidade, 1);
    pe = String(peso, 1);
    gas = String(CO2, 1);

    Serial.println("Enviado Temperatura: " + temp);
    Serial.println("Enviado Umidade: " + umid);
    Serial.println("Enviado Peso: " + pe );
    Serial.println("Enviado CO2: " + gas);
    Serial.println("                  ");

    SerialUart.println(String(temperatura, 1) + "," + String(umidade, 1) + "," + String(peso, 1) + "," + String(CO2, 0) + "\n");

    // Atualiza LCD com dados gerais
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperatura, 1);
    lcd.print("C U:");
    lcd.print(umidade, 1);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("CO2:");
    lcd.print(CO2, 0);
    lcd.print(" P:");

    tempoUltimaAtualizacao = millis();
  }

  // --- BOTÃO DE TARA ---
  if (digitalRead(BOTCAL) == HIGH) {
    scale.tare();
    peso = 0;
    Serial.println("Balança zerada pelo botão!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibrada!");
    delay(1000);
    lcd.clear();
  }
}
