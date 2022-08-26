/*--- NOTES ---
*/

/*--- INCLUDE LIBRARIES ---*/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include "LEDDriver.h"
#include "buzzDriver.h"

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

/*--- TEST VARIABLES ---*/

/*--- FUNCTION PROTOTYES ---*/
static void initMessage(void);
static void displayID(int,int,int,int,int,int);
static void displayCalibration(int,int,int,int,int,int);
static void displayIDLED(int);
static void initScreen();
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

/*--- MODULE VARIABLES ---*/
static int ledBits[LED_N] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
volatile int8_t calID = NO_ID;

/*--- STATE MACHINES ---*/
typedef enum{
  STANDBY, WAIT, ID_CAL, TRACK_FACE
}DemoStates_t;

DemoStates_t DemoState;

void setup() {
  pinMode(LED_SDI_PIN,OUTPUT);
  pinMode(LED_CLK_PIN,OUTPUT);
  pinMode(LED_LE_PIN,OUTPUT);
  Serial.begin(BAUD_RATE);
  display.begin(SPI_CLK_RATE);
  initMessage();
  delay(500);
  initScreen();
  DemoState = STANDBY;
  attachInterrupt(digitalPinToInterrupt(B0_PIN), b0ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B1_PIN), b1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B2_PIN), b2ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B3_PIN), b3ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B4_PIN), b4ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B5_PIN), b5ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B6_PIN), b6ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(B7_PIN), b7ISR, FALLING);
}

void loop() {
  ledDriver.ledDriverTick();
  buzzer.buzzDriverTick();

  switch(DemoState){
    case STANDBY:{
      if(calID != NO_ID){
        displayIDLED(calID);
        buzzer.buzz(250,255);
        DemoState = ID_CAL;
      }
    }
    break;

    case WAIT:{
      
    }
    break;

    case ID_CAL:{
      calID = NO_ID;
      DemoState = STANDBY;
    }
    break;

    case TRACK_FACE:{
      
    }
    break;
  }
}

/*--- FUNCTION DEFINITIONS ---*/
static void initMessage(void){
  display.fillScreen(BLACK);
  const int FILL_DELAY = 15;
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

static void displayID(int x, int y, int w, int h, int ID, int confidence){
  display.fillScreen(BLACK);
  display.setTextSize(1);
  display.setTextColor(YELLOW);
  if(confidence > 99) confidence = 99;
  display.drawRect(x,y,w,h,CYAN);
  display.setCursor(0, display.height()-10);
  display.println("ID:" + String(ID));
  display.setCursor(display.width()-20, display.height()-10);
  display.println(String(confidence) + "%");
}

static void initScreen(){
  display.fillScreen(BLACK);
  display.setTextSize(1);
  display.setTextColor(YELLOW);
  display.setCursor(0, display.height()-10);
  display.println("ID:" + String(NO_ID));
  display.setCursor(display.width()-20, display.height()-10);
  display.println(String(99) + "%");
}

static void displayCalibration(int x, int y, int w, int h, int ID, int calIndex){
  display.fillScreen(BLACK);
  
  const int BAR_W = 50;
  const int BAR_H = 8;
  const int MAX_INDEX = 100;
  int progWidth = float(calIndex)/float(MAX_INDEX)*float(BAR_W);
  
  display.setTextColor(YELLOW);
  display.setTextSize(1);
  display.drawRect(x,y,w,h,CYAN);
  display.setCursor(0, display.height()-10);
  display.println("ID:" + String(ID));
  display.drawRect(display.width()-BAR_W,display.height()-10,BAR_W,BAR_H,YELLOW);
  display.fillRect(display.width()-BAR_W,display.height()-10,progWidth,BAR_H,YELLOW);
}

static void displayIDLED(int ID){
  const int mapID[ID_N] = {0,1,2,3,12,13,14,15};
  for(int i=0; i<LED_N; i++){
    ledBits[i] = 0;
  }
  if(ID != -1) ledBits[mapID[ID]] = 1;
  ledDriver.setOutputs(ledBits);
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
