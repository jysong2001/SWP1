#include <Servo.h>
#define servo_pin 10
#define sensor_pin A3

#define dist_target 255
#define dist_min 100
#define dist_max 410

#define dist_alpha 0.15

#define duty_MIN 1000
#define duty_neu 1200 //수평 1450
#define duty_MAX 2000

#define servo_speed 300
#define servo_angle 30

#define interval_dist 20
#define interval_servo 20
#define interval_serial 100

#define kp 1.0
#define ki 0.01
#define kd 25.0

Servo myservo;

float dist_raw, dist_ema;

int duty_min, duty_max;

unsigned long last_sampling_time_dist, last_sampling_time_servo, last_sampling_time_serial;
bool event_dist, event_servo, event_serial;

int duty_chg;
int duty_target, duty_curr;
float error_curr, error_prev, control, pterm, dterm, dterm_prev, iterm;

void setup() {
  myservo.attach(servo_pin); //서보모터 핀 설정
  myservo.writeMicroseconds(duty_MIN);
  Serial.begin(57600); //시리얼 동기화
  duty_chg = (duty_MAX - duty_MAX) * (servo_speed / 180.0) * (interval_servo / 1000.0);

  last_sampling_time_dist = 0;
  last_sampling_time_servo = 0;
  last_sampling_time_serial = 0;

  event_dist = false;
  event_servo = false;
  event_serial = false;

  pterm = 0;
  iterm = 0;
  dterm = 0;

  dist_raw = 255;
  duty_min = 1000;
  duty_max = 1300;
  delay(5000);
}

void loop() {
  dterm_prev = dterm;
  if(millis() >= last_sampling_time_dist + interval_dist){
    event_dist = true;
  }
  if(millis() >= last_sampling_time_servo + interval_servo){
    event_servo = true;
  }
  if(millis() >= last_sampling_time_serial + interval_serial){
    event_serial = true;
  }

  if(event_dist){
    event_dist = false;
    dist_raw = ir_distance_filtered();
    error_curr = dist_target - dist_raw;
    pterm = kp * error_curr;
    iterm += ki * error_curr;
    dterm = kd * (error_curr - error_prev);
    control = pterm + iterm + dterm;
    duty_target = duty_neu + control;

    duty_target = min(max(duty_target, duty_min), duty_max);

    error_prev = error_curr;
    
    last_sampling_time_dist += interval_dist;
  }

  
  if(event_servo){
    event_servo = false;
    myservo.writeMicroseconds(duty_target);
    last_sampling_time_servo += interval_servo;
  }

  if(event_serial){
    event_serial = false;
    Serial.print("IR:");
    Serial.print(dist_raw);
    Serial.print(",T:");
    Serial.print(dist_target);
    Serial.print(",P:");
    Serial.print(map(pterm,-1000,1000,510,610));
    Serial.print(",D:");
    Serial.print(map(dterm,-1000,1000,510,610));
    Serial.print(",I:");
    Serial.print(map(iterm,-1000,1000,510,610));
    Serial.print(",DTT:");
    Serial.print(map(duty_target,1000,2000,410,510));
    Serial.print(",DTC:");
    Serial.print(map(duty_curr,1000,2000,410,510));
    Serial.println(",-G:245,+G:265,m:0,M:800");
    last_sampling_time_serial += interval_serial;
  }
}
float ir_distance(void){
  float val;
  float volt = float(analogRead(sensor_pin));
  val = ((6762.0/(volt - 9.0)) - 4.0) * 10.0;
  val = 100 + 300.0 / (480 - 73) * (val - 73);
  return val;
}
  
float ir_distance_filtered(void){
  static float val;
  static float dist_ema = 0;
  float raw = ir_distance();
  val = raw;
  dist_ema = dist_alpha * val + (1- dist_alpha) * dist_ema;
  return dist_ema;
}
