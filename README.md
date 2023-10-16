# CameraTrap
Internet of Things project for university - camera trap for capturing pictures of animals
The idea behind this project was to create a camera trap, to take pictures of animals in the Sefton Park or similar environment.
When a motion is detected in front of the camera trap, a photo is taken and saved to memory along with a log file. 
To create this camera, ESP32 Wrover Dev with a camera was used as the micro-controller. 
Infrared motion sensor was used to detect motion in front of the camera trap. 
As ESP32 Wrover Dev does not have its own memory to store the photos, they were stored on a microSD card using a microSD card adapter. 
Camera connected to the ESP32 Wrover Dev uses a large number of pins. We can see these pins in the diagram below, they are all underlined.
Because of this, the motion sensor and microSD card could only be connected to the rest of the pins. 
To connect a microSD card to an ESP32 micro-controller is usually done using the default VSPIO or HSPIO pins.
In the case of this project, those were taken up by the camera.
Luckily, the pins for interacting with the microSD card can be changed in the program. 
All the pins used are marked in the diagram below. 
Program for the camera trap was programmed using the Arduino IDE 
using the ESP32 libraries as well as libraries for communicating with SD card (SD.h, SPI.h, FS.h) and camera (esp_camera.h). 

![image](https://github.com/Jumblii/CameraTrap/assets/41615708/d34a4a7e-f6cd-45c6-87f7-713b0eb2e256)

The Arduino code can be viewed using Arduino IDE. ESP32 Wrover Dev Cam, micro SD card adapter and infrared sensor are needed to run the project on a device.
