/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>PROJECT BY [BIBIN] [ALEX JOHN] [DEEPAK] [ALEX JOSEPH] [FELIX]<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>THIS CODE IS SPECIFICALLY FOR ARDUINO NANO. IT MIGHT BE COMPATIBLE WITH UNO BUT THAT IS NOT TESTED<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>WE USE THE FINGERPRINT MODULE **** BY ************* WHIS IS CAPABLE OF STORING UPTO 250 FINGER TEMPLATES<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>THE DISPLAY USED IS A 16X2 LCD INTERFACED USING I2C<-->SERIAL CONVERTER WHICH USES ONLY 2 DATA LINES INSTEAD OF THE 12 REQUIRED FOR NORMAL LCD INTERFACING WITH ARDUINO<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<



#include <Adafruit_Fingerprint.h>

// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// comment these two lines if using hardware serial
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>// include the library code:
//Beginning of Auto generated function prototypes by Atmel Studio
uint8_t readnumber(void );
uint8_t getFingerprintEnroll();
uint8_t getFingerprintID();
uint8_t deleteFingerprint(uint8_t id);
//End of Auto generated function prototypes by Atmel Studio


SoftwareSerial mySerial(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);

//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address
String username;
int Lockpin=6;//pin controlling relay(MOSFET) to lock
int Ledpin=7;//pin controlling led

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.SETUP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//............................................................................................................................................................................................................................................................


void setup()  
{ 
  
  pinMode(Lockpin,OUTPUT);  //Setting MOSFET pin as output
  digitalWrite(Lockpin,LOW);//Default locked

  pinMode(Ledpin,OUTPUT);  //Setting MOSFET pin as output
  digitalWrite(Ledpin,LOW);
  
  Serial.begin(9600);//Begining serial communication with pc at 9600 Bauds
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  lcd.begin();
 //lcd.begin(16,2); // initialize the lcd for 16 chars 2 lines, turn on backlight

// ------- Quick 3 blinks of backlight -------------
 for(int i = 0; i< 3; i++)
 {
 lcd.backlight();
 delay(250);
 lcd.noBacklight();
 delay(250);
 }
 lcd.backlight(); // finish with backlight on 
;
  // set the data rate for the Fingerprint sensor serial port
  finger.begin(9600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Read Numbers (Serial inputs from pc and fingerprint sensor)<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//............................................................................................................................................................................................................................................................

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>MAIN LOOP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//............................................................................................................................................................................................................................................................


void loop()         
{  
  if(! Serial.available())//SEARCH
  {
  Serial.println("Waiting for valid finger...");
  while(!Serial.available()){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   Place Your");lcd.setCursor(0, 1);lcd.print("  Right  Thumb");
    getFingerprintID();
    delay(50);  //don't ned to run this at full speed.
  }
  }
  String command=Serial.readString();
  
  
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>ENROLL DELETE <<<<<<<<<<<<<<<
  
  
  if(command=="enroll")//ENROLL
  {
  lcd.clear();lcd.setCursor(0,0);lcd.print("Enrolling begun");delay(10);
  Serial.println("backclose");delay(100);//////////////////////////////////??????????????????????????????
  lcd.clear();lcd.setCursor(0,0);lcd.print("Entering details");delay(10);
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  Serial.println("typeid");
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  if(id==1313){return;}
  Serial.print("Enrolling ID #");
  Serial.println(id);
  
  while (!  getFingerprintEnroll() );
  }
  //--------------------------------------------------------------------------------------------------------------
  
  
  else if(command=="delete")//DELETE
  {
  Serial.println("backclose2");delay(1000);
  Serial.println("Please type in the ID # (from 1 to 127) you want to delete...");
  Serial.println("deleteid");

  uint8_t id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  } if(id==1313){return;}

  Serial.print("Deleting ID #");
  Serial.println(id);
  
  deleteFingerprint(id);
  }
    
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>ENROLL FUNCTION<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//............................................................................................................................................................................................................................................................

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready to enroll");delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place your");lcd.setCursor(0, 1);lcd.print("  Right Thumb");
  
  while (p != FINGERPRINT_OK) {
    if(Serial.available()){return p;}
    
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      Serial.println("errr");
        lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");Serial.println("errr");
      lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");Serial.println("errr");
      lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");Serial.println("errr");
      lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
    default:
      Serial.println("Unknown error");Serial.println("errr");
      lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
  }
  
  Serial.println("Remove finger");
  lcd.clear();lcd.setCursor(0,0);lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
    lcd.clear();lcd.setCursor(0,0);lcd.print("Place your right");lcd.setCursor(0,1);lcd.print(" thumb again...");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");Serial.println("errr");
      lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");Serial.println("errr");
      lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      break;
    default:
      Serial.println("Unknown error");Serial.println("errr");
      lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");Serial.println("errr");
     lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");Serial.println("errr");
     lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");Serial.println("errr");
     lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");Serial.println("errr");
      lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
    default:
      Serial.println("Unknown error");Serial.println("errr");
     lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");Serial.println("errr");
    lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");Serial.println("errr");
lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();    return p;
  } else {
    Serial.println("Unknown error");Serial.println("errr");
lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Success");delay(2000);lcd.clear();return true;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");Serial.println("errr");
    lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");Serial.println("errr");
    lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
  return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");Serial.println("errr");
    lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();    
    return p;
  } else {
    Serial.println("Unknown error");Serial.println("errr");
    lcd.clear();lcd.setCursor(0,0);lcd.print("Enroll Failed");lcd.setCursor(0, 1);lcd.print("  Try Again");delay(2000);lcd.clear();
    return p;
  }   
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>SEARCH for PLACED FINGER<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//............................................................................................................................................................................................................................................................


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" No Match found");
    delay(1000);lcd.clear(); 
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.println("Found ID #"); Serial.println(finger.fingerID);
  lcd.clear();
  lcd.setCursor(0,0);
  for(int i=0;i<10000&&!Serial.available();i++);
  username = Serial.readString();
  lcd.setCursor((16-(username.length()))/2,0);
  lcd.print(username);
  lcd.setCursor(0,1);
  lcd.print(" ACCESS GRANTED");
  digitalWrite(Lockpin,HIGH);  //Opening the relay to Lock
  digitalWrite(Ledpin,HIGH); //setting Led on
  delay(1000);lcd.clear();lcd.setCursor(0,0);
  lcd.print(" Door Unlocked");
  delay(1000);digitalWrite(Lockpin,LOW);digitalWrite(Ledpin,LOW); //Relocking the door and led
  lcd.clear();
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 

  return finger.fingerID;
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>DELETE A FIGERPRINT TEMPLATE by ID<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//............................................................................................................................................................................................................................................................


uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

      
          p = finger.deleteModel(id);

          if (p == FINGERPRINT_OK) {
            Serial.println("Deleted!");
          } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
            Serial.println("Communication error");
            return p;
          } else if (p == FINGERPRINT_BADLOCATION) {
            Serial.println("Could not delete in that location");
            return p;
          } else if (p == FINGERPRINT_FLASHERR) {
            Serial.println("Error writing to flash");
            return p;
          } else {
            Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
            return p;
          }
  
}
