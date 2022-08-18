/*--- NOTES ---
*/


/*--- INCLUDE LIBRARIES ---*/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include "LEDDriver.h"

/*--- INPUT CONSTANTS ---*/
#define BAUD_RATE 9600
#define SPI_CLK_RATE 16000000

/*--- PIN DEFINITIONS ---*/
#define sclk 13
#define mosi 11
#define cs   10
#define rst  9
#define dc   8
#define LED_SDI_PIN 17
#define LED_CLK_PIN 21
#define LED_LE_PIN 20

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

/*--- FUNCTION PROTOTYES ---*/
static void initMessage(void);
static void displayID(int,int,int,int,int,int);
static void displayCalibration(int,int,int,int,int,int);
static void displayIDLED(int);

/*--- CLASS INSTANCES ---*/
Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, cs, dc, rst);
LEDDriver ledDriver = LEDDriver(LED_SDI_PIN,LED_CLK_PIN,LED_LE_PIN,1);


/*--- MODULE VARIABLES ---*/
static int ledBits[LED_N] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};

void setup() {
  pinMode(LED_SDI_PIN,OUTPUT);
  pinMode(LED_CLK_PIN,OUTPUT);
  pinMode(LED_LE_PIN,OUTPUT);
  Serial.begin(BAUD_RATE);
  display.begin(SPI_CLK_RATE);
  initMessage();
  delay(500);
  for(int i = 0; i < 10; i++){
    displayID(10+i,10+i,15,15,5,random(100));
    delay(200);
  }
  for(int i = 0; i < 10; i++){
    displayCalibration(display.width()/2-8 + random(-5,5),display.height()/2-8 + random(-5,5),15+random(0,5),15+random(0,5),5,(i+1)*10);
    delay(200);
  }
  ledDriver.setOutputs(ledBits);
  displayIDLED(0);
}

void loop() {
  ledDriver.driverTick();
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
  if(confidence > 99) confidence = 99;
  display.setTextSize(1);
  display.setTextColor(YELLOW);
  display.drawRect(x,y,w,h,CYAN);
  display.setCursor(0, display.height()-10);
  display.println("ID:" + String(ID));
  display.setCursor(display.width()-20, display.height()-10);
  display.println(String(confidence) + "%");
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
