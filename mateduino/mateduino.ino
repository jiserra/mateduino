#include <Button.h>
#include <Timer.h>
#include <Event.h>

Timer t;

Button botonReset(4);
Button btnMedir(5);

// defines pins numbers
const int trigPin = 8;
const int echoPin = 7;
const int OKLed = 2;
const int SirviendoLed = 3;
const int RelayPin = 10;
const int boton = 4;
const int botonMedir = 5;
const int iluminaLed = 12;
const int delayCebar = 1500;

int val = 0;

const int maximumRange = 5;
const int minimumRange = 3;

boolean abierto = false;
boolean tiempo = false;
boolean btnPressed = false;
boolean systemUp = false;
boolean midiendo = false;
boolean servirHabilitado = false;

// defines variables
long duration;
int distance;

unsigned long previousMillis = 0; // last time update
long interval = 1000; // interval at which to do something (milliseconds)
unsigned long currentMillis = 0;
unsigned long inicioMedicionMillis = 0;
unsigned long finMedicionMillis = 0;

unsigned long mateMillis = 0;

int pararServirEvent;
int habilitarServirEvent;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  pinMode(OKLed, OUTPUT);
  pinMode(SirviendoLed, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);
  pinMode(boton, INPUT);
  pinMode(botonMedir, INPUT);
  pinMode(iluminaLed, OUTPUT);
  botonReset.on_press(resetSistema);
  botonReset.on_release(setBtnPressedFalse);

  btnMedir.on_long_press(btnMedirLgnPress);
  btnMedir.on_long_release(btnMedirLgnRelease);
}

void loop() {
  botonReset.init();
  btnMedir.init();
  btnMedir.set_minimum_gap(500);

  currentMillis = millis();
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;

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
    } else if (distance > maximumRange * 2) {
      if (abierto) {
        pararServir();
        t.stop(pararServirEvent);
        abierto = false;
      }
      //Serial.println("Salio");
      mateMillis = 0;
    }
  }

  if (isInRange()) {
    digitalWrite(iluminaLed, HIGH);
  } else {
    digitalWrite(iluminaLed, LOW);
  }

  delay(100);
  t.update();
}

/**
 * Checkea si el objeto esta dentro del rango
 */
boolean isInRange(){
  if (distance <= maximumRange && distance >= minimumRange) {
    return true;
  } else {
    return false;
  }
}
/**
 * Evento del boton cuando se efectua un Long_Press al btnMedir
 */
void btnMedirLgnPress() {
  if (!systemUp && isInRange()) {
    servirAgua();
  }
}

/**
 * Evento del boton cuando se suelta el Long_Press del btnMedir
 */
void btnMedirLgnRelease() {
  if (!systemUp && isInRange()) {
    pararServir();
    interval = btnMedir.gap();
    systemUp = true;
    digitalWrite(OKLed, HIGH);
    digitalWrite(SirviendoLed, LOW);
    if (distance <= maximumRange && distance >= minimumRange) {
      abierto = true;
    } else if (distance > maximumRange * 2) {
      abierto = false;
    }
  }
}

/**
   Resetea el sistema
*/
void resetSistema() {
  if (!abierto) {
    btnPressed = true;
    systemUp = false;
    midiendo = false;
    interval = 1000;
    digitalWrite(OKLed, LOW);
  }
}

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
