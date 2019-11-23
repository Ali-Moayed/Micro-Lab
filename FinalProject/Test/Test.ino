#include <FiniteStateMachine.h>

#include <LiquidCrystal.h>

//const int x =  0;
//const int x =  1;
//const int x =  8;

//const int x =  13; LED_BUILTIN


const int Button_0 =  2;
const int Button_1 =  3;

const int LCD_D7 =  4;
const int LCD_D6 =  5;
const int LCD_D5 =  6;
const int LCD_D4 =  7;
const int LCD_Enable =  11;
const int LCD_RS =  12;

const int Left_Motor_Enable =  9;
const int Right_Motor_Enable =  10;

const int Left_Sensor =  A0;
const int Middle_Sensor =  A1;
const int Right_Sensor =  A2;

int Left_Sensor_Limit = 511;
int Middle_Sensor_Limit = 511;
int Right_Sensor_Limit = 511;

int Left_Sensor_White = 0;
int Middle_Sensor_White = 0;
int Right_Sensor_White = 0;

int Left_Sensor_Black = 1023;
int Middle_Sensor_Black = 1023;
int Right_Sensor_Black = 1023;

char calibrationState = 0;
char settingsState = 0;

LiquidCrystal lcd(LCD_RS, LCD_Enable, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

unsigned long LED_PreviousMillis = 0;
unsigned long LCD_PreviousMillis = 0;

unsigned int LED_Interval = 1000;
unsigned int LCD_Interval = 5;

String LCD_Output_0 ="";
String LCD_Output_1 ="";

bool logChanged = true;
bool LCD_Delay_Enable = false;
unsigned int LCD_Delay = 0;
unsigned int LCD_Delay_Start = 0;

int rightMotorSpeed = 0;
int leftMotorSpeed = 0;
int power = 100;
const int minPower = 50;

String currentState = "";

/*
______________________________________________________________________________________________________
------------------------------------------------------------------------------------------------------
*/
void S_Start_Enter();
void S_Start_Update();
void S_Start_Exit();

void S_Settings_Enter();
void S_Settings_Update();
void S_Settings_Exit();

void S_Forward_Enter();
void S_Forward_Update();
void S_Forward_Exit();

void S_Left_Enter();
void S_Left_Update();
void S_Left_Exit();

void S_Right_Enter();
void S_Right_Update();
void S_Right_Exit();

void S_Lost_Enter();
void S_Lost_Update();
void S_Lost_Exit();
/*
______________________________________________________________________________________________________
------------------------------------------------------------------------------------------------------
*/
State S_Start = State(S_Start_Enter, S_Start_Update, S_Start_Exit);
State S_Settings = State(S_Settings_Enter, S_Settings_Update, S_Settings_Exit);
State S_Forward = State(S_Forward_Enter, S_Forward_Update, S_Forward_Exit);
State S_Left = State(S_Left_Enter, S_Left_Update, S_Left_Exit);
State S_Right = State(S_Right_Enter, S_Right_Update, S_Right_Exit);
State S_Lost = State(S_Lost_Enter, S_Lost_Update, S_Lost_Exit);

FSM stateMachine = FSM(S_Start);
/*
______________________________________________________________________________________________________
------------------------------------------------------------------------------------------------------
*/


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Right_Motor_Enable, OUTPUT);
  pinMode(Left_Motor_Enable, OUTPUT);
  pinMode(Button_0, INPUT_PULLUP);
  pinMode(Button_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Button_0), Button_0_Pressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_1), Button_1_Pressed, FALLING);

  lcd.begin(16, 2);

//  lcd.write(255);
//  Log(String(char(255)));
  Log(String(char(255)) + String(char(255)) + "Path finder" + String(char(255)) + String(char(255)) + String(char(255)),  String(char(255)) + String(char(255)) + String(char(255)) + String(char(255)) + String(char(255)) + "Robot" + String(char(255)) + String(char(255)) + String(char(255)) + String(char(255)) + String(char(255)) + String(char(255)), 2000);
}

void loop() {
  BlinkingLED();
  DebugingLCD();
  MotorManagement();
  stateMachine.update();
}



/*
______________________________________________________________________________________________________
------------------------------------------------------------------------------------------------------
*/

void SetMotorSpeed(int leftSpeed, int rightSpeed){
  leftMotorSpeed = (power*leftSpeed)/100;
  rightMotorSpeed = (power*rightSpeed)/100;
}

void MotorManagement(){
  analogWrite(Right_Motor_Enable, rightMotorSpeed);
  analogWrite(Left_Motor_Enable, leftMotorSpeed);
  
//  if(rightMotorSpeed > 0){
//    analogWrite(10,rightMotorSpeed);
//    analogWrite(9,0);
//  }else if(rightMotorSpeed = 0){
//    analogWrite(10,0);
//    analogWrite(9,0);
//  }else{
//    analogWrite(10,0);
//    analogWrite(9,rightMotorSpeed);
//  }
//
//  if(leftMotorSpeed > 0){
//    analogWrite(6,leftMotorSpeed);
//    analogWrite(5,0);
//  }else if(leftMotorSpeed = 0){
//    analogWrite(6,0);
//    analogWrite(5,0);
//  }else{
//    analogWrite(6,0);
//    analogWrite(5,leftMotorSpeed);
//  }
}

void DisplaySensors(){
  String left_sensor = String(analogRead(Left_Sensor));
  String middle_sensor = String(analogRead(Middle_Sensor));
  String right_sensor = String(analogRead(Right_Sensor));

  String s = currentState;
  for(int i = s.length(); i < 10; i++)
    s += " ";
  s += "|";
  if(analogRead(Left_Sensor) > Left_Sensor_Limit)
    s += String(char(255)) + " ";
  else{
    s += "_ ";
  }
  if(analogRead(Middle_Sensor) > Middle_Sensor_Limit)
    s += String(char(255)) + " ";
  else{
    s += "_ ";
  }
  if(analogRead(Right_Sensor) > Right_Sensor_Limit)
    s += String(char(255));
  else{
    s += "_";
  }
  
  Log(fixedSizeStringToRight(left_sensor, 4, "0") + "  " + fixedSizeStringToRight(middle_sensor, 4, "0") + "  " + fixedSizeStringToRight(right_sensor, 4, "0"), s);
}

void BlinkingLED(){
  unsigned long currentMillis = millis();

  if (currentMillis - LED_PreviousMillis >= LED_Interval) {
    LED_PreviousMillis = currentMillis;

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

void DebugingLCD(){
  if(LCD_Delay_Enable){
    if (millis() - LCD_Delay_Start >= LCD_Delay) {
      logChanged = true;
      LCD_Delay_Enable = false;
      LCD_Delay = 0;
      LCD_Delay_Start = 0;
    }
  }else if(logChanged){
    unsigned long currentMillis = millis();
  
    if (currentMillis - LCD_PreviousMillis >= LCD_Interval) {
      LCD_PreviousMillis = currentMillis;
      
      Set_LCD_Output();
    }

    logChanged = false;

    if(LCD_Delay > 0){
      LCD_Delay_Start = millis();
      LCD_Delay_Enable = true;
    }
  }
}

void Set_LCD_Output(){
  lcd.setCursor(0,0);
  lcd.print(LCD_Output_0);
  lcd.setCursor(0,1);
  lcd.print(LCD_Output_1);
}

void Log(String s){
  for(char i = s.length(); i < 16; i++){
    s += " ";
  }
  LCD_Output_0 = s;
  LCD_Output_1 = "                ";

  logChanged = true;
}

void Log(String s, int d){
  Log(s);
  LCD_Delay = d;
  
  LCD_Delay_Enable = false;
  DebugingLCD();
}

void Log(String s0, String s1){
  for(char i = s0.length(); i < 16; i++){
    s0 += " ";
  }
  for(char i = s1.length(); i < 16; i++){
    s1 += " ";
  }
  LCD_Output_0 = s0;
  LCD_Output_1 = s1;

  logChanged = true;
}

void Log(String s0, String s1, int d){
  Log(s0, s1);
  LCD_Delay = d;

  LCD_Delay_Enable = false;
  DebugingLCD();
}

String fixedSizeStringToRight(String s, char i, String space){
  String output = s;
  for(char t = output.length(); t < i; t++)
    output = space + output;
  return output;
}

void Button_0_Pressed(){
  if(stateMachine.isInState(S_Start)){
    switch(calibrationState++){
    case 0:
      //Log("Left Sensor:", "White");
      break;
    case 1:
      Left_Sensor_White = analogRead(Left_Sensor);
      //Log("Left Sensor:", "Black");
      break;
    case 2:
      Left_Sensor_Black = analogRead(Left_Sensor);
      //Log("Middle Sensor:", "White");
      break;
    case 3:
      Middle_Sensor_White = analogRead(Middle_Sensor);
      //Log("Middle Sensor:", "Black");
      break;
    case 4:
      Middle_Sensor_Black = analogRead(Middle_Sensor);
      //Log("Right Sensor:", "White");
      break;
    case 5:
      Right_Sensor_White = analogRead(Right_Sensor);
      //Log("Right Sensor:", "Black");
      break;
    case 6:
      Right_Sensor_Black = analogRead(Right_Sensor);
      Left_Sensor_Limit = (Left_Sensor_White + Left_Sensor_Black)/2;
      Middle_Sensor_Limit = (Middle_Sensor_White + Middle_Sensor_Black)/2;
      Right_Sensor_Limit = (Right_Sensor_White + Right_Sensor_Black)/2;
      Log("calibration", "Compelete", 2000);
      stateMachine.transitionTo(S_Lost);
      break;
     default:
      break;
    }
  }else if(stateMachine.isInState(S_Settings)){
    if(settingsState == 0){
      settingsState++;
    }else{
      if(power < 100)
        power += 5;
      else
        power = minPower;
    }
  }else{
    stateMachine.transitionTo(S_Settings);
  }
}

void Button_1_Pressed(){
  if(stateMachine.isInState(S_Start)){
    stateMachine.transitionTo(S_Lost);
  }else if(stateMachine.isInState(S_Settings)){
    if(settingsState == 0){
      stateMachine.transitionTo(S_Start);
    }else{
      stateMachine.transitionTo(S_Lost);
    }
  }
}


/*
______________________________________________________________________________________________________
------------------------------------------------------------------------------------------------------
*/

//---------------------------------------------------->S_Start
void S_Start_Enter(){
  currentState = "Start";
  calibrationState = 0;

  SetMotorSpeed(0, 0);
}
void S_Start_Update(){
  switch(calibrationState){
    case 0:
      if((millis()/500)%2 == 0)
        Log("--Calibration---", ">Start     >Skip");
      else
        Log("--Calibration---", " Start      Skip");
      break;
    case 1:
      Log("Left Sensor:" + fixedSizeStringToRight(String(analogRead(Left_Sensor)), 4, "0"), "White");
      break;
    case 2:
      Log("Left Sensor:" + fixedSizeStringToRight(String(analogRead(Left_Sensor)), 4, "0"), "Black");
      break;
    case 3:
      Log("Middle Sensor:" + fixedSizeStringToRight(String(analogRead(Middle_Sensor)), 4, "0"), "White");
      break;
    case 4:
      Log("Middle Sensor:" + fixedSizeStringToRight(String(analogRead(Middle_Sensor)), 4, "0"), "Black");
      break;
    case 5:
      Log("Right Sensor:" + fixedSizeStringToRight(String(analogRead(Right_Sensor)), 4, "0"), "White");
      break;
    case 6:
      Log("Right Sensor:" + fixedSizeStringToRight(String(analogRead(Right_Sensor)), 4, "0"), "Black");
      break;
    default:
      break;
  }
}
void S_Start_Exit(){
  
}

//---------------------------------------------------->S_Settings
void S_Settings_Enter(){
  currentState = "Start";
  settingsState = 0;

  SetMotorSpeed(0, 0);
}
void S_Settings_Update(){
  if(settingsState == 0){
    if((millis()/500)%2 == 0)
      Log("----Settings----", ">Power    >Calib");
    else
      Log("----Settings----", " Power     Calib");
  }else{
    if((millis()/500)%2 == 0)
      Log("Power: " + String(power) + "%", ">+5        >Exit");
    else
      Log("Power: " + String(power) + "%", " +5         Exit");
  }
}
void S_Settings_Exit(){
  
}

//----------------------------------------------------S_Forward
void S_Forward_Enter(){
  currentState = "Forward";
  
  SetMotorSpeed(255, 255);

//  Log("Forward", 1000);  
}
void S_Forward_Update(){
  if(analogRead(Middle_Sensor) <= Middle_Sensor_Limit){
    if((analogRead(Left_Sensor) > Left_Sensor_Limit) & (analogRead(Right_Sensor) <= Right_Sensor_Limit)){
      stateMachine.transitionTo(S_Left);
    }else if((analogRead(Left_Sensor) <= Left_Sensor_Limit) & (analogRead(Right_Sensor) > Right_Sensor_Limit)){
      stateMachine.transitionTo(S_Right);
    }else{
      stateMachine.transitionTo(S_Lost);
    }
  }else{
    DisplaySensors();
  }
}
void S_Forward_Exit(){
  
}

//---------------------------------------------------->S_Left
void S_Left_Enter(){
  currentState = "Left";
  
  SetMotorSpeed(63, 255);

//  Log("Left", 1000);
}
void S_Left_Update(){
  if(analogRead(Left_Sensor) > Left_Sensor_Limit){
    DisplaySensors();
  }else if(analogRead(Middle_Sensor) > Middle_Sensor_Limit){
    stateMachine.transitionTo(S_Forward);
  }else{
    stateMachine.transitionTo(S_Lost);
  }
}
void S_Left_Exit(){
  
}

//---------------------------------------------------->S_Right
void S_Right_Enter(){
  currentState = "Right";
  
  SetMotorSpeed(255, 63);

//  Log("Right", 1000);
}
void S_Right_Update(){
  if(analogRead(Right_Sensor) > Right_Sensor_Limit){
    DisplaySensors();
  }else if(analogRead(Middle_Sensor) > Middle_Sensor_Limit){
    stateMachine.transitionTo(S_Forward);
  }else{
    stateMachine.transitionTo(S_Lost);
  }
}
void S_Right_Exit(){
  
}

//---------------------------------------------------->S_Lost
void S_Lost_Enter(){
  currentState = "Lost";
  
  SetMotorSpeed(255, 255);

//  Log("Lost", 1000);
}
void S_Lost_Update(){
  if(analogRead(Middle_Sensor) > Middle_Sensor_Limit){
    stateMachine.transitionTo(S_Forward);
  }else if((analogRead(Left_Sensor) > Left_Sensor_Limit) & (analogRead(Right_Sensor) <= Right_Sensor_Limit)){
    stateMachine.transitionTo(S_Left);
  }else if((analogRead(Left_Sensor) <= Left_Sensor_Limit) & (analogRead(Right_Sensor) > Right_Sensor_Limit)){
    stateMachine.transitionTo(S_Right);
  }else{
    DisplaySensors();
  }
}
void S_Lost_Exit(){
  
}
