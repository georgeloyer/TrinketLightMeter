/*
 Trinket Light Meter sketch 
 Trinket mini microcontroller
 Adafruit i2c/SPI LCD backpack using MCP23008 I2C expander, I2C addr set to 1
 Adafruit TCS34725 breakout

 This sketch turns the white LED on the TCS34725 OFF and then returns measurements in Lux and Correlated Color Temperature
 once per second

 * 5V to Trinket 5V pin
 * GND to Trinket GND pin
 * SCL or CLK to Trinket pin GPIO #2
 * SDA or DAT to Trinket pin GPIO #0
 * TCS34725 INT connected to LED to control LED with setinterrupt()
*/

// include the library code:
#include <TinyWireM.h>
#include <TinyLiquidCrystal.h>
#include <TinyTCS34725.h>

// Connect to LCD via i2c, address #1 (A0 jumpered)
TinyLiquidCrystal lcd(1);

/* Initialise TCS34725 sensor with specific integration time and gain values */
TinyTCS34725 tcs = TinyTCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup() {
  // set up the LCD
  lcd.begin(16, 2);            // our display has 16 cols, 2 rows
  lcd.setBacklight(HIGH);      // Turn on the backlight
  // lcd.print("RGB Light Meter Online");  // Print a message to the LCD.
  if (tcs.begin()) {
    // lcd.print("Found RGB Sensor");
  } else {
    lcd.print("No RGB sensor found ... check your connections");
    while(1);
  }
  // Now we're ready to get readings!!
  tcs.setInterrupt(1); //turn off the LED
}

void loop() {
  uint16_t r, g, b, c;  // 16 bit ints for the tcs 
  long x, y, z, xx, yy, n, cct;  // 32 bit data for the calculations, avoiding floating point
  tcs.getRawData(&r, &g, &b, &c);  

  // calculate the color temperature and illuminance (e)
  // reference: ams.com/kor/content/view/download/145158
  // scaling by 1,000 to avoid floating point math
  
  // step 2 calculate the locus of the readings in Plankian space
  x = 1000L*r/(-7002L) + 1000L*g/645L + 1000L*b/(-1046L);
  y = 1000L*r/(-3080L) + 1000L*g/634L + 1000L*b/(-1366L);
  z = 1000L*r/(-1466L) + 1000L*g/1297L + 1000L*b/1775L;
  // illuminance is the y value of the Plankian space locus

  // step 3 calculate the chromaticicy coordinates, scaled by 1000
  xx = 1000L*x/(x + y + z);
  yy = 1000L*y/(x + y + z);

  //step 4 calculate Correlated Color Temperature (CCT)
  n = (xx-332L)*1000L/(186L-y);
  cct = 449L*n*n*n/1000000000L + 3525L*n*n/1000000L + 6823L*n/1000L +5520L;
  
    
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0,0);
  lcd.print("Lux: ");
  lcd.setCursor(10,0);
  lcd.print(y);
  lcd.setCursor(0, 1);
  lcd.print("CCT: ");
  lcd.setCursor(10,1);
  lcd.print(cct);
  
}
