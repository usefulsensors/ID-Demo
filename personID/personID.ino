/*--- NOTES ---
*/

/*--- INCLUDE LIBRARIES ---*/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>
#include "LEDDriver.h"
#include "buzzDriver.h"
#include "I2CDriver.h"

/*--- INPUT CONSTANTS ---*/
#define BAUD_RATE 9600
#define SPI_CLK_RATE 16000000
#define LED_WRITE_RATE 1

/*--- PIN DEFINITIONS ---*/
#define sclk 13
#define mosi 11
#define cs   10
#define rst  9
#define dc   8
#define LED_SDI_PIN 17
#define LED_CLK_PIN 21
#define LED_LE_PIN 20

#define IO_PIN 12
#define B0_PIN 7
#define B1_PIN 5
#define B2_PIN 4
#define B3_PIN 3
#define B4_PIN 2
#define B5_PIN 14
#define B6_PIN 15
#define B7_PIN 16

/*--- COLOR DEFINITIONS ---*/
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

/*--- OTHER CONSTANTS ---*/
#define ID_N 8
#define NO_ID -1
#define DEBOUNCE_DELAY 50
#define REFRESH_PERIOD 100

/*--- TEST VARIABLES ---*/

/*--- FUNCTION PROTOTYES ---*/
static void initMessage(void);
static void displayID(int,int,int,int,int,int,int);
static void displayCalibration(int,int,int,int,int,int);
static void displayIDLED(int);
static void initScreen();
static void ioISR();
static void b0ISR();
static void b1ISR();
static void b2ISR();
static void b3ISR();
static void b4ISR();
static void b5ISR();
static void b6ISR();
static void b7ISR();

/*--- CLASS INSTANCES ---*/
Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, cs, dc, rst);
LEDDriver ledDriver = LEDDriver(LED_SDI_PIN,LED_CLK_PIN,LED_LE_PIN,LED_WRITE_RATE);
buzzDriver buzzer = buzzDriver();
I2CDriver i2c = I2CDriver();

/*--- MODULE VARIABLES ---*/
static int ledBits[LED_N] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
volatile int8_t calID = NO_ID;
volatile int8_t dataFlag = 0;
static long demoTimestamp;
static inference_results_t results;

/*--- STATE MACHINES ---*/
typedef enum{
  DEMO_CONTINUOUS, ID_CAL
}DemoStates_t;

typedef enum{
  DISPLAY_STANDBY, UPDATE_DISPLAY
}DisplayStates_t;

DemoStates_t DemoState;
DisplayStates_t DisplayState;

void setup() {
  pinMode(LED_SDI_PIN,OUTPUT);
  pinMode(LED_CLK_PIN,OUTPUT);
  pinMode(LED_LE_PIN,OUTPUT);
  Serial.begin(BAUD_RATE);
  Serial.println("init i2c");
  i2c.setMode(i2c.MODE_CONTINUOUS);
  i2c.setIdModelEnabled(true);
  i2c.setDebugMode(true);
  i2c.setPersistentIds(false);
  Serial.println("finish init i2c");
  display.begin(SPI_CLK_RATE);
  initMessage();
  delay(500);
  initScreen();
  DemoState = DEMO_CONTINUOUS;
  DisplayState = DISPLAY_STANDBY;
  attachInterrupt(digitalPinToInterrupt(IO_PIN), ioISR, RISING);
  attachInterrupt(digitalPinToInterrupt(B0_PIN), b0ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B1_PIN), b1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B2_PIN), b2ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B3_PIN), b3ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B4_PIN), b4ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B5_PIN), b5ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B6_PIN), b6ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B7_PIN), b7ISR, FALLING);
}

int getButtonState() {
  int button_state = digitalRead(B0_PIN) |
                    (digitalRead(B1_PIN) << 1) |
                    (digitalRead(B2_PIN) << 2) |
                    (digitalRead(B3_PIN) << 3) |
                    (digitalRead(B4_PIN) << 4) |
                    (digitalRead(B5_PIN) << 5) |
                    (digitalRead(B6_PIN) << 6) |
                    (digitalRead(B7_PIN) << 7);
  return button_state;
}

void loop() {
  buzzer.buzzDriverTick();

  results = i2c.read();
  DisplayState = UPDATE_DISPLAY;

  switch(DemoState){
    case DEMO_CONTINUOUS:{
      if(calID != NO_ID){
        //displayIDLED(calID);
        buzzer.buzz(100,200);
        DemoState = ID_CAL;
        demoTimestamp = millis();
      }
    }
    break;

    case ID_CAL:{
      if(millis() - demoTimestamp > DEBOUNCE_DELAY) {
        DemoState = DEMO_CONTINUOUS;
        Serial.println("calibrating id " + String(calID));
        i2c.calibrate(calID);
        // Only update to NO_ID if button is not still pressed.
        if ((getButtonState() >> calID) & 0x1) {
          calID = NO_ID;
        }
      }
    }
    break;
  }

  switch(DisplayState){
    case DISPLAY_STANDBY:{
      
    }
    break;

    case UPDATE_DISPLAY:{
      if(DemoState == DEMO_CONTINUOUS){
        displayID(results);
      }else if(DemoState == ID_CAL){
        
      }
      DisplayState = DISPLAY_STANDBY;
    }
    break;
  }
}

/*--- FUNCTION DEFINITIONS ---*/
static void initMessage(void){
  display.fillScreen(BLACK);
  const int FILL_DELAY = 10;
  const int BAR_Y = 25;
  const int BAR_H = 8;
  display.setCursor(0, 5);
  display.setTextColor(CYAN);
  display.setTextSize(1);
  display.println("ID Demo");
  display.setCursor(0, 15);
  display.println("Useful Sensors");
  display.drawRect(0,BAR_Y,display.width(),BAR_H,CYAN);
  for(int i = 1; i <= display.width(); i++){
     display.fillRect(0,BAR_Y,i,BAR_H,CYAN);
     delay(FILL_DELAY);
  }
}

static void displayID(inference_results_t results){
  static inference_results_t old_results = {};
  if (!memcmp(&old_results, &results, sizeof(inference_results_t))) {
    return;
  }
  display.fillScreen(BLACK);
  display.setTextSize(1);
  display.setTextColor(YELLOW);
  old_results = results;
  int id_bitmap = 0;
  for (int i=0; i< results.num_faces; i++) {
    int x1 = (results.boxes[i].data[0] * display.width()) / 256;
    int x2 = (results.boxes[i].data[2] * display.width()) / 256;
    int y1 = (results.boxes[i].data[1] * display.height()) / 256;
    int y2 = (results.boxes[i].data[3] * display.height()) / 256;
  
    int w = x2 - x1;
    int h = y2 - y1;
    uint8_t confidence = results.boxes[i].confidence;
    if(confidence > 99) confidence = 99;
    if (confidence > 42) {
      display.drawRect(x1,y1,w,h,CYAN);
      display.setCursor(x2 + 1, y1);
      display.println(String(confidence) + "%");
    }

    uint8_t id_confidence = results.boxes[i].id_confidence;
    int8_t id = results.boxes[i].id;
    if (id >= 0) {
      display.setCursor(x1, y2+10);
      display.println(String(results.boxes[i].id) + " (" + String(results.boxes[i].id_confidence) + "%)");
      id_bitmap |= (1 << results.boxes[i].id);
    }
    if (results.boxes[i].face_on) {
      display.drawRect(x1,y1,w,h,GREEN);
    }
  }
  display.setCursor(0, 0);
  display.println("# faces: " + String(results.num_faces));
  displayIDLED(id_bitmap);
}

static void initScreen(){
  display.fillScreen(BLACK);
  display.setTextSize(1);
  display.setTextColor(YELLOW);
  display.setCursor(0, display.height()-10);
  display.println("ID:--");
  display.setCursor(display.width()-20, display.height()-10);
  display.println("--%");
}

static void displayCalibration(int x1, int y1, int x2, int y2, int ID, int calIndex){
  int w = x2 - x1;
  int h = y2 - y1;
  display.fillScreen(BLACK);
  const int BAR_W = 50;
  const int BAR_H = 8;
  const int MAX_INDEX = 100;
  int progWidth = float(calIndex)/float(MAX_INDEX)*float(BAR_W);
  display.setTextColor(YELLOW);
  display.setTextSize(1);
  display.drawRect(x1,y1,w,h,CYAN);
  display.setCursor(0, display.height()-10);
  display.println("ID:" + String(ID));
  display.drawRect(display.width()-BAR_W,display.height()-10,BAR_W,BAR_H,YELLOW);
  display.fillRect(display.width()-BAR_W,display.height()-10,progWidth,BAR_H,YELLOW);
}

static void displayIDLED(int ID_bitmap){
  const int mapID[ID_N] = {3,2,1,0,12,13,14,15};
  for(int i=0; i<LED_N; i++){
    ledBits[i] = 0;
  }
  for (int i=0; i<ID_N; i++) {
    ledBits[mapID[i]] = (ID_bitmap >> i) & 0x1;
  }
  ledDriver.setOutputs(ledBits);
}

static void ioISR(){
  dataFlag = 1;
}

static void b0ISR(){
  calID = 0;
}

static void b1ISR(){
  calID = 1;
}

static void b2ISR(){
  calID = 2;
}

static void b3ISR(){
  calID = 3;
}

static void b4ISR(){
  calID = 4;
}

static void b5ISR(){
  calID = 5;
}

static void b6ISR(){
  calID = 6;
}

static void b7ISR(){
  calID = 7;
}
