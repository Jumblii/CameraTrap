#include <Wire.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "esp_camera.h"
#include "camera_pins.h"
//custom pins for SD card
#define SCK  14
#define MISO  33
#define MOSI  32
#define CS  13

#define CAMERA_MODEL_WROVER_KIT

SPIClass spi = SPIClass(VSPI);

 //create struct for Motion sensor to easily assign GPIO number,
 // number of Detections and detected later
struct MotionSensor
{
  const uint8_t PIN;
  bool detected;
};

//assigning GPIO for LED
const int ledPin = 2;

MotionSensor motionSensor = {15, false};
//Button button = {15, false};

int photoNo;
String path;

//creating variables to check time
// between interrupts so multiple interrupts won't happen at the same time
unsigned long detTime = 0; 
unsigned long lastDetTime =0;

//creating method for interrupt (isr according to Esp32 documentation)
void IRAM_ATTR interMotion() 
{
  detTime = millis();
  if(detTime-lastDetTime>5000) //new interrupt will occur only if 250 milliseconds have passed since last interrupt
  {
    motionSensor.detected = true;
    lastDetTime = detTime;
  }
}

void setup()
{ 
  //starting serial conversation to print out number of detections to serial monitor(debugging)
  Serial.begin(115200);
  
  //initializing pin modes and attaching interrupt to motionSensor pin
  pinMode(motionSensor.PIN, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(motionSensor.PIN, interMotion, RISING);

  //starting connection to SD card and printing out error if it fails
  spi.begin(SCK, MISO, MOSI, CS);

  if (!SD.begin(CS,spi,8000000)) {
    Serial.println("Card Mount Failed");
    return;
  }
  
  //creating a directory called photos on the SD card
  if(SD.mkdir("/photos"))
  {
    Serial.println("Directory created");
  } else
    {
      Serial.println("mkdir failed");
    }

  //creating a log file on the SD card
  File file = SD.open("/log.txt", FILE_WRITE);
  if(!file || file.println(""))
  {
    Serial.println("Failed");
  } else {Serial.println("Success");}
  file.close();

  //configuring the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 5;
  config.fb_count = 1;

  //initializing the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  } else {Serial.println("Camera Initialized");}

  photoNo = 0;
}

void loop()
{
  if(motionSensor.detected)
  {
    Serial.println("Motion has been detected");
    
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();  
    if(!fb) 
    {
      Serial.println("Camera Failed to Capture");
      return;
    } else 
      {
      photoNo += 1;  
      }
    
    path = "/photos/photo"+String(photoNo)+".jpg";
    
    Serial.printf("Picture file name: %s\n", path.c_str());
    File photo = SD.open(path.c_str(), FILE_WRITE);
    if(!photo)
      {
        Serial.printf("Failed to open file in write mode.");
      } else 
        {
          photo.write(fb->buf, fb->len);
          Serial.printf("Saved photo as: %s\n", path.c_str());
        }
    photo.close();
    //log the number of photos into a log.txt file
    File log = SD.open("/log.txt", FILE_WRITE);
    if(!log)
    {
      Serial.println("Failed to open log");
    }
    if(log.printf("Number of photos taken: %u\n", photoNo))
    {
      Serial.println("Logged the number of photos takes");
    } else {Serial.println("Logging failed");}
    log.close();
    esp_camera_fb_return(fb);

    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    motionSensor.detected = false;
  }
 
}