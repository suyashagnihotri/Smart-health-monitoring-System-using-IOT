#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 4, 9, 10, 11, 12);

#define REPORTING_PERIOD_MS     3000

PulseOximeter pox;

uint32_t tsLastReport = 0;

int temp_adc_val;
float temp_val;
int t = 0;
int pulse =0;
int spo2 =0 ;
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    //Serial.println("Beat!");
    digitalWrite(13,!digitalRead(13));
}

void setup(){
    delay(500);
    Serial.begin(9600);
    pinMode(13,OUTPUT);
    lcd.begin(16,2);
    lcd.home();
    lcd.print("P:");
    lcd.setCursor(9,0);
    lcd.print("Sp:");
    lcd.setCursor(0,1);
    lcd.print("T:");

    Serial.print("Initializing pulse oximeter..");

    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
     pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
    // Make sure to call update as fast as possible
    pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        //Serial.print("#,");
        pulse = pox.getHeartRate();
        Serial.print(pulse);
        Serial.print(",");
        spo2 = pox.getSpO2();
        Serial.print(spo2);
        Serial.print(",");
        
        temp_adc_val = analogRead(A3);  /* Read Temperature */
        temp_val = (temp_adc_val * 4.88); /* Convert adc value to equivalent voltage */
        t = (temp_val/10); /* LM35 gives output of 10mv/°C */
        Serial.print(t);
        Serial.println(",");
        lcd.setCursor(2,0);
        lcd.print(pulse);
        lcd.print(" ");
        lcd.setCursor(12,0);
        lcd.print(spo2);
        lcd.print("  ");
        lcd.setCursor(2,1);
        lcd.print(t);

        tsLastReport = millis();
    }
}
