#include <MPU6050_tockn.h>
#include <Wire.h>

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

//odom var start
float dPA,dPB;
float prev_enc1,prev_enc2;
float curr_enc_1, curr_enc_2;
float thetta;
float ab = 30.8;
float aa = 29.2;
float dX,dY;
//odom var end

#define ENCODER_A_1 32 
#define ENCODER_B_1 33 

#define ENCODER_A_2 25 
#define ENCODER_B_2 26 

MPU6050 mpu6050(Wire);

volatile int encoder_value_1 = 0; 
volatile int encoder_value_2 = 0; 

void encoder_isr_1() {
  int A_1 = digitalRead(ENCODER_A_1);
  int B_2 = digitalRead(ENCODER_B_1);

  if ((A_1 == HIGH) != (B_2 == LOW)) {
    encoder_value_1--;
  } else {
    encoder_value_1++;
  }
}

void encoder_isr_2() {
  int A_3 = digitalRead(ENCODER_A_2);
  int B_4 = digitalRead(ENCODER_B_2);

  if ((A_3 == HIGH) != (B_4 == LOW)) {
    encoder_value_2--;
  } else {
    encoder_value_2++;
  }
}

int cond = 0;
int task = 0;
char data_;
int turn_ = 0;
//float convV;

//pid odometry
float sp_y = 0; 
float sp_x = 0; 
float sp_t = 0; 

//PID parameters - tuned by the user
float proportional = 1; //k_p = 0.5
float integral = 0.00; //k_i = 3
float derivative = 0.00; //k_d = 1
float controlSignal = 0; //u - Also called as process variable (PV)

//PID-related
float previousTime = 0; //for calculating delta t
float previousError = 0; //for calculating the derivative (edot)
float errorIntegral = 0; //integral error
float currentTime = 0; //time in the moment of calculation
float deltaTime = 0; //time difference
float errorValue = 0; //error
float edot = 0; //derivative (de/dt)

void setup() {
  Serial.begin(115200); 
  Serial2.begin(38400);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  
  pinMode(ENCODER_A_1, INPUT_PULLUP);
  pinMode(ENCODER_B_1, INPUT_PULLUP);
  pinMode(ENCODER_A_2, INPUT_PULLUP);
  pinMode(ENCODER_B_2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A_1), encoder_isr_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_2), encoder_isr_2, CHANGE);
  
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
    odometry();
    mpu6050.update();
//    controller();
    debug();

//    if(cond == 1){ //kombinasi 1
//      walk_loop_1();
//    }else if(cond == 2){ //kombinasi 2
//      walk_loop_2();
//    }else if(cond == 3){ //kombinasi 3
//      walk_loop_3();
//    }else if(cond == 4){ //kombinasi 4
//      walk_loop_4();
//    }else if(cond == 5){ //rotate -90 deg
//      turn_right();
//    }else if(cond == 6){ //rotate 90 deg
//      turn_left();
//    }else if(cond == 7){ //rotate 90 deg
//      rectangle();
//    }
//    else if(cond == 9){ //kombinasi 4
//      back_home();
//    }


}

void odometry(){
  thetta = mpu6050.getAngleZ();
  dX = (encoder_value_1 * cos(ab + (thetta/57.3)) + encoder_value_2 * cos(aa + (thetta/57.3))) / sin(ab-aa);
  dY = (encoder_value_1 * sin(ab + (thetta/57.3)) + encoder_value_2 * sin(aa + (thetta/57.3))) / sin(ab-aa);
}

void debug(){
//debug start
    SerialBT.print(" last pos :");
    SerialBT.print(dY);
    SerialBT.print("     ");
    SerialBT.print(dX);
    SerialBT.print("     ");
    SerialBT.println(thetta);
//debug end
}

void controller(){
    if(SerialBT.available()){
        
        data_ = SerialBT.read();
        Serial.println(data_);
        if(data_ == '1'){task = 1;cond = 1;walk_loop_1();}
        else if(data_ == '2'){task = 1;cond = 2;walk_loop_2();}
        else if(data_ == '3'){task = 1;cond = 3;walk_loop_3();}
        else if(data_ == '4'){task = 1;cond = 4;walk_loop_4();}
        else if(data_ == '5'){task = 1;cond = 5;turn_right();}
        else if(data_ == '6'){task = 1;cond = 6;turn_left();}
        else if(data_ == '7'){task = 1;cond = 7;rectangle();}
        else if(data_ == '9'){task = 1; cond = 9; back_home();}
        else if(data_ == '0'){task = 0;}
        else if(data_ == 'k'){ // debug position
          SerialBT.print("last pos :");
          SerialBT.print(dY);
          SerialBT.print("     ");
          SerialBT.print(dX);
          SerialBT.print("     ");
          SerialBT.println(thetta);
        }else{
          Serial2.println(0);
        }
        delay(100);
      }
}


void PID_cal(float sb_ ,float sp_){
  currentTime = micros(); 
  deltaTime = (currentTime - previousTime) / 1000000.0; 
  previousTime = currentTime; 
  
  errorValue = sb_ - sp_; 
  edot = (errorValue - previousError) / deltaTime; 
  errorIntegral = errorIntegral + (errorValue * deltaTime); 
  controlSignal = (proportional * errorValue) + (derivative * edot) + (integral * errorIntegral); 
//  convV = (controlSignal / deltaTime)/1000;
  
  previousError = errorValue; 
}



void go_y_axis(){
      turn_ = 1;
      PID_cal(dY, sp_y);
      SerialBT.print(controlSignal); //output yang akan jadi Vy
      
      if(controlSignal < -200){
        Serial2.println(1);
      }else if(controlSignal > 200){
        Serial2.println(-1);
      }else if(controlSignal < -1000){
        Serial2.println(2);
      }else if(controlSignal > 1000){
        Serial2.println(-2);
      }else{
        Serial2.println(0);
        task++;
      }
}
void go_x_axis(){
      turn_ = -1;
      PID_cal(dX, sp_x);
      SerialBT.print(controlSignal); //output yang akan jadi Vy
      
      if(controlSignal < -200){
        Serial2.println(3);
      }else if(controlSignal > 200){
        Serial2.println(-3);
      }else if(controlSignal < -1000){
        Serial2.println(4);
      }else if(controlSignal > 1000){
        Serial2.println(-4);
      }else{
        Serial2.println(0);
        task++;
      }
}
void go_t_axis(){
      PID_cal(thetta , sp_t);
      SerialBT.print(controlSignal); //output yang akan jadi Vy
      
      if(controlSignal < -5){
        Serial2.println(-5);
      }else if(controlSignal > 5){
        Serial2.println(5);
      }else if(controlSignal < -15){
        Serial2.println(-6);
      }else if(controlSignal > 15){
        Serial2.println(6);
      }else{
        Serial2.println(0);
        task++;
      }
}

void walk_loop_1(){
    odometry();
    mpu6050.update();
    controller();
      if(task == 1){
        sp_y = 4800;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else if(task == 2){
        sp_y = 0;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else if(task == 3){
        sp_y = 4800;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else{
        cond = 0;
      }
      delay(50);
}

void walk_loop_2(){
      if(task == 1){
        sp_y = 4800;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else if(task == 2){
        sp_y = 0;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else if(task == 3){
        sp_y = 4800;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else if(task == 4){
        sp_t = 180;
        go_t_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else{
        cond = 0;
      }
      delay(50);
}

void walk_loop_3(){
      if(task == 1){
        sp_y = 4800;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else{
        cond = 0;
      }
      delay(50);  
}

void walk_loop_4(){
      if(task == 1){
        sp_x = 4800;
        go_x_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else{
        cond = 0;
      }
      delay(50);  
}

void turn_right(){      
  if(task == 1){
        sp_t = -90;
        go_t_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else{
        cond = 0;
      }
      delay(50); 
}

void turn_left(){      
  if(task == 1){
        sp_t = 90;
        go_t_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else{
        cond = 0;
      }
      delay(50); 
}

void rectangle(){
      if(task == 1){
        sp_y = 4800;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else if(task == 2){
        sp_x = 4800;
        go_x_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else if(task == 3){
        sp_y = 0;
        go_y_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else if(task == 4){
        sp_x = 0;
        go_x_axis();
        SerialBT.print("    ");
        SerialBT.print(task);
        debug();
      }else{
        cond = 0;
      }
      delay(50);  
}

void back_home(){
//  benerin sudut dulu
      if(task == 1){
          while(thetta >= 5 || thetta <= -5){
          sp_t = 0;
          go_t_axis();
          SerialBT.print("    ");
          SerialBT.print(task);
          debug();
        }
      }else if(task == 2){
          if(turn_ == 1){ // arah sb Y
              task = 2;
          }else if(turn_ == -1){ //arah sb X
              task = 3;
          }
        if(dY >= 0 || dY <= 0){
          sp_y = 0;
          go_y_axis();
          SerialBT.print("    ");
          SerialBT.print(task);
          debug();
        }
      }else if(task == 3){
        if(dX >= 0 || dX <= 0){
          sp_x = 0;
          go_x_axis();
          SerialBT.print("    ");
          SerialBT.print(task);
          debug();
        }
      }else{
        cond = 0;
      }
      delay(50);
}
