#include <Arduino.h>
#include <LiquidCrystal.h>
//#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

// RFID pins
#define SS_PIN 53
#define RST_PIN 5
#define RELAY A0   // relay pin
#define BUZZER A1  // buzzer pin
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

// Define the authorized UIDs
String authorizedUIDs[] = { "6C 31 B7 2B", "D9 4A CC AD", "43 95 77 92" };

// Create Servo objects
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

LiquidCrystal lcd(32, 30, 28, 26, 24, 22);  // Example LCD pins on Arduino Mega
//LiquidCrystal_I2C lcd(0x27, 20, 4);
// Define the servo pins
const int servoPin1 = 3;    // servo1
const int servoPin2 = 2;    // servo2
const int servoPin3 = 12;   // servo3
const int servoPin4 = 4;    // servo4
const int buttonPin = 7;    // start button
const int buttonAPin = 11;  // Juice button
const int buttonBPin = 10;  // Candy button
const int buttonCPin = 9;   // Jelly button
const int buttonDPin = 8;   // Chips button
const int coinPin = 6;      // Coin acceptor

// Track money collected and product quantities
int totalMoneyCollected = 0;
int cakeCount = 0;
int jellyCount = 0;
int biscuitCount = 0;
int chocoCount = 0;

void setup() {
  // Attach the servo objects to the specified pins
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonAPin, INPUT_PULLUP);
  pinMode(buttonBPin, INPUT_PULLUP);
  pinMode(buttonCPin, INPUT_PULLUP);
  pinMode(buttonDPin, INPUT_PULLUP);
  pinMode(coinPin, INPUT_PULLUP);
   // Initialize the LCD
  lcd.begin(20, 4);
  
  // Turn on the backlight
  //lcd.backlight();
  Serial.begin(9600);  // Initiate a serial communication
  SPI.begin();         // Initiate SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  digitalWrite(RELAY, LOW);
  Serial.println("Put your card to the reader...");
  Serial.println();

  // LCD start Message
  lcd.begin(20, 4);
  lcd.print("|    WELCOME TO    |");
  lcd.setCursor(0, 1);
  lcd.print("|  VENDING MACHINE |");
  lcd.setCursor(0, 2);
  lcd.print("| PRESS ANY BUTTON |");
  lcd.setCursor(0, 3);
  lcd.print("|    TO START      |");

  // Wait for button press or owner mode activation
  while (digitalRead(buttonPin) == HIGH);  // Button state

  // Display the product menu
  displayMenu();
}

void loop() {
  if (digitalRead(buttonAPin) == LOW) {
    processSelection("Cake", servo1, 2);
  } else if (digitalRead(buttonBPin) == LOW) {
    processSelection("Jelly", servo2, 3);
  } else if (digitalRead(buttonCPin) == LOW) {
    processSelection("Oreo", servo3, 1);
  } else if (digitalRead(buttonDPin) == LOW) {
    processSelection("Choco", servo4, 5);
  } else if (digitalRead(buttonPin) == LOW) {
    unsigned long startTime = millis();
    while (digitalRead(buttonPin) == LOW) {
      if (millis() - startTime >= 1000) {
        activateOwnerMode();
        break;
      }
    }
    delay(400);  // Simple debounce delay
  }
}

void displayMenu() {
  lcd.clear();
  lcd.print("Select Your Product:");
  lcd.setCursor(0, 1);
  lcd.print("1.Cake  | 2.Jelly");
  lcd.setCursor(0, 2);
  lcd.print("3.Oreo  | 4.Choco");
  lcd.setCursor(0, 3);
  lcd.print("====Using Button====");
}

void processSelection(const char *product, Servo &servo, int price) {
  int coinCount = 0;

  lcd.clear();
  lcd.print("Insert ");
  lcd.print(price);
  lcd.print(" EGP Coins");
  lcd.setCursor(0, 1);
  lcd.print("For: ");
  lcd.print(product);
  lcd.setCursor(0, 2);
  lcd.print("Paid Coins:");

  while (coinCount < price) {
    if (digitalRead(coinPin) == LOW) {
      coinCount++;
      delay(450);  // Debounce delay for coin insertion
      while (digitalRead(coinPin) == LOW);  // Wait for the coin signal to go high again
      lcd.setCursor(13, 2);
      lcd.print(coinCount);
    }
  }

  lcd.setCursor(0, 3);
  lcd.print("Paid Successfully..");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Pick Up Your ");
  lcd.print(product);
  rotateServo(servo);

  // Update total money collected and product count
  totalMoneyCollected += price;
  if (strcmp(product, "Cake") == 0) {
    cakeCount++;
  } else if (strcmp(product, "Jelly") == 0) {
    jellyCount++;
  } else if (strcmp(product, "Oreo") == 0) {
    biscuitCount++;
  } else if (strcmp(product, "Choco") == 0) {
    chocoCount++;
  }

  // Display a thank you message
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("Thank you!");
  delay(2000);  // Display the thank you message for 2 seconds

  // Redisplay the product menu
  displayMenu();
}

void rotateServo(Servo &servo) {
  servo.write(0);  // Rotate to 180 degrees
  delay(850);        // Wait for the servo to reach the position
  servo.write(90);    // Rotate back to 0 degrees
}

void activateOwnerMode() {
    lcd.clear();
    lcd.print("Owner Mode Activated");
    lcd.setCursor(0, 1);
    lcd.print("Total Products Sold: ");
    lcd.setCursor(0, 2);
    lcd.print("Cake: ");
    lcd.print(cakeCount);
    lcd.setCursor(10, 2);
    lcd.print("Jelly: ");
    lcd.print(jellyCount);
    lcd.setCursor(0, 3);
    lcd.print("Oreo: ");
    lcd.print(biscuitCount);
    lcd.setCursor(10, 3);
    lcd.print("Choco: ");
    lcd.print(chocoCount);
    delay(4000);
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Total Money: ");
    lcd.print(totalMoneyCollected);
    lcd.setCursor(16,1);
    lcd.print("EGP ");
    delay(4000);
    lcd.clear();
    lcd.print("Owner Mode: ");
    lcd.setCursor(0, 1);
    lcd.print("Insert Your Card ==>");
    lcd.setCursor(0, 2);
    lcd.print("Sensor Placed Right ");
    delay(4000);  // Wait for card insertion
    displayMenu();

    
/*  } else {
    lcd.clear();
    lcd.print("Unauthorized Card!");
    delay(2000);
    displayMenu();
  }*/
}

bool checkCardAuthorization() {
  // Check if a card is present
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Read the UID of the card
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  // Check if the UID is authorized
  for (String authorizedUID : authorizedUIDs) {
    if (uid == authorizedUID) {
      return true;
    }
  }
  return false;
}