#include <Wire.h>

// Lita de slaves
const int colorSensorAddress = 0x01;
const int hallSensorAddress = 0x02; //Contador
const int Motor1Address = 0x03;
const int Motor2Address = 0x04;
const int Motor3Address = 0x05;
const int panicButtonAddress = 0x06;
const int sinalizationLightAddress = 0x06;
const int gasSensorAddress = 0x07;
const int LCDAddress = 0x08;
const int MiddlewareAddress = 0x09;
const int tempSensorAddress = 0x10;
const int alarmBuzzAddress = 0x10;
const int analogInPinVert = A0;  // Vertical
const int analogInPinHorz = A1;  // Horizontal
int buttonUpDown;
int buttonLeftRight;


bool alarmActive = false;
unsigned int tempValue = 0;
int colorValue; // 0 - black ; !0 - white; red - 9; green - 4/5/6; blue - 3
int Red = 9;
int Green = 5;
int Blue = 3;
int Empty = 0;
int panicButtonValue = 0;
int goProductionSide = 1;
int goProductionOppositeSide = 2;
int stopMotor = 3;
int greenLedOn = 2;
int redLedOn = 3;
int yellowLedOn = 4;
int ledsOff = 5;
int alarmButtonOff = 6;
unsigned int gasValue = 0;
unsigned int hallCntValue = 0;
int piecesCntr = 0;
int tempThreshold = 90; //Fahrenheit
int gasThreshold = 150; //

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void moveMotorProductionSide(int motorAddress) {
  Wire.beginTransmission(motorAddress);
  Wire.write(goProductionSide);
  Wire.endTransmission();
}

void moveMotorProductionOppositeSide(int motorAddress) {
  Wire.beginTransmission(motorAddress);
  Wire.write(goProductionOppositeSide);
  Wire.endTransmission();
}

void pauseMotor(int motorAddress) {
  Wire.beginTransmission(motorAddress);
  Wire.write(stopMotor);
  Wire.endTransmission();
}

void printLCD(String str) {
  Wire.beginTransmission(LCDAddress);
  Wire.write(str.c_str());
  Wire.endTransmission();
  Wire.requestFrom(LCDAddress, 1);
  while (Wire.available()) {
    Wire.read();
  }
}

void turnOnLeds(int ledsOn) {
  Wire.beginTransmission(sinalizationLightAddress);
  Wire.write(ledsOn);
  Wire.endTransmission();
}

void turnOffLeds() {
  Wire.beginTransmission(sinalizationLightAddress);
  Wire.write(ledsOff);
  Wire.endTransmission();
}

void clearScreen() {
  printLCD("                ");
}

void getTempValue() {
  Wire.beginTransmission(tempSensorAddress);
  Wire.write(3);
  Wire.endTransmission();
  Wire.requestFrom(tempSensorAddress, 2);    // request 2 bytes from slave device
  while (Wire.available()) { // slave may send less than requested
    int lowest = Wire.read(); // receive a byte as character
    int highest  = Wire.read(); // receive a byte as character
    tempValue = getInt(lowest, highest);
  }
}

void getColorValue() {
  Wire.beginTransmission(colorSensorAddress);
  Wire.write(66);
  Wire.endTransmission();
  Wire.requestFrom(colorSensorAddress, 1);    // request 1 bytes from slave device
  while (Wire.available()) { // slave may send less than requested
    colorValue = Wire.read(); // receive a byte as character
  }
}

void getPanicButtonValue() {
  Wire.beginTransmission(panicButtonAddress);
  Wire.write(1);
  Wire.endTransmission();
  Wire.requestFrom(panicButtonAddress, 1);    // request 1 bytes from slave device
  while (Wire.available()) { // slave may send less than requested
    panicButtonValue = Wire.read(); // receive a byte as character
  }
}

void offPanicButtonValue() {
  Wire.beginTransmission(panicButtonAddress);
  Wire.write(alarmButtonOff);
  Wire.endTransmission();
  Wire.requestFrom(panicButtonAddress, 1);    // request 1 bytes from slave device
  while (Wire.available()) { // slave may send less than requested
    Wire.read(); // receive a byte as character
  }
}
void getHallSensorValue() {
  Wire.requestFrom(hallSensorAddress, 2);    // request 2 bytes from slave device
  while (Wire.available()) { // slave may send less than requested
    int lowest = Wire.read(); // receive a byte as character
    int highest  = Wire.read(); // receive a byte as character
    hallCntValue = getInt(lowest, highest);
  }
}

void getGasSensorValue() {
  Wire.requestFrom(gasSensorAddress, 2);    // request 2 bytes from slave device
  while (Wire.available()) { // slave may send less than requested
    int lowest = Wire.read(); // receive a byte as character
    int highest  = Wire.read(); // receive a byte as character
    gasValue = getInt(lowest, highest);
  }
}

unsigned int getInt(byte lowest, byte highest) {
  return (unsigned int) 256 * (unsigned int) highest + (unsigned int) lowest;
}

void turnOnBuzz() {
  Wire.beginTransmission(alarmBuzzAddress);
  Wire.write(2);
  Wire.endTransmission();
}

void turnOffBuzz() {
  Wire.beginTransmission(alarmBuzzAddress);
  Wire.write(1);
  Wire.endTransmission();
}

void printTemp() {
  char str[16];
  sprintf(str, "T: %d", tempValue );
  printLCD(str);
}

void printColor() {
  char str[16];
  sprintf(str, "C: %d", colorValue );
  printLCD(str);
}

void printGas() {
  char str[16];
  sprintf(str, "G: %d", gasValue );
  printLCD(str);
}

void printHall() {
  char str[16];
  sprintf(str, "H: %d", hallCntValue );
  printLCD(str);
}

void printPieces() {
  char str[16];
  sprintf(str, "N Pieces: %d", piecesCntr );
  printLCD(str);
}

bool isRed() {
  if (colorValue <= Red + 1 && colorValue >= Red - 1)
    return true;
  else
    return false;
}

bool isEmpty() {
  if (colorValue == Empty)
    return true;
  else
    return false;
}

void loop() {

  buttonUpDown = analogRead(analogInPinVert);
  buttonLeftRight = analogRead(analogInPinHorz);

  if (buttonUpDown <= 20) {
    clearScreen();
    delay(1000);
    printPieces();
  } else if (buttonUpDown >=  800) {
    getTempValue();
    clearScreen();
    delay(1000);
    printTemp();
  }

  if (buttonLeftRight <= 200) {
    offPanicButtonValue();
    alarmActive = false;
  } else if (buttonLeftRight >= 800) {
    getGasSensorValue();
    clearScreen();
    delay(1000);
    printGas();
  }

  //************************* Security Measures ****************************************
  getPanicButtonValue();
  getTempValue();
  getGasSensorValue();

  if (panicButtonValue == 1) { //State 2 - Panic Button activated
    pauseMotor(Motor1Address);
    pauseMotor(Motor2Address);
    pauseMotor(Motor3Address);
    turnOnLeds(yellowLedOn);
    delay(500);
    clearScreen();
    delay(500);
    printLCD("Panic Button");
    alarmActive = true;
  } else if (tempValue > tempThreshold /*&& gasValue < gasThreshold*/) { //State 3 - Fire makes temperature go higher and also the CO2 values
    turnOnLeds(redLedOn);
    turnOnBuzz();
    delay(500);
    clearScreen();
    delay(500);
    printLCD("Fire!");
    alarmActive = true;
  } else {
    turnOffBuzz();
    turnOnLeds(greenLedOn); //State 1 - Normal Status
    delay(500);
    clearScreen();
    delay(500);
    printLCD("Normal Status");
    alarmActive = false;
  }
  //************************* Security Measures ****************************************


  //********************* Manufacturing Maintenance ************************************
  if (!alarmActive) {
    getColorValue();

    if (!isEmpty()) {
      if (isRed()) { //Product Rejected due to its color being wrong
        clearScreen();
        delay(1000);
        printLCD("Rejected Piece");
        moveMotorProductionSide(Motor1Address);
        moveMotorProductionSide(Motor3Address);
        delay(2500);
        pauseMotor(Motor1Address);
        pauseMotor(Motor3Address);
      } else { //Product accepted can continue on the production line
        clearScreen();
        delay(1000);
        printLCD("Accepted Piece");
        moveMotorProductionSide(Motor1Address);
        moveMotorProductionSide(Motor2Address);
        delay(5000);
        pauseMotor(Motor1Address);
        pauseMotor(Motor2Address);
        getHallSensorValue();

        if (hallCntValue < 500) {
          piecesCntr++;
          clearScreen();
          delay(1000);
          printPieces();
        }
      }
    }
  }
  else {
    clearScreen();
    delay(500);
    printLCD("Alarm Active");
  }
  //********************* Manufacturing Maintenance ************************************

}

