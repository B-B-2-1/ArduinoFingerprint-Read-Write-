
#include <Adafruit_Fingerprint.h>

// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
// uncomment this line:
// #define mySerial Serial1

// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// comment these two lines if using hardware serial
#include <SoftwareSerial.h>

// include the library code:
#include <LiquidCrystal.h>

SoftwareSerial mySerial(2, 3);
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

int Lockpin=6;//pin controlling relay to lock

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup()  
{ pinMode(Lockpin,OUTPUT);
  digitalWrite(Lockpin,LOW);
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
    // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // set the data rate for the sensor serial port
  finger.begin(9600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
//  Serial.println("Waiting for valid finger...");
}
    
    
    
    //........................
    uint8_t downloadFingerprintTemplate(uint16_t id)
{
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #"); Serial.println(id);
  uint8_t p = finger.loadModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template "); Serial.print(id); Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    default:
      Serial.print("Unknown error "); Serial.println(p);
      return p;
  }

  // OK success!

  Serial.print("Attempting to get #"); Serial.println(id);
  p = finger.getModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template "); Serial.print(id); Serial.println(" transferring:");
      break;
   default:
      Serial.print("Unknown error "); Serial.println(p);
      return p;
  }
  
  // one data packet is 267 bytes. in one data packet, 11 bytes are 'usesless' :D
//  uint8_t bytesReceived[534]; // 2 data packets
//  memset(bytesReceived, 0xff, 534);
//
//  uint32_t starttime = millis();
//  int i = 0;
//  Serial.println("20sec count started");
//  while (i < 534 && (millis() - starttime) < 20000) {
//    Serial.println(i);
//      if (mySerial.available()) {
//          bytesReceived[i++] = mySerial.read();
//      }
//  }
//  Serial.print(i); Serial.println(" bytes read.");
//  Serial.println("Decoding packet...");
//
//  uint8_t fingerTemplate[512]; // the real template
//  memset(fingerTemplate, 0xff, 512);
//
//  // filtering only the data packets
//  int uindx = 9, index = 0;
//  while (index < 534) {
//      while (index < uindx) ++index;
//      uindx += 256;
//      while (index < uindx) {
//          fingerTemplate[index++] = bytesReceived[index];
//      }
//      uindx += 2;
//      while (index < uindx) ++index;
//      uindx = index + 9;
//  }
//  for (int i = 0; i < 512; ++i) {
//      //Serial.print("0x");
//      printHex(fingerTemplate[i], 2);
//      //Serial.print(", ");
//  }
//  Serial.println("\ndone.");
/////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  uint8_t templateBuffer[256];
  memset(templateBuffer, 0xff, 256);  //zero out template buffer
  int index=0;
  uint32_t starttime = millis();
  while ((index < 256) && ((millis() - starttime) < 1000))
  {
    if (mySerial.available())
    {
      templateBuffer[index] = mySerial.read();
      index++;
    }
  }
  
  Serial.print(index); Serial.println(" bytes read");
  
  //dump entire templateBuffer.  This prints out 16 lines of 16 bytes
  for (int count= 0; count < 16; count++)
  {
    for (int i = 0; i < 16; i++)
    {
      Serial.print("0x");
      Serial.print(templateBuffer[count*16+i], HEX);
      Serial.print(", ");
    }
    Serial.println();
  }
}



void printHex(int num, int precision) {
    char tmp[16];
    char format[128];
 
    sprintf(format, "%%.%dX", precision);
 
    sprintf(tmp, format, num);
    Serial.print(tmp);
}
    //........................




uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop()                     // run over and over again
{  
  if(! Serial.available())
  {
  Serial.println("Waiting for valid finger...");
  while(!Serial.available()){
    lcd.setCursor(0, 0);
    lcd.print("Place your");lcd.setCursor(0, 1);lcd.print("  Right Thumb");
    getFingerprintID();
    delay(50);  //don't ned to run this at full speed.
  }
  }
  String command=Serial.readString();
                            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>ENROLL DELETE SHOW-TEMPLATE<<<<<<<<<<<<<<<
  if(command=="enroll")
  {
  lcd.clear();lcd.setCursor(0,0);lcd.print("Enrolling begun");delay(500);
  Serial.println("backclose");delay(1000);//////////////////////////////////??????????????????????????????
  lcd.clear();lcd.setCursor(0,0);lcd.print("Entering details");delay(500);
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
  
  
  else if(command=="delete"){
  Serial.println("backclose2");delay(1000);//////////////////////////////////??????????????????????????????
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
  
  //-----------------------------------------------------------------------------------------------------------
  
  
  else if(command=="tmp"){
    Serial.println("Fingerprint template extractor");
    delay(1000);
    Serial.println("Enter the ID of the template you want to display");
    while(!Serial.available());
    int finger=readnumber();
    downloadFingerprintTemplate(finger);

  }
}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>><<<<<<><><


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready to enroll");delay(2000);
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
      Serial.println("Unknown error");
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
//    lcd.clear();lcd.setCursor(0,0);lcd.print("Fingerprint      enrolled");delay(5000);lcd.clear();
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
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<>>
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
    lcd.clear();//////////////////////////////////?
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
  lcd.print("Match found");
  digitalWrite(Lockpin,HIGH);                                            //Opening the relay to Lock
  delay(1000);lcd.clear();lcd.setCursor(0,0);
  lcd.print("Door Unlocked");
  delay(3000);digitalWrite(Lockpin,LOW);                                 //Relocking the door
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 

  return finger.fingerID;
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>><<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
//  Serial.println("Enter Password");
//  while(!Serial.available());
  
//  if(Serial.readString()=="password"){
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
//  }else{Serial.println("Wrong Password");}  
}
//>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

