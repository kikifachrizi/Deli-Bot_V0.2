#include <MPU6050_tockn.h>
#include <Wire.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

const int trigPin1 = 13;
const int echoPin2 = 12;

const int trigPin3 = 18;
const int echoPin4 = 5;

const int trigPin5 = 27;
const int echoPin6 = 14;

const int trigPin7 = 19;
const int echoPin8 = 34;

const int trigPin9 = 23;
const int echoPin10 = 35;

const int trigPin11 = 15;
const int echoPin12 = 2;

float duration, distance;
float mes_1,mes_2,mes_3,mes_4,mes_5,mes_6;

char data_;
float dPA, dPB;
float prev_enc1, prev_enc2;
float curr_enc_1, curr_enc_2;
float thetta;
float ab = 1.0472; // 60 deg
float aa = 5.23599; //300 deg
float dX, dY;
float X,Y;
float output[3];

#define ENCODER_A_1 32 
#define ENCODER_B_1 33 

#define ENCODER_A_2 26
#define ENCODER_B_2 25 

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

int task;
float Exr_s;
float Eyr_s;
float cS_th_s;

long unsigned int prev_t;

int status_jalan;
float ultra_threshold = 20.0;

int movement = 0;

void setup() {
  Serial.begin(115200); 
  Serial2.begin(9600);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin4, INPUT);
  pinMode(trigPin5, OUTPUT);
  pinMode(echoPin6, INPUT);
  pinMode(trigPin7, OUTPUT);
  pinMode(echoPin8, INPUT);
  pinMode(trigPin9, OUTPUT);
  pinMode(echoPin10, INPUT);
  pinMode(trigPin11, OUTPUT);
  pinMode(echoPin12, INPUT);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  
  pinMode(ENCODER_A_1, INPUT_PULLUP);
  pinMode(ENCODER_B_1, INPUT_PULLUP);
  pinMode(ENCODER_A_2, INPUT_PULLUP);
  pinMode(ENCODER_B_2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A_1), encoder_isr_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_2), encoder_isr_2, CHANGE);
  
  SerialBT.begin("Deli-Bot");
}

void loop() {
  mpu6050.update();
  mes_2 = read_ultra(trigPin1, echoPin2);
  mes_5 = read_ultra(trigPin3, echoPin4);
  mes_4 = read_ultra(trigPin5, echoPin6);
  mes_1 = read_ultra(trigPin7, echoPin8);
  mes_3 = read_ultra(trigPin9, echoPin10);
  mes_6 = read_ultra(trigPin11, echoPin12);


  if(millis() - prev_t >= 10){
    prev_t = millis();
    odometry();
//    debug_RE();
    sm_output();
//    debug_ultra();
    controller();
//gotopos(float cmdX, float cmdY, float cmdDeg, float vmaxtrans, float vmaxrot, float actX, float actY, float actDeg, int transtol, int rottol)
  switch(movement){
        case 1: //pengujian maju
          if(status_jalan == 1){
            if(gotopos(0,100,0,3,1,X,Y,thetta,3,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
        
        case 2: // pengujian mundur
          if(status_jalan == 1){
            if(gotopos(0,-100,0,3,1,X,Y,thetta,10,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break ;
        case 3: // kanan
          if(status_jalan == 1){
            if(gotopos(100,0,0,3,1,X,Y,thetta,10,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
        case 4: // kiri
          if(status_jalan == 1){
            if(gotopos(-100,0,0,3,1,X,Y,thetta,10,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
        case 5: // start zone
          if(status_jalan == 1){
            if(gotopos(0,0,0,3,1,X,Y,thetta,3,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
        case 6: // go to table 1
          if(status_jalan == 1){
            if(gotopos(0,123,0,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
              if(gotopos(178,124,0,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 3){
              if(gotopos(192,359,0,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 4){
              if(gotopos(260,459,0,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 5){
              if(gotopos(250,1001,0,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }
          else if(status_jalan == 6){
              if(gotopos(270,1095,-63,3,1,X,Y,thetta,10,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }
          else if(status_jalan == 7){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
        case 7: // back to midpoint
          if(status_jalan == 1){
            if(gotopos(278,1095,-180,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
              if(gotopos(284,626,-180,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 3){
              if(gotopos(284,473,-270,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 4){
              if(gotopos(200,475,-270,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 5){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
//        case 8: // go to table 2
//          if(status_jalan == 1){
//            if(gotopos(323,427,97,3,1,X,Y,thetta,5,5)){
//              SerialBT.print(X);
//              SerialBT.print(" ");
//              SerialBT.print(Y);
//              SerialBT.print(" ");
//              SerialBT.println(thetta);
//              status_jalan++;
//            }
//          }else if(status_jalan == 2){
//              if(gotopos(257,421,98,3,1,X,Y,thetta,5,5)){
//              SerialBT.print(X);
//              SerialBT.print(" ");
//              SerialBT.print(Y);
//              SerialBT.print(" ");
//              SerialBT.println(thetta);
//              status_jalan++;
//            }
//          }else if(status_jalan == 3){
//              if(gotopos(200,418,98,3,1,X,Y,thetta,5,5)){
//              SerialBT.print(X);
//              SerialBT.print(" ");
//              SerialBT.print(Y);
//              SerialBT.print(" ");
//              SerialBT.println(thetta);
//              status_jalan++;
//            }
//          }else if(status_jalan == 4){
//            status_jalan = 0;
//          }else{
//            Exr_s = 0;
//            Eyr_s = 0;
//            cS_th_s = 0;
//          }
//        break;
        case 8: // go to home
          if(status_jalan == 1){
            if(gotopos(175,441,-180,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
              if(gotopos(160,180,-180,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 3){
              if(gotopos(0,167,-180,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 4){
              if(gotopos(0,60,-180,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 5){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
        case 9: // go to home
          if(status_jalan == 1){
            if(gotopos(110,0,0,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
              if(gotopos(110,220,-30,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 3){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
        case 10: // go to home
          if(status_jalan == 1){
            if(gotopos(110,0,0,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 2){
              if(gotopos(0,0,0,3,1,X,Y,thetta,5,5)){
              SerialBT.print(X);
              SerialBT.print(" ");
              SerialBT.print(Y);
              SerialBT.print(" ");
              SerialBT.println(thetta);
              status_jalan++;
            }
          }else if(status_jalan == 3){
            status_jalan = 0;
          }else{
            Exr_s = 0;
            Eyr_s = 0;
            cS_th_s = 0;
          }
        break;
        default:
          Exr_s = 0;
          Eyr_s = 0;
          cS_th_s = 0;
      }

    Serial2.print(Exr_s);
    Serial2.print(';');
    Serial2.print(Eyr_s);
    Serial2.print(';');
    Serial2.println(cS_th_s);
    delay(100);
  }
}

void odometry() {
  thetta = mpu6050.getAngleZ();
  dX = (encoder_value_2 * cos(ab + (thetta*0.01745329251)) + encoder_value_1 * cos(aa + (thetta*0.01745329251)));
  dY = (encoder_value_2 * sin(ab + (thetta*0.01745329251)) + encoder_value_1 * sin(aa + (thetta*0.01745329251)));

  encoder_value_1 = 0;
  encoder_value_2 = 0;

  X += (dX*0.01783);
  Y += (dY*0.01967);
}

void debug_RE() {
  Serial.print(encoder_value_1);
  Serial.print("     ");
  Serial.println(encoder_value_2);
}

void debug() {
  SerialBT.print(" last pos :");
  SerialBT.print(X);
  SerialBT.print("     ");
  SerialBT.print(Y);
  SerialBT.print("     ");
  SerialBT.println(thetta);
}

void debug_pid_pos() {
  SerialBT.print(" PID pos :");
  SerialBT.print(Exr_s);
  SerialBT.print("     ");
  SerialBT.print(Eyr_s);
  SerialBT.print("     ");
  SerialBT.println(cS_th_s);
}

void sm_output(){
  SerialBT.print(X);
  SerialBT.print("     ");
  SerialBT.print(Y);
  SerialBT.print("     ");
  SerialBT.print(thetta);
  SerialBT.print("     ");
  SerialBT.print(mes_1);
  SerialBT.print("   ");
  SerialBT.print(mes_2);
  SerialBT.print("   ");
  SerialBT.print(mes_3);
  SerialBT.print("   ");
  SerialBT.print(mes_4);
  SerialBT.print("   ");
  SerialBT.print(mes_5);
  SerialBT.print("   ");
  SerialBT.println(mes_6);
  delay(50);
}

void controller() {
  if (SerialBT.available()) {
    data_ = SerialBT.read();
    Serial.println(data_);
    if (data_ == 'a') {movement = 1;status_jalan = 1;} // pengujian maju
    else if(data_ == 'b'){movement = 2;status_jalan = 1;} //pengujian mundur
    else if(data_ == 'c'){movement = 3;status_jalan = 1;}//pengujian kanan
    else if(data_ == 'd'){movement = 4;status_jalan = 1;}//pengujian kiri
    else if(data_ == '1'){movement = 6;status_jalan = 1;}//path pola 1 go to table
    else if(data_ == '2'){movement = 7;status_jalan = 1;}//path pola 2 back to table 2
    else if(data_ == '3'){movement = 8;status_jalan = 1;}//path pola 3 go to table 2
    else if(data_ == '4'){movement = 9;status_jalan = 1;}//path pola 4 go to home
    else if(data_ == 'o'){movement = 5;status_jalan = 1;}
    else if(data_ == '0'){status_jalan = 0;}
    delay(100);
  }
}

float read_ultra(int pin_trig, int pin_echo){
  digitalWrite(pin_trig, LOW);
  delayMicroseconds(2);
  digitalWrite(pin_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig, LOW);

  duration = pulseIn(pin_echo, HIGH);
  distance = (duration*.0343)/2;
  return distance;
}

int gotopos(float cmdX, float cmdY, float cmdDeg, float vmaxtrans, float vmaxrot, float actX, float actY, float actDeg, int transtol, int rottol) {
    float kp[3] = {0.125, 0.125, 0.125};
    float ki[3] = {0.0011, 0.0011, 0.0011};
    float kd[3] = {0.00, 0.00, 0.00};
    float pose[3] = {0, 0, 0};
    float setpoint[3] = {0, 0, 0};
    float error[3] = {0, 0, 0};
    float prev_error[3] = {0, 0, 0};
    float p[3] = {0, 0, 0};
    float i[3] = {0, 0, 0};
    float d[3] = {0, 0, 0};

    setpoint[0] = cmdX;
    setpoint[1] = cmdY;
    setpoint[2] = cmdDeg;

    pose[0] = actX;
    pose[1] = actY;
    pose[2] = actDeg;

    // Logika penghindaran objek
    if (mes_1 <= ultra_threshold || mes_2 <= ultra_threshold || mes_3 <= ultra_threshold || mes_4 <= ultra_threshold || mes_5 <= ultra_threshold || mes_6 <= ultra_threshold) {
        // Objek terdeteksi
        vmaxtrans = 2; // Kurangi kecepatan translasi saat objek terdeteksi

        // Logika penghindaran objek berdasarkan posisi sensor
        if (mes_3 <= ultra_threshold) {
            // Objek di kanan depan, robot bergerak ke kiri
            setpoint[1] = pose[1] - 10;
        } else if (mes_2 <= ultra_threshold) {
            // Objek di kanan serong depan, robot bergerak ke kiri
            setpoint[0] = pose[0] - 10;
        } else if (mes_4 <= ultra_threshold) {
            // Objek di kiri depan, robot bergerak ke kanan
            setpoint[1] = pose[1] + 10;
        } else if (mes_5 <= ultra_threshold) {
            // Objek di serong kiri depan, robot bergerak ke kanan
            setpoint[0] = pose[0] + 10;
        } else if (mes_1 <= ultra_threshold) {
            // Objek di kanan serong belakang, robot bergerak ke kiri
            setpoint[0] = pose[0] - 10;
        } else if (mes_6 <= ultra_threshold) {
            // Objek di serong kiri belakang, robot bergerak ke kanan
            setpoint[0] = pose[0] + 10;
        } else if (mes_3 <= ultra_threshold && mes_4 <= ultra_threshold) {
            // Objek di depan, robot bergerak mundur sejenak
            setpoint[1] = pose[1] - 20;
        }
    } else {
        // Tidak ada objek, ikuti setpoint tujuan
        setpoint[0] = cmdX;
        setpoint[1] = cmdY;
        setpoint[2] = cmdDeg;
    }
    
    for (int ii = 0; ii < 3; ii++) {
        error[ii] = setpoint[ii] - pose[ii];

        if (ii == 2) {
            if (error[ii] > 180) {
                setpoint[ii] -= 360;
                error[ii] = setpoint[ii] - pose[ii];
            } else if (error[ii] < -180) {
                setpoint[ii] += 360;
                error[ii] = setpoint[ii] - pose[ii];
            }
        }

        p[ii] = kp[ii] * error[ii];
        i[ii] += ki[ii] * error[ii];
        d[ii] = kd[ii] * (error[ii] - prev_error[ii]);

        prev_error[ii] = error[ii];

        if (ii == 2) {
            if (i[ii] > vmaxrot) i[ii] = vmaxrot;
            else if (i[ii] < -vmaxrot) i[ii] = -vmaxrot;
        } else {
            if (i[ii] > vmaxtrans) i[ii] = vmaxtrans;
            else if (i[ii] < -vmaxtrans) i[ii] = -vmaxtrans;
        }

        output[ii] = p[ii] + i[ii] + d[ii];

        if (ii == 2) {
            if (output[ii] > vmaxrot) output[ii] = vmaxrot;
            else if (output[ii] < -vmaxrot) output[ii] = -vmaxrot;
        } else {
            if (output[ii] > vmaxtrans) output[ii] = vmaxtrans;
            else if (output[ii] < -vmaxtrans) output[ii] = -vmaxtrans;
        }
    }

    Exr_s = output[0] * cos(thetta * 0.0174533) + output[1] * sin(thetta * 0.0174533);
    Eyr_s = -output[0] * sin(thetta * 0.0174533) + output[1] * cos(thetta * 0.0174533);
    cS_th_s = -output[2];

    if ((abs(pose[0] - setpoint[0]) < transtol) && (abs(pose[1] - setpoint[1]) < transtol) && (abs(pose[2] - setpoint[2]) < rottol)) {
        return 1;
    }
    return 0;
}
