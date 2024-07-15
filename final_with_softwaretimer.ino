#include <AFMotor.h>
#include <Servo.h>

unsigned long startMillis=0;//to store the time when all the sensors detect white for the first time

Servo myservo;

AF_DCMotor motor1(1);//Motor 1 connected to M1 pin
AF_DCMotor motor2(2);//Motor 2 connected to M2 pin
AF_DCMotor motor3(3);//Motor 3 connected to M3 pin
AF_DCMotor motor4(4);//Motor 4 connected to M4 pin

int leftSensor=A1;
int rightSensor=A0;
int middleSensor=A3;

bool timer_flag=true; //state flag to trigger the start timer for the first time all sensors detect white and reset when any of them detect black 

int threshold= 500;//Setting appropriate threshold

void setup() {
  Serial.begin(9600);

  myservo.attach(10);//Servo attached to pin 10
  myservo.write(175);//Initial angle of servo

  motor1.setSpeed(255);//Initial speed of BO motor
  motor2.setSpeed(255);//Initial speed of BO motor
  motor3.setSpeed(255);//Initial speed of BO motor
  motor4.setSpeed(255);//Initial speed of BO motor
}


void linefollow(int leftValue, int middleValue, int rightValue){
  if(leftValue > threshold && rightValue > threshold && middleValue > threshold){
    //All sensors detects black - rotates to find the line
    motor1.setSpeed(150);
    motor2.setSpeed(150);
    motor3.setSpeed(150);
    motor4.setSpeed(150);
    motor1.run(FORWARD);
    motor2.run(BACKWARD);
    motor3.run(FORWARD);
    motor4.run(BACKWARD);
  }
  else if(leftValue > threshold && rightValue >threshold) {
    //On the line
    motor1.setSpeed(255);
    motor2.setSpeed(255);
    motor3.setSpeed(255);
    motor4.setSpeed(255);
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
  }
  else if(leftValue < threshold && rightValue > threshold) {
    //Moving towards left
    motor1.setSpeed(150);
    motor2.setSpeed(150);
    motor3.setSpeed(150);
    motor4.setSpeed(150);
    motor1.run(BACKWARD);
    motor2.run(FORWARD);
    motor3.run(BACKWARD);
    motor4.run(FORWARD);
  }
  else if(leftValue > threshold && rightValue < threshold) {
    //Moving towards right
    motor1.setSpeed(150);
    motor2.setSpeed(150);
    motor3.setSpeed(150);
    motor4.setSpeed(150);
    motor1.run(FORWARD);
    motor2.run(BACKWARD);
    motor3.run(FORWARD);
    motor4.run(BACKWARD);
  }
}


void payload_drop() {
   
  myservo.write(115);//Dropping payload
  delay(3000);
  //Coming back for a while
  motor1.setSpeed(255);
  motor2.setSpeed(255);
  motor3.setSpeed(255);
  motor4.setSpeed(255);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  delay(400);
  myservo.write(175);//Back to initial position of box


  //Rotating by 180 degrees to follow back the line
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);
  delay(800);
}


void loop() {
  int leftValue=analogRead(leftSensor);
  int rightValue=analogRead(rightSensor);
  int middleValue=analogRead(middleSensor);


  //To check values in Serial monitor and calibrate the sensors
  Serial.print("Right: ");
  Serial.print(rightValue);
  Serial.print("  Middle: ");
  Serial.print(middleValue);
  Serial.print("  Left: ");
  Serial.println(leftValue);


  if(leftValue < threshold && middleValue < threshold && rightValue < threshold) {
    //All sensors detect white - STOP command
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);

    if(timer_flag==true){
      startMillis = millis();  //to store the time when all the sensors detect white for the first time
      timer_flag=false;
    }

    unsigned long currentMillis = millis();  // Get the current time

    if(currentMillis-startMillis >= 2000){
      payload_drop(); //drop payload only if all sensors continously detect white for more than 2 seconds
    }
  }
  else {
    timer_flag=true;
    linefollow(leftValue, middleValue, rightValue);
  }
  delay(50);//Appropriate belay between loops
}
