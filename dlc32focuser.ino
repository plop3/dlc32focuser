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
byte data=0;                                // Etat de  EN, step, dir... (74HC595)
static volatile unsigned int Position;                      // Position du focuser
static volatile unsigned int NextPos;                       // Position à atteindre
int Sens=1;                                 // Sens de rotation (-1, 1)
static volatile bool MotMove=false;         // Moteur en mouvement 

hw_timer_t *Tmot = NULL;

/////////////////////////////////////////////////////
// Fonctions rapides //
void IRAM_ATTR motorRun() {
  if (Position == NextPos) {
    timerAlarmDisable(Tmot);
    digitalWrite2(enPin, HIGH);
    MotMove=false;
  }
  else {
      digitalWrite2(stepPin, HIGH);
      digitalWrite2(stepPin, LOW);
      Position+=Sens;
  }
}

/////////////////////////////////////////////////////
// Setup, Loop //
void setup() {
  Serial.begin(115200);
  Position=0;
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
  while (MotMove);
    delay(2000);
    MoveTo(5000);
  while (MotMove);
    delay(2000);
    MoveTo(0);
  
}

/////////////////////////////////////////////////////
// Fonctions //
void StopMotor() {
  NextPos=Position;
}

void MoveTo(int pos) {
  NextPos=pos;
  Sens=(NextPos>=Position) ? 1 : -1;
  timerAlarmEnable(Tmot);
  digitalWrite2(dirPin, (Sens>0)? false: true); // Sens de rotation
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