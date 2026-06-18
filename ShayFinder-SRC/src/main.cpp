#include <Arduino.h>
#include <Adafruit_TinyUSB.h> 

// Button Pin definitions
const int sw1{D0};
const int sw2{D1};
const int sw3{D2};

const int red = sw1;
const int yel = sw2;
const int gre = sw3;

// Added LED Pin definitions
const int ledRed = D3; // Lights up for sw1 (Next Track)
const int ledYel = D4; // Lights up for sw2 (Play/Pause)
const int ledGre = D5; // Lights up for sw3 (Previous Track)

// Create the USB HID instance
Adafruit_USBD_HID usb_hid;

// Media keys require a "Consumer Control" configuration
#define RID_CONSUMER_CONTROL 3
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL))
};

// Modified function to accept the LED pin
void buttonCheck(int pin, uint16_t key, int ledPin) {
  if (digitalRead(pin) == LOW) { 
    // 1. Turn ON the LED
    digitalWrite(ledPin, HIGH);

    // 2. Send the media key press
    usb_hid.sendReport(RID_CONSUMER_CONTROL, &key, sizeof(key));
    
    // 3. Hold here until you let go of the physical button
    while (digitalRead(pin) == LOW) {
      delay(10); 
    }
    
    // 4. Send a blank report (0) so the computer knows the key was released
    uint16_t release_key = 0;
    usb_hid.sendReport(RID_CONSUMER_CONTROL, &release_key, sizeof(release_key));

    // 5. Turn OFF the LED when released
    digitalWrite(ledPin, LOW);
  }
}

void setup() {
  // Set up the custom HID descriptor for actions
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  // Configure Button pins
  pinMode(red, INPUT_PULLUP);
  pinMode(yel, INPUT_PULLUP);
  pinMode(gre, INPUT_PULLUP);
  
  // Configure LED pins
  pinMode(ledRed, OUTPUT);
  pinMode(ledYel, OUTPUT);
  pinMode(ledGre, OUTPUT);

  // Ensure all LEDs start OFF
  digitalWrite(ledRed, LOW);
  digitalWrite(ledYel, LOW);
  digitalWrite(ledGre, LOW);
  
  // Wait until the computer recognizes the USB device
  while (!TinyUSBDevice.mounted()) {
    delay(1);
  }
}

void loop() {
  // Check buttons and pass their corresponding LED pins
  buttonCheck(red, HID_USAGE_CONSUMER_SCAN_NEXT_TRACK, ledRed);
  buttonCheck(yel, HID_USAGE_CONSUMER_PLAY_PAUSE, ledYel);
  buttonCheck(gre, HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK, ledGre);
}