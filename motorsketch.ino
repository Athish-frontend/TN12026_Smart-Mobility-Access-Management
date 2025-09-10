  char command;

 
  int LF_EN  = 5;
  int LF_IN1 = 8;
  int LF_IN2 = 9;

  int LR_EN  = 6;
  int LR_IN1 = 10;
  int LR_IN2 = 11;

  int RF_EN  = 3;
  int RF_IN1 = 2;
  int RF_IN2 = 4;

  int RR_EN  = 7;
  int RR_IN1 = 12;
  int RR_IN2 = 13;

  int speed = 150;  

  void setup() {
    Serial.begin(9600);

    // Left Front
    pinMode(LF_IN1, OUTPUT);
    pinMode(LF_IN2, OUTPUT);
    pinMode(LF_EN, OUTPUT);

    // Left Rear
    pinMode(LR_IN1, OUTPUT);
    pinMode(LR_IN2, OUTPUT);
    pinMode(LR_EN, OUTPUT);

    // Right Front
    pinMode(RF_IN1, OUTPUT);
    pinMode(RF_IN2, OUTPUT);
    pinMode(RF_EN, OUTPUT);

    // Right Rear
    pinMode(RR_IN1, OUTPUT);
    pinMode(RR_IN2, OUTPUT);
    pinMode(RR_EN, OUTPUT);

    stopMotors();

    Serial.println("Arduino Ready - 4 Motor Control Active");
  }

  void loop() {
    if (Serial.available() > 0) {
      command = Serial.read();
      Serial.print("Received: ");
      Serial.println(command);

      if (command == 'F') moveForward();
      else if (command == 'B') moveBackward();
      else if (command == 'L') turnLeft();
      else if (command == 'R') turnRight();
      else if (command == 'S') stopMotors();
      else Serial.println("Unknown command");
    }
  }

  
  void moveForward() {
    // Left Motors Forward
    digitalWrite(LF_IN1, HIGH); digitalWrite(LF_IN2, LOW);
    digitalWrite(LR_IN1, HIGH); digitalWrite(LR_IN2, LOW);

    // Right Motors Forward
    digitalWrite(RF_IN1, HIGH); digitalWrite(RF_IN2, LOW);
    digitalWrite(RR_IN1, HIGH); digitalWrite(RR_IN2, LOW);

    setSpeed(speed);
    Serial.println("Moving Forward");
  }

  void moveBackward() {
    // Left Motors Backward
    digitalWrite(LF_IN1, LOW); digitalWrite(LF_IN2, HIGH);
    digitalWrite(LR_IN1, LOW); digitalWrite(LR_IN2, HIGH);

    // Right Motors Backward
    digitalWrite(RF_IN1, LOW); digitalWrite(RF_IN2, HIGH);
    digitalWrite(RR_IN1, LOW); digitalWrite(RR_IN2, HIGH);

    setSpeed(speed);
    Serial.println("Moving Backward");
  }

  void turnLeft() {
    // Left Motors Backward
    digitalWrite(LF_IN1, LOW); digitalWrite(LF_IN2, HIGH);
    digitalWrite(LR_IN1, LOW); digitalWrite(LR_IN2, HIGH);

    // Right Motors Forward
    digitalWrite(RF_IN1, HIGH); digitalWrite(RF_IN2, LOW);
    digitalWrite(RR_IN1, HIGH); digitalWrite(RR_IN2, LOW);

    setSpeed(speed);
    Serial.println("Turning Left");
  }

  void turnRight() {
    // Left Motors Forward
    digitalWrite(LF_IN1, HIGH); digitalWrite(LF_IN2, LOW);
    digitalWrite(LR_IN1, HIGH); digitalWrite(LR_IN2, LOW);

    // Right Motors Backward
    digitalWrite(RF_IN1, LOW); digitalWrite(RF_IN2, HIGH);
    digitalWrite(RR_IN1, LOW); digitalWrite(RR_IN2, HIGH);

    setSpeed(speed);
    Serial.println("Turning Right");
  }

  void stopMotors() {
    setSpeed(0);

    // Left Motors Off
    digitalWrite(LF_IN1, LOW); digitalWrite(LF_IN2, LOW);
    digitalWrite(LR_IN1, LOW); digitalWrite(LR_IN2, LOW);

    // Right Motors Off
    digitalWrite(RF_IN1, LOW); digitalWrite(RF_IN2, LOW);
    digitalWrite(RR_IN1, LOW); digitalWrite(RR_IN2, LOW);

    Serial.println("Stopped");
  }

  void setSpeed(int spd) {
    analogWrite(LF_EN, spd);
    analogWrite(LR_EN, spd);
    analogWrite(RF_EN, spd);
    analogWrite(RR_EN, spd);
  }
