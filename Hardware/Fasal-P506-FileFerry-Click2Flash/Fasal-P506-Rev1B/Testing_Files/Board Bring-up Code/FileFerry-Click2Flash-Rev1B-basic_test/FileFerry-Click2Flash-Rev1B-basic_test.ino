/*
STM32 Arduino Core:

Follow the Guide: https://github.com/stm32duino/Arduino_Core_STM32/wiki/Getting-Started

Use Board as : Generic STM32F103RETx

*/
//                      RX    TX
HardwareSerial Serial1(PA10, PA9);
#define LED_BLUE PC10
#define LED_RED PC11
#define LED_GREEN PC12
#define userButton PA0
#define sensorPowerEnablePin PB8
int count = 0;
void setup()
{
  //afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);//needed to use since some pins of RGB led are system pins
  delay(1000);
  Serial1.begin(115200);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  pinMode(sensorPowerEnablePin, OUTPUT);
  digitalWrite(sensorPowerEnablePin, LOW);
  delay(1000);
  for (int i = 0; i < 10; i++)
  {
    //green
    digitalWrite(LED_RED, HIGH);
    delay(100);
    digitalWrite(LED_RED, LOW);
    delay(50);
  }
  delay(1000);
  Serial1.println("Fasal# says HI!");
  Serial1.println("Setup Start");
  pinMode(userButton, INPUT);
  attachInterrupt(digitalPinToInterrupt(userButton), userButtonInterrupt_function, FALLING);

  Serial1.println("setup end");
}

void loop()
{
  Serial1.println("*******************************************************");
  Serial1.println("loop start");
  digitalWrite(sensorPowerEnablePin, HIGH);
  // put your main code here, to run repeatedly:
  rgb_led_blink();
  Serial1.println("Count: " + String(count++));
  delay(1000);
  digitalWrite(sensorPowerEnablePin, LOW);
  delay(100);
  Serial1.println("EOL");
  Serial1.println("*******************************************************");
  delay(5000);
}


void rgb_led_blink() {
  //red
  digitalWrite(LED_RED, HIGH);
  Serial1.println("LED COLOR IS: RED");
  delay(1000);
  digitalWrite(LED_RED, LOW);
  delay(50);

  //green
  digitalWrite(LED_GREEN, HIGH);
  Serial1.println("LED COLOR IS: GREEN");
  delay(1000);
  digitalWrite(LED_GREEN, LOW);
  delay(50);

  //blue
  digitalWrite(LED_BLUE, HIGH);
  Serial1.println("LED COLOR IS: BLUE");
  delay(1000);
  digitalWrite(LED_BLUE, LOW);
  delay(50);

  //red + blue = pink
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  Serial1.println("LED COLOR IS: PINK");
  delay(1000);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, LOW);
  delay(50);

  //green + blue = emerald
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  Serial1.println("LED COLOR IS: TURQUOIS");
  delay(1000);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  delay(50);

  // red + green = yellow
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  Serial1.println("LED COLOR IS: YELLOW");
  delay(1000);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  delay(50);

  // red + green + blue = white
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  Serial1.println("LED COLOR IS: WHITE");
  delay(1000);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  delay(50);


  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
}


void userButtonInterrupt_function() {
  Serial1.println("\r\n******* User Button Triggered *********\r\n");
}
