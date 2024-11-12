/* Description: This project uses a DS3231 to measure time and displays the time on a 1306 OLED display.
 * Further, it measures temperature with an analog temperature module and displays a mapped value to a 9g servo motor.
 */

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include <U8glib.h>

// Init constants
const int tempPin = A0;    // Temperature sensor connected to analog pin A0
const float R1 = 10000.0;  // 10K resistor value in ohms

// Steinhart-Hart coefficients for thermistor (adjust as needed for specific thermistor)
const float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// Init global variables

// Construct objects
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

void setup() {
  // Init communication
  Serial.begin(9600);
  // Init Hardware
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  u8g.setFont(u8g_font_unifont);
}

void loop() {
  oledWrite(30, 20, getTime(), 30, 40, String(getTemp()) + char(176) + "C");
  delay(200);
}

/*
 * This function reads time from a DS3231 module and packages the time as a String.
 * Parameters: Void
 * Returns: time in hh:mm:ss as String
 */
String getTime() {
  DateTime now = rtc.now();

  String newTime = String(now.hour());
  if (now.minute() < 10) {
    newTime = newTime + ":" + "0" + String(now.minute());
  } else {
    newTime = newTime + ":" + String(now.minute());
  }

  if (now.second() < 10) {
    newTime = newTime + ":" + "0" + String(now.second());
  } else {
    newTime = newTime + ":" + String(now.second());
  }

  return newTime;
}

/*
 * This function reads an analog pin connected to an analog temperature sensor and calculates the corresponding temperature in Celsius.
 * Parameters: Void
 * Returns: temperature as float
 */
float getTemp() {

  int Vo;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;


  Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  T = T - 273.15;                                              //convert Kelvin to Celcius

  return T;
}

/*
 * This function takes two strings and draws them to an OLED display at specified positions.
 * Parameters: - text: String to write to display
 * Returns: void
 */
void oledWrite(int x, int y, String text, int x2, int y2, String text2) {
  u8g.firstPage();
  do {
    u8g.drawStr(x, y, text.c_str());
    u8g.drawStr(x2, y2, text2.c_str());

    if (getTemp() > 27.0) {
      u8g.drawStr(x2, y2 + 20, "Varmt");
    } else {
      u8g.drawStr(x2, y2 + 20, "Kallt"); 
    }
  } while (u8g.nextPage());
}
