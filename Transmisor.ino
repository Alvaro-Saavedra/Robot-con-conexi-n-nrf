#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
MPU6050 mpu6050(Wire);
// Define the digital inputs
#define jB1 1  // Joystick button 1
#define jB2 0  // Joystick button 2
#define t1 7   // Toggle switch 1
#define t2 4   // Toggle switch 1
#define b1 8   // Button 1
#define b2 9   // Button 2
#define b3 2   // Button 3
#define b4 3   // Button 4

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY;
float angleX, angleY, angleZ;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY;
float elapsedTime, currentTime, previousTime;
int c = 0;


RF24 radio(5, 6);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001"; // Address

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte button1;
  byte button2;
  byte button3;
  byte button4;
  byte anglexcontrol;
  byte angleycontrol;
  byte anglezcontrol;
};

Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  
  // Initialize interface to the MPU6050
  //initialize_MPU6050();

  // Call this function if you need to get the IMU error values for your module
  //calculate_IMU_error();
  
  // Define the radio communication
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.printDetails();
  // Activate the Arduino internal pull-up resistors
  pinMode(jB1, INPUT_PULLUP);
  pinMode(jB2, INPUT_PULLUP);
  pinMode(t1, INPUT_PULLUP);
  pinMode(t2, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);
  
  // Set initial default values
  data.j1PotX = 127; // Values from 0 to 255. When Joystick is in resting position, the value is in the middle, or 127. We actually map the pot value from 0 to 1023 to 0 to 255 because that's one BYTE value
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.j1Button = 1;
  data.j2Button = 1;
  data.pot1 = 1;
  data.pot2 = 1;
  data.tSwitch1 = 1;
  data.tSwitch2 = 1;
  data.button1 = 1;
  data.button2 = 1;
  data.button3 = 1;
  data.button4 = 1;
  data.anglexcontrol = 123;
  data.angleycontrol = 109;
  data.anglezcontrol = 127;
}
void loop() {
  mpu6050.update();
  if((mpu6050.getAngleX()<90)and (mpu6050.getAngleX()>-90)){
    angleX=mpu6050.getAngleX();
  }
  if((mpu6050.getAngleY()<90)and (mpu6050.getAngleY()>-90)){
    angleY=mpu6050.getAngleY();
  }
  if((mpu6050.getAngleZ()<180)and (mpu6050.getAngleZ()>-180)){
    angleZ=mpu6050.getAngleZ();
  }
  //Serial.println(angleZ);
  //Serial.print(angleY);
  //Serial.println(mpu6050.getAngleZ());
  // Read all analog inputs and map them to one Byte value
  data.j1PotX = map(analogRead(A1), 0, 1023, 0, 255); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 255
  data.j1PotY = map(analogRead(A0), 0, 1023, 0, 255);
  data.j2PotX = map(analogRead(A2), 0, 1023, 0, 255);
  data.j2PotY = map(analogRead(A3), 0, 1023, 0, 255);
  data.pot1 = map(analogRead(A7), 0, 1023, 0, 255);
  data.pot2 = map(analogRead(A6), 0, 1023, 0, 255);
  data.anglexcontrol = map(angleX, -90, 90, 0, 255);
  data.angleycontrol = map(angleY, -90, 90, 0, 255);
  data.anglezcontrol = map(angleZ, -180, 180, 0, 255);
  // Read all digital inputs
  data.j1Button = digitalRead(jB1);
  data.j2Button = digitalRead(jB2);
  data.tSwitch1 = digitalRead(t1);
  data.tSwitch2 = digitalRead(t2);
  data.button1 = digitalRead(b1);
  data.button2 = digitalRead(b2);
  data.button3 = digitalRead(b3);
  data.button4 = digitalRead(b4);
  // If toggle switch 1 is switched on
  //if (digitalRead(t1) == 0) {
    //read_IMU();    // Use MPU6050 instead of Joystick 1 for controling left, right, forward and backward movements
  //}
  // Send the whole data from the structure to the receiver
  radio.write(&data, sizeof(Data_Package));
  //Serial.print("anglex ");
  //Serial.println(data.tSwitch2);
//Serial.print(data.button1);
  //Serial.print(data.tSwitch2);
  //Serial.print(data.button3);
  //Serial.println(data.button4);
  Serial.print(data.j1PotY);
  Serial.print("------");
  Serial.println(data.j1PotX);
  }

//void initialize_MPU6050() {
  //Wire.begin();                      // Initialize comunication
  //Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  //Wire.write(0x6B);                  // Talk to the register 6B
  //Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  //Wire.endTransmission(true);        //end the transmission
  // Configure Accelerometer
  //Wire.beginTransmission(MPU);
  //Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register
  //Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  //Wire.endTransmission(true);
  // Configure Gyro
  //Wire.beginTransmission(MPU);
  //Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  //Wire.write(0x10);                   // Set the register bits as 00010000 (1000dps full scale)
  //Wire.endTransmission(true);
//}
