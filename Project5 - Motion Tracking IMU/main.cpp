// Code written by Tyler Montcalm based on open source code
// The included libraries are not of my development

#include <Arduino.h>
#include <Wire.h>
#include <TimerOne.h>
#include <attitude_estimator.h>
#include <quaternion.h>
#include <vector.h>

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00  
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00  
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18
//using namespace std;
using namespace stateestimation;

AttitudeEstimator Est;


// This function read Nbytes bytes from I2C device at address Address. 
// Put read bytes starting at register Register in the Data array. 
void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();
  
  // Read Nbytes
  Wire.requestFrom(Address, Nbytes); 
  uint8_t index=0;
  while (Wire.available())
    Data[index++]=Wire.read();
}

// Write a byte (Data) in device (Address) at register (Register)
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();
}

void get_sensor_data(double g[3],double a[3], double m[3]);
Quaternion QuaternionBetweenVectors(double u[3],double v[3]);
// Counter
long int cpt=0;

// Initial time 
double g[3], a[3], m[3];
double a_static[3];
double previousTime=0;
double currentTime=0;
double elapsedTime=0;
double gyroAngleX=0;
double gyroAngleY=0;
double initial_euler[3];
double yaw=0;
double roll=0;
double pitch=0;
double accAngleX=0;
double accAngleY=0;
double lastmillis=0;
double HeadingValue=0;
Quaternion InitialQ;

//Quaternion values
void QuaternionToEuler(double &pitch,double &roll, double &yaw, Quaternion q);
void PrintValues();
void CalculateEuler();
void CalculateHeading(double roll, double pitch,double &heading);
void angles_setup();
void CalculateHeading2(Quaternion q, double &HeadingValue);
volatile bool intFlag=false;

void callback()
{ 
  intFlag=true;
  digitalWrite(13, digitalRead(13) ^ 1);
}
// Initializations

void setup()
{
  // Arduino initializations
  Wire.begin();
  Serial.begin(115200);
  // Set accelerometers low pass filter at 5Hz
  I2CwriteByte(MPU9250_ADDRESS,29,0x06);
  // Set gyroscope low pass filter at 5Hz
  I2CwriteByte(MPU9250_ADDRESS,26,0x06);
  // Configure gyroscope range
  I2CwriteByte(MPU9250_ADDRESS,27,GYRO_FULL_SCALE_1000_DPS);
  // Configure accelerometers range
  I2CwriteByte(MPU9250_ADDRESS,28,ACC_FULL_SCALE_4_G);
  // Set by pass mode for the magnetometers
  I2CwriteByte(MPU9250_ADDRESS,0x37,0x02);
  // Request continuous magnetometer measurements in 16 bits
  I2CwriteByte(MAG_ADDRESS,0x0A,0x16);
  pinMode(13, OUTPUT);
  Timer1.initialize(10000);         // initialize timer1, and set a 1/2 second period
  Timer1.attachInterrupt(callback); // attaches callback() as a timer overflow interrupt
  a_static[0]=0.0;
  a_static[1]=0.0;
  a_static[2]=1.0;
  angles_setup();
  lastmillis=millis();
  yaw=initial_euler[2];
}
   

// Main loop, read and display data
void loop()
{
  // Read accelerometer and gyroscope
  get_sensor_data(g, a, m);
  CalculateEuler();
  CalculateHeading2(InitialQ,HeadingValue);
  
  if((millis()-lastmillis)>=1500)
  {
    PrintValues();
    lastmillis=millis();
  }
}


void get_sensor_data(double g[3],double a[3], double m[3])
{
  int count=0;
  while(count!=5)
  {
    double mag_strength=0;
uint8_t Buf[14];
  I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);
  
  // Create 16 bits values from 8 bits data
  
  // Accelerometer
  int16_t ax=-(Buf[0]<<8 | Buf[1]);
  int16_t ay=-(Buf[2]<<8 | Buf[3]);
  int16_t az=Buf[4]<<8 | Buf[5];
  // Accelerometer Conversions
  a[0]+=ax/10485.0; //using 10485 for right now was 8192 but it was giving 1.28g as gravity.. now it gives 1g
  a[1]+=ay/10485.0;
  a[2]+=az/10485.0;
  
  // Gyroscope
  int16_t gx=-(Buf[8]<<8 | Buf[9]);
  int16_t gy=-(Buf[10]<<8 | Buf[11]);
  int16_t gz=Buf[12]<<8 | Buf[13];
  gx=gx/32.8;
  gy=gy/32.8;
  gz=gz/32.8;
   g[0]+=gx; 
   g[1]+=gy;
   g[2]+=gz;



  
  // Read register Status 1 and wait for the DRDY: Data Ready
  I2CwriteByte(MAG_ADDRESS,0x0A,0x01); 
  uint8_t ST1;
  do
  {
    I2Cread(MAG_ADDRESS,0x02,1,&ST1);
  }
  while (!(ST1&0x01));

  // Read magnetometer data  
  uint8_t Mag[7];  
  I2Cread(MAG_ADDRESS,0x03,7,Mag);
  

  // Create 16 bits values from 8 bits data
  
  // Magnetometer
  int16_t mx=-(Mag[3]<<8 | Mag[2]);
  int16_t my=-(Mag[1]<<8 | Mag[0]);
  int16_t mz=-(Mag[5]<<8 | Mag[4]);
  m[0]+=mx;
  m[1]+=my;
  m[2]+=mz;
 
 mag_strength=sqrt((m[0]*m[0])+(m[1]*m[1])+(m[2]*m[2]));

count++;
  }
    a[0]=a[0]/5.0;
    a[1]=a[1]/5.0;
    a[2]=a[2]/5.0;
    g[0]=g[0]/5.0;
    g[1]=g[1]/5.0;
    g[2]=g[2]/5.0;
    m[0]=m[0]/5.0;
    m[1]=m[1]/5.0;
    m[2]=m[2]/5.0;
    
    return;

}


void PrintValues()
{
 Serial.println();
 Serial.print("Pitch: "); Serial.print(pitch);
 Serial.print("    Roll: "); Serial.print(roll);
 Serial.print("    Yaw: "); Serial.print(yaw);
 Serial.print("    Heading Value:"); Serial.print(HeadingValue);
 Serial.print("    Ax =");Serial.print(a[0]);Serial.print("    Ay =");Serial.print(a[1]);Serial.print("    Az =");Serial.print(a[2]);
 Serial.print("    Mx =");Serial.print(m[0]);Serial.print("    My =");Serial.print(m[1]);Serial.print("    Mz =");Serial.print(m[2]);
 Serial.println();
}

void CalculateEuler()
{
accAngleX = atan2f(a[1], a[2]) * 180 / M_PI;
accAngleY = atan2f(a[0], a[2]) * 180 / M_PI;
elapsedTime=(millis()-previousTime)/1000;
previousTime=millis();
gyroAngleX = gyroAngleX + g[0] * elapsedTime; // deg/s * s = deg
gyroAngleY = gyroAngleY + g[1] * elapsedTime;
yaw = yaw + g[2] * elapsedTime; //+ initial_euler[2];
roll = 0.96 * gyroAngleX + 0.04 * accAngleX;// +initial_euler[1]
pitch = 0.96 * gyroAngleY + 0.04 * accAngleY; //+initial_euler[0]
}

void CalculateHeading(double roll, double pitch,double &heading) 
{
double headX;
double headY;
double cos_roll;
double sin_roll;
double cos_pitch;
double sin_pitch;

cos_roll=cos(roll*.0174);
sin_roll=sqrt(1-(cos_roll*cos_roll));
cos_pitch=cos(pitch*.0174);
sin_pitch=sqrt(1-(cos_pitch*cos_pitch));

// Tilt compensated magnetic field X component:
    headX = m[0]*cos_pitch+m[1]*sin_roll*sin_pitch+m[2]*cos_roll*sin_pitch;
    // Tilt compensated magnetic field Y component:
    headY = m[1]*cos_roll-m[2]*sin_roll;
    // magnetic heading
    heading = atan2(-headY,headX); 
}

void CalculateHeading2(Quaternion q, double &HeadingValue)
{
double magnitude_magnetometer=sqrt(m[0]*m[0]+m[1]*m[1]+m[2]*m[2]);
double x,y,z;
x=m[0]/magnitude_magnetometer;
y=m[1]/magnitude_magnetometer;
z=m[2]/magnitude_magnetometer;
Vector Vm(x,y,z);
Quaternion qinv=q.Inverted(); //quaternion that gives us the rotation to go back to flat ground
Vector VmRotated= qinv*Vm;

HeadingValue =(57.29*atan2(VmRotated.y, VmRotated.x)); //gives heading value in rad
}

Quaternion QuaternionBetweenVectors(double u[3],double v[3])
{
Quaternion Q;
Vector v1(u[0],u[1],u[2]);
Vector v2(v[0],v[1],v[2]);
Vector a=v1.Cross(v2);
Q.v.x=a.x;
Q.v.y=a.y;
Q.v.z=a.z;
Q.w=sqrt( (v1.Length()*v1.Length()) * (v2.Length()*v2.Length())  ) + v1.Dot(v2);

return Q;
}

void QuaternionToEuler(double &pitch,double &roll, double &yaw, Quaternion q)
{
  // roll
double sinr_cosp=2*(q.w*q.v.x+q.v.y*q.v.z);
double cosr_cosp=1-2*(q.v.x*q.v.x+q.v.y*q.v.y);
roll=atan2(sinr_cosp,cosr_cosp);

//pitch
double sinp = 2 * (q.w * q.v.y - q.v.z * q.v.x);
if(abs(sinp)>=1)
{
pitch= copysign(M_PI/2,sinp);
}
else
{
pitch=asin(sinp);  
}
// yaw
double siny_cosp = 2 * (q.w * q.v.z + q.v.x * q.v.y);
double cosy_cosp = 1 - 2 * (q.v.y * q.v.y + q.v.z * q.v.z);
yaw = atan2(siny_cosp, cosy_cosp);
}

void angles_setup()
{
  lastmillis=millis();
  while((millis()-lastmillis)<=2500) // Future: add a check here to make sure G force is within +/- 10percent of 1g total.
  {
    get_sensor_data(g,a,m);
    delay(20);
  }
  
    Quaternion Q =QuaternionBetweenVectors(a,a_static); // returns a quaternion based on what rotation was made from the accel vector to the 0,0,1 accel vector.
    InitialQ=Q; 
    QuaternionToEuler(initial_euler[0], initial_euler[1], initial_euler[2], Q);
    Serial.println("Done initial euler angle calculations");
    Serial.print("Pitch Initial: "); Serial.print(initial_euler[0]); Serial.print("  Roll Initial: "); Serial.print(initial_euler[1]); Serial.print("  Yaw Initial: "); Serial.print(initial_euler[2]);
}