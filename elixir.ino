#include <LiquidCrystal.h>

const int analogInPin = 33; // Pin for pH sensor
const int pumpPin = 32; // Replace pumpPin with your actual pin number for controlling the pump
const int LED_BUILTIN = 2; // Built-in LED pin
const int SENSOR = 34; // Water flow sensor pin

LiquidCrystal lcd(19, 23, 18, 17, 16, 15); // Assuming your LCD is 16x2 and connected to these pins

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);
  pinMode(33, OUTPUT);
  lcd.begin(16, 2); // Assuming your LCD is 16x2
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Measure pH ");
  lcd.setCursor(0, 1);
  lcd.print("Value ....... ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO   ");
  lcd.setCursor(0, 1);
  lcd.print("OUR PROJECT");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ELIXIR  ");
  lcd.setCursor(0, 1);
  lcd.print("  !!!!!!!   ");
  delay(2000);
  lcd.clear();
}

void loop()
{
  // Water flow sensor part
  currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {

    pulse1Sec = pulseCount;
    pulseCount = 0;

    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    flowMilliLitres = (flowRate / 60) * 1000;

    totalMilliLitres += flowMilliLitres;

    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));
    Serial.print("L/min");
    Serial.print("\t");

    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    Serial.println("L");

    // pH sensor part
    int sensorValue = 0;
    unsigned long int avgValue = 0;
    int buf[10], temp = 0;

    for (int i = 0; i < 10; i++)
    {
      buf[i] = analogRead(analogInPin);
      delay(10);
    }

    for (int i = 0; i < 9; i++)
    {
      for (int j = i + 1; j < 10; j++)
      {
        if (buf[i] > buf[j])
        {
          temp = buf[i];
          buf[i] = buf[j];
          buf[j] = temp;
        }
      }
    }


    for (int i = 2; i < 8; i++)
      avgValue += buf[i];

    float pHVol = (float)avgValue * 5.0 / 1024 / 4.3;
    float phValue = -5.70 * pHVol + 22.8;
    phValue = 14.2 - phValue;

    Serial.print("sensor = ");
    Serial.println((phValue / 10) - 7);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("pH Value");
    lcd.setCursor(3, 1);
    lcd.print((phValue / 10) - 7);

    delay(900);
  }
}
