/*
ESP32 focuser, dew-heater for MKS DLC32
*/

// Pinout
#define dataPin  21
#define clockPin 16
#define latchPin 17
#define enPin    0
#define dirPin   2
#define stepPin  1
#define heat2Pin 3
#define interPin 4

byte data=0;  // State of EN, step, dir...)

// Etat moteur

#define VIT 2000

static volatile bool Depl=false;
static volatile int Distance;

hw_timer_t *Tmot = NULL;

void IRAM_ATTR motorRun() {
  if (Distance == 0) {
    timerAlarmDisable(Tmot);
    digitalWrite2(enPin, HIGH);
    Depl=false;
  }
  else {
      digitalWrite2(stepPin, HIGH);
      digitalWrite2(stepPin, LOW);
      Distance--;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Tmot=timerBegin(0,80,true);
  timerAttachInterrupt(Tmot, &motorRun, true);
  timerAlarmWrite(Tmot,VIT,true);
  //timerStop(Tmot);
  Serial.println("Setup");
}

bool Sens=true; 

void loop() {
  if (!Depl) {
    Serial.println("Debut");
    delay(2000);
    Serial.println("Apres 2s");
    Sens=!Sens;
    bouge(15000,Sens);
    Serial.println("Fin deplacement");
    delay(1000);
    Serial.println("Apres 1s");
    Serial.println("***");
  }
}

void bouge(int Dist, bool Sens) {
  //timerRestart(Tmot);
  timerAlarmEnable(Tmot);
  digitalWrite2(dirPin, (Sens)?HIGH:LOW); // Enable stepper
  Distance=Dist;
    digitalWrite2(enPin, LOW); // Enable stepper
    Depl=true;
}

void digitalWrite2(uint8_t PIN, uint8_t STATE) {
  bitWrite(data,PIN,STATE);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}