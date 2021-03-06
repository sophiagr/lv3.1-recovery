/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This example demonstrates basic scale output. See the calibration sketch to get the calibration_factor for your
 specific load cell setup.

 This example code uses bogde's excellent library: "https://github.com/bogde/HX711"
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE

 The HX711 does one thing well: read load cells. The breakout board is compatible with any wheat-stone bridge
 based load cell which should allow a user to measure everything from a few grams to tens of tons.
 Arduino pin 2 -> HX711 CLK
 3 -> DAT
 5V -> VCC
 GND -> GND

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/

// include the HX711 library:
  #include "HX711.h"

// define variables for the force sensor
  #define calibration_factor 31300 //This value is obtained using the SparkFun_HX711_Calibration sketch
  #define DOUT  3
  #define CLK  2

  HX711 scale;
 
// include the SD library:
  #include <SPI.h>
  #include <SD.h>

// set up variables using the SD utility library functions:
  Sd2Card card;
  SdVolume volume;
  SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4?
// Adafruit SD shields and modules: pin 10?
// Sparkfun SD shield: pin 8?
  const int chipSelect = 10;

File myFile;


int dataset = 0; // this way when the user triggers data collection in the serial monitor is begins writing at 1
boolean pause = false;


//////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  // Open serial communications and wait for port to open:
  while (!Serial) {
  ; // wait for serial port to connect. Needed for native USB port only
  }
  
//////////////////////////////////////////////////////////////////////////////////////////////////
// FORCE SENSOR SET UP //
//////////////////////////////////////////////////////////////////////////////////////////////////

  Serial.println("HX711 scale demo");

  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

////////////////////////////////////////////////////////////////////////////////////
// SD CARD SET UP //
////////////////////////////////////////////////////////////////////////////////////

 /*
 The circuit:
  * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - pin 10; depends on your SD card shield or module.


 created  28 Mar 2011
 by Limor Fried
 modified 9 Apr 2012
 by Tom Igoe
 */

Serial.print("\nInitializing SD card...");

// initialization code from the utility libraries since we're just testing if the card is working
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

// print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

// Now try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }
  

// print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();
  
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  
  myFile = SD.open("dev2.txt", FILE_WRITE);
  myFile.println("Starting testing");
  myFile.close();

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
// list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

}


/////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  //Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
  //Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
  //Serial.println();


  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  Serial.print("opening file");
  myFile = SD.open("dev2.txt", FILE_WRITE);
  Serial.print("file open");

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to dev2.txt...");//leting you know its started
    myFile.println("printing data 1, 2, 3");//logging to the SD card
    // close the file:
    myFile.close();
    Serial.println("done.");//letting you know that it's logged b/c you can't see it logging
    myFile = SD.open("dev2.txt");
    Serial.println("dev2.txt:");//printing what is on the SD card
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    // pause the monitor
    pause = true;
    while(pause == true){ 
        if (Serial.read() == 'n'){ //"n" for next test
          pause = false;
          Serial.write("unpaused.");
          //else pause = true;
        }
    }
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
    pause = true;
    while(pause == true){ 
        if (Serial.read() == 'n'){ //"n" for next test
          pause = false;
          Serial.write("unpaused.");
          //else pause = true;
        }
  }
  }
}

