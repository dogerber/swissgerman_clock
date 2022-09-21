/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_ThinkInk.h"
#include "RTClib.h"

#define EPD_DC      10 // can be any pin, but required!
#define EPD_CS      9  // can be any pin, but required!
#define EPD_BUSY    -1  // can set to -1 to not use a pin (will wait a fixed delay)
#define SRAM_CS     6  // can set to -1 to not use a pin (uses a lot of RAM!)
#define EPD_RESET   -1  // can set to -1 and share with chip Reset (can't deep sleep)
#define COLOR1 EPD_BLACK
#define COLOR2 EPD_LIGHT
#define COLOR3 EPD_DARK

#define LEFT_MARGIN 15
#define LINE_HEIGHT 18

#define FONT_NAME FreeSans9pt7b
#define FONT_NAME2 FreeSansBold9pt7b

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>


// 2.9" Grayscale Featherwing or Breakout:
ThinkInk_290_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

boolean editMode = false;
const boolean everyfiveminute = false; // false = every 1 min refresh

unsigned long t_last_refresh = millis();
unsigned long t_interval = 60 * 1000; // [ms]


RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunntig", "Maentig", "Zistig", "Mittwuch", "Dunnstig", "Fritig", "Samstig"};

char minutesCH[12][20] = {"punkt", "foif ab", "zeh ab", "viertel ab", "zwaenzg ab", "foif-e-zwaenzg ab", "halbi", "foif ab halbi", "zwaenzg vor", "viertel vor", "zeh vor", "foif vor"};
//char minutesCHmin[60][20] = {"punkt", "eis", "zwei ", "drue ", "vier", "foif", "sechs", "siebe", "acht", "nuen", "zeh",
//                             "elf", "zwölf", "drizeh", "vierzeh", "fuefzeh", "sechzeh", "siebzeh", "achtzeh", "nuenzeh", "zwaenzg",
//                             "einezwaenzg", "zweiezwaenzg", "drueezwaenzg", "vierezwaenzg", "foifezwaenzg", "sechsezwaenzg", "siebenezwaenzg", "achtezwaenzg", "nuenezwaenzg", "drissg",
//                             "einedrissg", "zweiedrissg", "drueedrissg", "vieredrissg", "foifedrissg", "sechsedrissg", "siebenedrissg", "achtedrissg", "nuenedrissg", "vierzg",
//                             "einevierzg", "zweievierzg", "drueevierzg", "vierevierzg", "foifevierzg", "sechsevierzg", "siebenevierzg", "achtevierzg", "nuenevierzg", "fuefzg",
//                             "einefuefzg", "zweiefuefzg", "drueefuefzg", "vierefuefzg", "foifefuefzg", "sechsefuefzg", "siebenefuefzg", "achtefuefzg", "nuenefuefzg"
//                            };

char minutesCHmin[60][30] = {"punkt", "eis ab", "zwei ab", "drue ab", "vier ab", "foif ab", "sechs ab", "siebe ab", "acht ab", "nuen ab", "zeh ab",
                             "elf ab", "zwoelf ab", "drizeh ab", "vierzeh ab", "viertel ab", "sechzeh ab", "siebzeh ab", "achtzeh ab", "nuenzeh ab", "zwaenzg ab",
                             "einezwaenzg ab", "zweiezwaenzg ab", "drueezwaenzg ab", "vierezwaenzg ab", "foifezwaenzg ab", "sechsezwaenzg ab", "siebenezwaenzg ab", "achtezwaenzg ab", "nuenezwaenzg ab", "halbi",
                             "eis ab halbi", "zwei ab halbi", "drue ab halbi", "vier ab halbi", "foif ab halbi", "sechs ab halbi", "siebe ab halbi", "acht ab halbi", "nuen ab halbi", "zwaenzg vor",
                             "nuenzeh vor", "achtzeh vor", "siebzeh vor", "sechzeh vor", "viertel vor", "vierzeh vor", "drizeh vor", "zwoelf vor", "elf vor", "zeh vor",
                             "nuen vor", "acht vor", "siebe vor", "sechs vor", "foif vor", "vier vor", "drue vor", "zwei vor", "eis vor"
                            };
char hourCH[13][10] = {"Eis", "Zwei", "Drue", "Vieri", "Foifi", "Sechsi", "Siebni", "Achti", "Nueni", "Zehni", "Elfi", "Zwoelfi", "Eis"};
int minidx;
int houridx;

// Notes: A button is D11, B D12, C D13
#define PIN_BUTTON_A 11
#define PIN_BUTTON_B 12
#define PIN_BUTTON_C 13

void setup() {
  Serial.begin(115200);
  //  while (!Serial) { // wait for Serial to open
  //    delay(10);
  //  }


  // Initiate digital Pins
  pinMode(PIN_BUTTON_A, INPUT);
  pinMode(PIN_BUTTON_B, INPUT);
  pinMode(PIN_BUTTON_C, INPUT);




  Serial.println("Adafruit EPD full update test in mono & grayscale");

  display.begin(THINKINK_GRAYSCALE4);
  display.clearBuffer();
  display.setTextSize(1);
  display.setTextColor(EPD_BLACK, EPD_WHITE);
  display.setFont(&FONT_NAME);
  display.setCursor(10, 60);
  display.print("Fuer Tessa zum 29ste Geburtstag");
  display.display();
  delay(2000);



  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

 // if (! rtc.initialized() || rtc.lostPower()) {
 if (true) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //delay(2000);
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }


  DateTime now = rtc.now();

  printTime(now);
}




void loop() {


    DateTime now = rtc.now();


    //
    // if (millis() - t_last_refresh > t_interval) {
    if (everyfiveminute) { // display every 5 min
      if (now.minute() % 5 == 0 & millis() - t_last_refresh > t_interval) {
        // alternate if now.minute/5 has no remainder
        printTime(now);
      }
    }
    else { // display every minute
      printTime(now);
    }

    delay(60 * 1000); // if holding display and looping to many times screen gets wierd spots



}



// -----------------------------------------------------------------Subfunctions

void printTime(DateTime now ) {

  if (everyfiveminute) { // every 5 min
    minidx = floor(now.minute() / 5);
  }
  else { //every 1 minute
    minidx = now.minute();
  }


  if (now.minute() < 30) {
    houridx = now.twelveHour() - 1; // note that houridx = 0 is equal to "Eis"
  }
  else {
    houridx = now.twelveHour();
  }



  display.clearBuffer();

  // Date
  display.setCursor(60, 1 * LINE_HEIGHT);
  display.setTextSize(1);
  display.print(now.day(), DEC);
  display.print('.');
  display.print(now.month(), DEC);
  display.print('.');
  display.print(now.year(), DEC);
  display.print(" - ");
  display.println(daysOfTheWeek[now.dayOfTheWeek()]);

  // debug time
  if (false) {
    display.setCursor(LEFT_MARGIN, 2 * LINE_HEIGHT);
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(minidx);
    display.print(" ");
    display.println(houridx);
  }

  // es isch
  display.setTextSize(1);
  display.setCursor(LEFT_MARGIN, 3 * LINE_HEIGHT);
  display.println("Es isch ");

  // main
  display.setTextSize(1);
  display.setFont(&FONT_NAME2);
  display.setCursor(LEFT_MARGIN * 2, 4 * LINE_HEIGHT + 5);
  if (everyfiveminute){
  display.print(minutesCH[minidx]);
  }
  else{
    display.print(minutesCHmin[minidx]);
  }
  display.print(" ");
  display.print(hourCH[houridx]);
  display.setFont(&FONT_NAME);

  // gsi
  display.setTextSize(1);
  display.setCursor(LEFT_MARGIN, 5 * LINE_HEIGHT + 5);
  display.print("gsi.");
  display.println();

  display.display();
  t_last_refresh = millis();
}

//void displayTime() {
//  // display.clearBuffer();
//  display.setCursor(0, 0);
//  display.print(now.year(), DEC);
//  display.print('/');
//  display.print(now.month(), DEC);
//  display.print('/');
//  display.println(now.day(), DEC);
//  display.print(" (");
//  display.println(daysOfTheWeek[now.dayOfTheWeek()]);
//  display.print(") ");
//  display.print(now.hour(), DEC);
//  display.print(':');
//  display.print(now.minute(), DEC);
//  display.print(':');
//  display.print(now.second(), DEC);
//  display.println();
//  display.display();
//}



// example of grey levels
//  display.clearBuffer();
//  display.fillRect(display.width() / 4, 0, display.width() / 4, display.height(), EPD_LIGHT);
//  display.fillRect((display.width() * 2) / 4, 0, display.width() / 4, display.height(), EPD_DARK);
//  display.fillRect((display.width() * 3) / 4, 0, display.width() / 4, display.height(), EPD_BLACK);
//  display.display();
//  delay(2000);



//  // read buttons
//  if (!digitalRead(PIN_BUTTON_A)) {
//    editMode = !editMode;
//    Serial.println("A pressed");
//  }
//
//
//  if (!digitalRead(PIN_BUTTON_B)) {
//    Serial.println("B pressed");
//  }


//  if (editMode) {
//    Serial.println("Edit Mode");
//
//    if (!digitalRead(PIN_BUTTON_B)) {
//      DateTime now ( now + TimeSpan(1, 0, 0, 0));
//      Serial.println("Adding an hour");
//    }
//
//    //    if (digitalRead(PIN_BUTTON_A)) {
//    //        DateTime now ( now - TimeSpan(1, 0, 0, 0));
//    //        Serial.println("Subtracting an hour");
//    //    }
//
//    // show time in console
//    if (false) {
//      Serial.print(now.year(), DEC);
//      Serial.print('/');
//      Serial.print(now.month(), DEC);
//      Serial.print('/');
//      Serial.print(now.day(), DEC);
//
//      Serial.println(daysOfTheWeek[now.dayOfTheWeek()]);
//
//      Serial.print(now.hour(), DEC);
//      Serial.print(':');
//      Serial.print(now.minute(), DEC);
//      Serial.print(':');
//      Serial.print(now.second(), DEC);
//      Serial.println();
//    }
//
//    // change time on rtc
//    rtc.adjust(DateTime(now));
//
//  } // end edit Mode
