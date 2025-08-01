#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <AFMotor.h>
#include <Servo.h>
Servo servo1;
Servo servo2;
Servo servo3;
AF_DCMotor motorA(3);
AF_DCMotor motorB(2);
AF_DCMotor motorC(1);
AF_DCMotor motorD(4);
int vel=255;
int angulo=0;
//int val=0;
int posicionmedia=127;

RF24 radio(44, 46);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001";

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

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
//---------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  servo1.attach(43);
  servo2.attach(9);
  servo3.attach(7);
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); //  Set the module as receiver
  resetData();
  motorA.setSpeed(250);
  motorB.setSpeed(250);
  motorC.setSpeed(250);
  motorD.setSpeed(250);
}
//---------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis(); // At this moment we have received the data
  }
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values
  }
  if(data.anglezcontrol == posicionmedia-32){
    posicionmedia=posicionmedia-32;
    }
  if(data.anglezcontrol == posicionmedia+32){
    posicionmedia=posicionmedia+32;
    }
  // Print the data in the Serial Monitor
  //Serial.print("j1PotX: ");
  //Serial.print(data.j1PotX);
  //Serial.print("; j1PotY: ");
  //Serial.println(data.j1PotY);
  //Serial.print("; button1: ");
  //Serial.println(data.button1);
  //Serial.print("; button2: ");
  //Serial.print(data.button2); 
  //Serial.print("; tSwitch1");
  //Serial.print(data.tSwitch1);
  //Serial.print("; pot1");
  //Serial.print(data.pot1);
  //Serial.print("; j2PotX: ");
  //Serial.print(data.j2PotX);
  //Serial.print("; j2PotY: ");
  //Serial.print(data.j2PotY);
  //Serial.print("; button3: ");
  //Serial.print(data.button3);
  //Serial.print("; button4: ");
  //Serial.println(data.button4); 
  //Serial.print("; tSwitch2");
  //Serial.print(data.tSwitch2);
  //Serial.print("; pot2");
  //Serial.println(data.pot2);

//---------------------------------------------------------------------------------------------------------------------------------------

  //angulo=map(data.pot2, 0, 1023, 0, 360);
  //if (data.button3==1);
  
    //servo1.write(180);//servo para subir el ascensor
  //Serial.println(angulo);
  if (data.button3==1){
    servo2.write(180);
    servo3.write(0);
  }
  else if(data.button3==0){
    servo2.write(0);
    servo3.write(180);
  }
  if (data.button4==1){
    servo1.write(180);
    }
  else if(data.button4==0){
    servo1.write(120);
    }
  if (data.tSwitch1 ==0){
    if (data.tSwitch2 ==0){
      if (data.j2PotX >= 160){
        Serial.println("Giro izquierda");
        giroizquierda();
        } 
      else if ( 94>= data.j2PotX){
        Serial.println("Giro derecha");
        giroderecha();
        } 
      else if ((data.j1PotY > 127) and (data.j1PotX >= 127 and data.j1PotX <= 129)){
        Serial.println("Adelante");
        adelante();
        }
      else if ((data.j1PotY < 126) and (data.j1PotX >= 127 and data.j1PotX <= 129)){
        Serial.println("Atras");
        atras();
        }
      else if ((data.j1PotX < 127) and (data.j1PotY >= 126 and data.j1PotY <= 127)){
        Serial.println("Derecha");
        derecha();
        }
      else if ((data.j1PotX > 129) and (data.j1PotY >= 126 and data.j1PotY <= 127)){
        Serial.println("Izquierda");
        izquierda();
        }
      else if ((data.j1PotX==255) and (data.j1PotY==255)){ //
        Serial.println("Diagonal izquierda adelante");
        diad();
        }
      else if ((data.j1PotX==255) and (data.j1PotY==0)){
        Serial.println("Diagonal izquierda atras");
        diat();
        }
      else if ((data.j1PotX==0) and (data.j1PotY==255)){
        Serial.println("Diagonal derecha adelante");
        ddad();
        }
      else if ((data.j1PotX==0) and (data.j1PotY==0)){ //
        Serial.println("Diagonal derecha atras");
        ddat();
        }
      else if (data.button3==0){
        Serial.println("Servo");
        servo2.write(0);
        delay(30);
        servo2.write(90);
        }
      else{
        detenido();
      
      }
    }
    else{
      if(90>=data.angleycontrol){
        atras();
      }
      else if(data.angleycontrol>=140){
        adelante();
      }
      else if(107>=data.anglexcontrol){
        izquierda();
      }
      else if(data.anglexcontrol>=140){
        derecha();
      }
      else if (data.anglezcontrol >= (posicionmedia+13)){
        Serial.println("Giro izquierda");
        giroizquierda();
        } 
      else if ( (posicionmedia-13)>= data.anglezcontrol){
        Serial.println("Giro derecha");
        giroderecha();
        }
      else if (data.button3==0){
        Serial.println("Servo");
        servo2.write(0);
        delay(30);
        servo2.write(90);
      }
      else{
        detenido();
      }
    }
  }
  //else if (data.button3==0){
    // Serial.println("Servo");
     //servo2.write(0);
     //delay(30);
     //servo2.write(120);
  //}
  else{
    detenido();
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------
void resetData() {
  // Reset the values when there is no radio connection - Set initial default values
  Serial.println("RESET DATA");
  data.j1PotX = 127;
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
  data.angleycontrol = 109;
  data.anglexcontrol=123;
  data.anglezcontrol=127;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void giroizquierda(){
    motorA.setSpeed(vel-55);  
    motorB.setSpeed(vel-55);  
    motorC.setSpeed(vel-55);
    motorD.setSpeed(vel-55);
    
    motorA.run(BACKWARD);           
    motorB.run(FORWARD);
    motorC.run(FORWARD);
    motorD.run(BACKWARD);
    delay(3);
  }
void giroderecha(){
  motorA.setSpeed(vel-55);  
  motorB.setSpeed(vel-55);
  motorC.setSpeed(vel-55);
  motorD.setSpeed(vel-55);

  motorA.run(FORWARD);           
  motorB.run(BACKWARD);
  motorC.run(BACKWARD);
  motorD.run(FORWARD);
  delay(3);
  }
//---------------------------------------------------------------------------------------------------------------------------------------
void adelante(){
    motorA.setSpeed(vel);
    motorB.setSpeed(vel);
    motorC.setSpeed(vel);
    motorD.setSpeed(vel);

    motorA.run(FORWARD);
    motorB.run(FORWARD);
    motorC.run(FORWARD);
    motorD.run(FORWARD);
    delay(3);
  }
void atras(){
  motorA.setSpeed(vel);
    motorB.setSpeed(vel);
    motorC.setSpeed(vel);
    motorD.setSpeed(vel);
    
    motorA.run(BACKWARD);
    motorB.run(BACKWARD);
    motorC.run(BACKWARD);
    motorD.run(BACKWARD);
    delay(3);
  }
void derecha(){
  motorA.setSpeed(vel);
    motorB.setSpeed(vel);
    motorC.setSpeed(vel);
    motorD.setSpeed(vel);

    motorA.run(FORWARD);
    motorB.run(BACKWARD);
    motorC.run(FORWARD);
    motorD.run(BACKWARD);
    delay(3);
  }
 void izquierda(){
  motorA.setSpeed(vel);
    motorB.setSpeed(vel);
    motorC.setSpeed(vel);
    motorD.setSpeed(vel);

    motorA.run(BACKWARD);
    motorB.run(FORWARD);
    motorC.run(BACKWARD);
    motorD.run(FORWARD);
    delay(3); 
  }
//---------------------------------------------------------------------------------------------------------------------------------------
void diad(){
  motorB.setSpeed(vel);
      motorD.setSpeed(vel);
    
      motorB.run(FORWARD);
      motorD.run(FORWARD);
      delay(3);
  }
void diat(){
  motorA.setSpeed(vel);
      motorC.setSpeed(vel);

      motorA.run(BACKWARD);
      motorC.run(BACKWARD);
      delay(3);
  }
void ddad(){
  motorA.setSpeed(vel);
      motorC.setSpeed(vel);

      motorA.run(FORWARD);
      motorC.run(FORWARD);
      delay(3);
  }
void ddat(){
  motorB.setSpeed(vel);
      motorD.setSpeed(vel);
    
      motorB.run(BACKWARD);
      motorD.run(BACKWARD);
      delay(3);
  }
void detenido(){
  motorA.setSpeed(0);
      motorB.setSpeed(0);
      motorC.setSpeed(0);
      motorD.setSpeed(0);
    
      motorA.run(FORWARD);
      motorB.run(FORWARD);
      motorC.run(FORWARD);
      motorD.run(FORWARD);
  }
