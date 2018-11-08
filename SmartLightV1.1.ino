
// This is the code for the MakerLabs Bedside Lamp V1
// You need to wire a momentary push button to connect from ground to digital pin D2.  This means, when you
// push the button it make the led strip will change to a new pixel colour.  Note that you will need to press the
// button once to turn on the first colour. Pressing the button again will cycle through the colours you have listed below!


#include <Adafruit_NeoPixel.h>                                     // Includes the library that helps control the LED strip
#include <BlynkSimpleEsp8266.h>
#include <DNSServer.h>
#include <Ethernet.h>
#include <ESP8266WiFi.h>                                          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <WiFiManager.h>                                          // https://github.com/tzapu/WiFiManager

#define BLYNK_PRINT Serial                                       // Enables Serial Monitor if you want to look at whats going on when the device is plugged into a PC running the Arduino IDE

// You need to download the Blynk app on iOS or Android. You will need to make an app (scan the QR code on your project kit for access to the videos to help you
// Go to the Project Settings (nut icon)in your Blynk app, and copy the auth token into the line of code below. Don't delete the "" just change the blue text!
char auth[] = "AUTH TOKEN GOES IN HERE";
char blynk_token[] = "token_number";

#define BUTTON_PIN   D2                                           // Digital IO pin connected to the push button.  This will be
                                                                  // driven with a pull-up resistor so the switch should
                                                                  // pull the pin to ground momentarily. When the button is pressed, the D2 pin will go LOW, which will tell the device to move to the next colour.

#define PIXEL_PIN    D1                                            // Digital IO pin connected to the NeoPixels.
                                                                  // Wire the 5V LED lead to 5V on the board
                                                                  //Wire the GND LED lead to the G pin on the board (this lead shares the G pin with the push button)

#define PIXEL_COUNT 4                                            // The number of LEDs you have in the strip, you are given 4 to start with, but can buy more here: https://amzn.to/2sethw1




// The next bit describes the setup for your LED strip. Some LED strips work slightly differently...from the way the colour is controlled, to the frequency of the light. 

                                                                 //   Parameter 1 = number of pixels in strip,  neopixel stick has 8
                                                                 //   Parameter 2 = pin number (most are valid)
                                                                 //   Parameter 3 = pixel type flags, add together as needed:
                                                                 //   NEO_RGB     Pixels are wired for RGB bitstream
                                                                 //   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
                                                                 //   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
                                                                 //   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ400); //This is the correct setup for the LED strips bought from the web link above. if you find your colours look weird, try changing GRB to RGB.


bool oldState = HIGH;
int showType = 0;



BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 1) {                 // If value is 1 run this command
    digitalWrite(BUTTON_PIN, HIGH);          //D2 output from Wemos D1 mini
  }
  else {                                     // If value is 0 run this command
    digitalWrite(BUTTON_PIN, LOW);          //D2 output from Wemos D1 mini
  }
  Serial.print("V1 Button value is: ");
  Serial.println(pinValue);
}



void setup() {

  Serial.begin(9600); // Starts the serial communication
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  WiFiManagerParameter custom_blynk_token("Blynk", "blynk token", blynk_token, 34);
  wifiManager.addParameter(&custom_blynk_token);
  wifiManager.autoConnect("SmartLightV2");
  Serial.println(custom_blynk_token.getValue());
  Blynk.config(custom_blynk_token.getValue());

  pinMode(BUTTON_PIN, INPUT_PULLUP);                             // This line is telling the board how to setup the Push Button pin. 
  strip.begin();
  strip.show();                                                  // Set all pixels (LED's) to 'off'

}



void loop() {

 
  Blynk.run();                                                   // If you have set up your BLYNK account, and put in the details (at the top) this starts the connection to your Blynk app!

  bool newState = digitalRead(BUTTON_PIN);                       // Get current button state. Figuring out if the pin is HIGH or LOW

  
  if (newState == LOW && oldState == HIGH) {                     // Check if state changed from high to low (when the button is pressed).
    delay(20);                                                   // Short delay to make sure the button springs back to how it was before it was pressed - 'debounced'
    newState = digitalRead(BUTTON_PIN);                          // Check if button is still low after debounce.
    if (newState == LOW) {
      showType++;
      if (showType > 3)                                          // Here is where you will need to change the code/value to match the number of 'cases' you have
                                                                 // What this does is reset the cycle when you reach the end of the different colour combinations, so when you get to the end of the colours, the LED strip goes to 'case 0 - black/off' turning the LED off
        showType=0;
      startShow(showType);
    }
  }

 
  oldState = newState;                                           // Set the last button state back to the old state.
}

void startShow(int i) {                                          // This is the cool stuff! There is a counter. When you press the button it adds 1 to the value of the counter. This example starts with 4 'cases' as you press the button it will cycle through these...
  switch(i){                                                     // You can add more 'cases' by copying the code and creating a new case underneath. Make usre to copy from 'case 3:' to 'break;'...then change the case value to 4,5,6 etc.
                                                                 // We will need to change a bit of code above if you add more 'cases' but you will see that in the notes!

                                                                 // The colours are set by changing the GRB/RGB value. You can find an online colour calculator here: https://www.rapidtables.com/convert/color/hex-to-rgb.html (255 is HIGH and 0 is LOW)
                                                                
    case 0: colorWipe(strip.Color(0, 0, 0), 50);                 // Black/off
            break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);               // Red
            break;
    case 2: colorWipe(strip.Color(0, 255, 0), 50);               // Green
            break;
    case 3: colorWipe(strip.Color(0, 0, 255), 50);               // Blue
            break;
  }
}

// Fill the dots one after the other with a color               // This controls the way the LEDs are turned on, it gives a slightly nicer effect than simply on/off
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
