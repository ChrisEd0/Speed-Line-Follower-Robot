#include <QTRSensors.h>

// ===================== TB6612 =====================
#define PWMA 6
#define AIN1 8
#define AIN2 7

#define PWMB 11
#define BIN1 9
#define BIN2 10

#define BUTTON_PIN 5

// ===================== QTR =====================
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// ===================== CONFIG =====================
const int umbral = 750;

float kp = 0.13;
float kd = 2.75;

int baseSpeed = 90;

int lastError = 0;
int lastPosition = 3500;

// ===================== MOTOR =====================
void setForward() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void setPWM(int left, int right) {
  analogWrite(PWMA, right);
  analogWrite(PWMB, left);
}

void stopMotors() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

// ===================== LECTURA DIGITAL =====================
void readSensors(int &pos, int &active) {

  active = 0;
  long sum = 0;

  for (int i = 0; i < 8; i++) {

    int v = sensorValues[i];

    if (v > umbral) {
      active++;
      sum += (long)i * 1000;
    }
  }

  if (active > 0) {
    pos = sum / active;
  } else {
    pos = lastPosition; // mantiene dirección si pierde línea
  }

  lastPosition = pos;
}

// ===================== CONTROL =====================
void runRobot() {

  setForward();

  while (true) {

    if (digitalRead(BUTTON_PIN) == LOW) {
      delay(200);
      stopMotors();
      break;
    }

    qtr.read(sensorValues);

    int pos, active;
    readSensors(pos, active);

    int error = pos - 3500;

    // ===================== DERIVADA =====================
    int derivative = error - lastError;
    lastError = error;

    // ===================== PID =====================
    float vel = (kp * error) + (kd * derivative);

    // ===================== CURVA EXTREMA =====================
    int vl, vr;

    if (active <= 1) {

      // curva extrema izquierda
      if (sensorValues[0] > umbral) {
        vl = 0;
        vr = baseSpeed + 60;
      }
      // curva extrema derecha
      else if (sensorValues[7] > umbral) {
        vl = baseSpeed + 60;
        vr = 0;
      }
      // pérdida total
      else {
        vl = baseSpeed;
        vr = -baseSpeed;
      }

    } else {

      // ===================== VELOCIDAD DINÁMICA =====================
      int absError = abs(error);

      int reduction = 0;

      if (absError < 600) reduction = 0;
      else if (absError < 1800) reduction = map(absError, 600, 2000, 15, 75);
      else reduction = 90;

      int dynamicBase = baseSpeed - reduction;
      dynamicBase = constrain(dynamicBase, 65, baseSpeed);

      vl = dynamicBase - vel;
      vr = dynamicBase + vel;
    }

    // ===================== LIMITES =====================
    vl = constrain(vl, 0, 255);
    vr = constrain(vr, 0, 255);

    setPWM(vr, vl);

    delay(5);
  }
}

// ===================== SETUP =====================
void setup() {

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(BUTTON_PIN, INPUT);

  Serial.begin(115200);

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0,A1,A2,A3,A4,A5,A6,A7}, SensorCount);

  Serial.println("Calibrando...");

  for (int i = 0; i < 200; i++) {
    qtr.calibrate();
    delay(5);
  }

  Serial.println("Listo");
}

// ===================== LOOP =====================
void loop() {

  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(200);
    runRobot();
  }
}