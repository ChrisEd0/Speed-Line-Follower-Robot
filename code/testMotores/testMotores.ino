#define PWMA 6
#define AIN2 7
#define AIN1 8
#define BIN1 9
#define BIN2 10
#define PWMB 11

int velocidad = 0;

void setup()
{
  Serial.begin(115200);

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  Serial.println("Listo. Envia valor -100 a 100");
}

void setMotor(int speed)
{
  int pwm = map(abs(speed), 0, 100, 0, 255);

  if (speed > 0)
  {
    // ADELANTE
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }
  else if (speed < 0)
  {
    // REVERSA
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  }
  else
  {
    // FRENO
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }

  analogWrite(PWMA, pwm);
  analogWrite(PWMB, pwm);
}

void loop()
{
  if (Serial.available())
  {
    velocidad = Serial.parseInt();

    velocidad = constrain(velocidad, -100, 100);

    Serial.print("Velocidad: ");
    Serial.println(velocidad);

    setMotor(velocidad);
  }
}