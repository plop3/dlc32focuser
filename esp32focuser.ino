/*
ESP32 focuser, dew-heater for MKS DLC32
*/

/////////////////////////////////////////////////////
// Configuration //
#define VIT 2000        // µs/pas

// Pinout
#define dataPin  21
#define clockPin 16
#define latchPin 17
#define enPin    0
#define dirPin   2
#define stepPin  1
#define heat2Pin 3
#define interPin 4
/////////////////////////////////////////////////////
// Variables globales //
byte data=0;                               // Etat de  EN, step, dir... (74HC595)
unsigned int Position;                     // Position du focuser
bool Sens=true;                            // Sens de rotation
static volatile bool MotMove=false;        // Moteur en mouvement 
static volatile unsigned int MotDist;      // Distance à parcourir

hw_timer_t *Tmot = NULL;

/////////////////////////////////////////////////////
// Fonctions rapides //
void IRAM_ATTR motorRun() {
  if (MotDist == 0) {
    timerAlarmDisable(Tmot);
    digitalWrite2(enPin, HIGH);
    MotMove=false;
  }
  else {
      digitalWrite2(stepPin, HIGH);
      digitalWrite2(stepPin, LOW);
      MotDist--;
  }
}

/////////////////////////////////////////////////////
// Setup, Loop //
void setup() {
  Serial.begin(115200);
  // Initialisation 74HC595
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  // Timer moteur
  Tmot=timerBegin(0,80,true);
  timerAttachInterrupt(Tmot, &motorRun, true);
  timerAlarmWrite(Tmot,VIT,true);
}

void loop() {
  if (!MotMove) {
    delay(2000);
    Sens=!Sens;
    bouge(15000,Sens);
  }
}

/////////////////////////////////////////////////////
// Fonctions //
void bouge(unsigned int Dist, bool Sens) {
  //timerRestart(Tmot);
  timerAlarmEnable(Tmot);
  digitalWrite2(dirPin, (Sens)?HIGH:LOW); // Enable stepper
  MotDist=Dist;
    digitalWrite2(enPin, LOW); // Enable stepper
    MotMove=true;
}

void digitalWrite2(uint8_t PIN, uint8_t STATE) {
  bitWrite(data,PIN,STATE);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}

/////////////////////////////////////////////////////