#include <Button.h>
#include <Timer.h>
#include <Event.h>

// Timer
Timer t;

// Botones
Button btnReset(4);
Button btnMedir(5);

// defines pins numbers
const int trigPin = 8;
const int echoPin = 7;
const int OKLed = 2;
const int SirviendoLed = 3;
const int RelayPin = 10;
const int iluminaLed = 12;
const int delayCebar = 1500;
const int maximumRange = 5;
const int minimumRange = 3;

// defines variables
boolean abierto = false;
boolean tiempo = false;
boolean btnPressed = false;
boolean systemUp = false;
boolean midiendo = false;
boolean servirHabilitado = false;
int val = 0;
int distance;
long duration;
long interval = 1000; // interval at which to do something (milliseconds)
int pararServirEvent;
int habilitarServirEvent;

unsigned long previousMillis = 0; // last time update
unsigned long currentMillis = 0;
unsigned long inicioMedicionMillis = 0;
unsigned long finMedicionMillis = 0;
unsigned long mateMillis = 0;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  pinMode(OKLed, OUTPUT);
  pinMode(SirviendoLed, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);
  pinMode(iluminaLed, OUTPUT);

  // Los Metodos estan invertidos en la Lib,
  btnReset.on_press(setBtnPressedFalse); // es en realidad cuando se suelta el boton Reset
  btnReset.on_release(resetSistema);     // evento cuando se apreta el boton Reset

  btnMedir.on_press(btnMedirLgnRelease);   // Cuando se suelta el boton Medir
  btnMedir.on_release(btnMedirLgnPress);   // Cuando se apreta el boton Medir
}

void loop() {
  currentMillis = millis();

  distance = getSensorDistance();

  
  if (!btnPressed && systemUp) {
    if (isInRange() && !abierto && servirHabilitado) {
      if (mateMillis == 0) {
        mateMillis = millis();
      }
      if (millis() - mateMillis > delayCebar) {
        servirAgua();
        pararServirEvent = t.after(interval, pararServir);
        previousMillis = currentMillis;
        tiempo = true;
        abierto = true;
      }
    } else if (!isInRange()) {
      if (abierto) {
        pararServir();
        t.stop(pararServirEvent);
        abierto = false;
      }
      mateMillis = 0;
    }
  }

  // Prende o apaga las luces que iluminan el mate
  if (isInRange()) {
    digitalWrite(iluminaLed, HIGH);
  } else {
    digitalWrite(iluminaLed, LOW);
  }

  delay(100);
  t.update();

  btnReset.init();
  btnMedir.init();
}

/**
   Obtiene la distancia medida por el sensor
*/
int getSensorDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration_1 = pulseIn(echoPin, HIGH);

  // Calculating the distance
  int distance_1 = duration_1 * 0.034 / 2;
  return distance_1;
}

/**
   Checkea si el objeto esta dentro del rango
*/
boolean isInRange() {
  if (distance <= maximumRange && distance >= minimumRange) {
    return true;
  } else {
    return false;
  }
}

/**
   Evento del boton cuando se efectua un Long_Press al btnMedir
*/
void btnMedirLgnPress() {
  if (!systemUp && isInRange()) {
    servirAgua();
    midiendo = true;
  }
}

/**
   Evento del boton cuando se suelta el Long_Press del btnMedir
*/
void btnMedirLgnRelease() {
  if (!systemUp && isInRange() && midiendo) {
    pararServir();
    abierto = true;
    midiendo = false;
    interval = btnMedir.gap();
    Serial.print(interval);
    systemUp = true;
    digitalWrite(OKLed, HIGH);
    digitalWrite(SirviendoLed, LOW);
  }
}

/**
   Resetea el sistema
   No importa el estado en el que este, la idea es si sucede algo, apretando este boton se para el agua y se resetea todo
*/
void resetSistema() {
    pararServir();
    btnPressed = true;
    systemUp = false;
    midiendo = false;
    interval = 1000;
    digitalWrite(OKLed, LOW);
}

/**
   Setea la variable btnPressed en FALSE
*/
void setBtnPressedFalse() {
  btnPressed = false;
}

/**
   Metodo para Servir agua
    Activa el relay de la bomba de agua
    Activa el LED indicador de sirviendo agua
    Deshabilita servir
*/
void servirAgua() {
  digitalWrite(SirviendoLed, HIGH);
  digitalWrite(RelayPin, LOW);
  servirHabilitado = false;
}

/**
   Metodo para parar de Servir
    Desactiva el relay de la bomba de agua
    Desactiva el LED indicador de sirviendo agua
    Habilita servir despues de 2s
*/
void pararServir() {
  digitalWrite(SirviendoLed, LOW);
  digitalWrite(RelayPin, HIGH);
  habilitarServirEvent = t.after(2000, habilitarServir);
}

/**
   Setea la variable 'servirHabilitado' en true
*/
void habilitarServir() {
  servirHabilitado = true;
}
