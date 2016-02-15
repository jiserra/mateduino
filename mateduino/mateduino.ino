
// defines pins numbers
const int trigPin = 8;
const int echoPin = 7;
const int OKLed = 2;
const int SirviendoLed = 3;
const int RelayPin = 10;
const int boton = 4;
const int botonMedir = 5;
const int iluminaLed = 12;

int val = 0;

const int maximumRange = 5;
const int minimumRange = 3;

boolean abierto = false;
boolean tiempo = false;
boolean btnPressed = false;
boolean systemUp = false;
boolean midiendo = false;

// defines variables
long duration;
int distance;

unsigned long previousMillis = 0; // last time update
long interval = 1000; // interval at which to do something (milliseconds)
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
  pinMode(boton, INPUT);
  pinMode(botonMedir, INPUT);
  pinMode(iluminaLed, OUTPUT);
}

void loop() {

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

  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(distance);

  if (tiempo && (currentMillis - previousMillis > interval)) {
    tiempo = false;
    digitalWrite(SirviendoLed, LOW);
    digitalWrite(RelayPin, HIGH);
    delay(2000);
    //Serial.print("NO");
  }

  if (!btnPressed && systemUp) {
    if (distance <= maximumRange && distance >= minimumRange && !abierto) {
      if (mateMillis == 0) {
        mateMillis = millis();
      }
      if (millis() - mateMillis > 1000) {
        digitalWrite(SirviendoLed, HIGH);
        digitalWrite(RelayPin, LOW);
        //Serial.print('Abriendo el agua ' + interval + ' milisegundos');
        previousMillis = currentMillis;
        tiempo = true;
        abierto = true;
      }
    } else if (distance > maximumRange * 2 && abierto && !tiempo) {
      abierto = false;
      //Serial.println("Salio");
      mateMillis = 0;
    }
  }
// Boton reset
  if (!abierto || !tiempo) {
    val = digitalRead(boton);
    if (val) {
      btnPressed = true;
      systemUp = false;
      midiendo = false;
      interval = 1000;
      digitalWrite(OKLed, LOW);
    } else {
      btnPressed = false;
    }
  }

// Boton servir
  if (!systemUp && distance <= maximumRange && distance >= minimumRange) {
    val = digitalRead(botonMedir);
    digitalWrite(RelayPin, !val);
    if (val) {
      if (!midiendo) {
        inicioMedicionMillis = millis();
        midiendo = true;
      }
      digitalWrite(SirviendoLed, HIGH);
    } else if (midiendo && !val) {
      finMedicionMillis = millis();
      interval = finMedicionMillis - inicioMedicionMillis;
      //Serial.print(interval);
      systemUp = true;
      digitalWrite(OKLed, HIGH);
      digitalWrite(SirviendoLed, LOW);
      midiendo = false;

      if (distance <= maximumRange && distance >= minimumRange) {
        abierto = true;
      } else if (distance > maximumRange * 2) {
        abierto = false;
      }
    }
  }

  if(distance <= maximumRange && distance >= minimumRange) {
    digitalWrite(iluminaLed, HIGH);
  } else {
    digitalWrite(iluminaLed, LOW);
  }

  delay(100);

}
