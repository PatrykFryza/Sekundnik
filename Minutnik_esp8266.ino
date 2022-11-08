#include <Keypad.h>
#include <TM1637.h>

#define CLK 3
#define DIO D7
#define buzzer D8

TM1637 tm1637(CLK,DIO);

const uint8_t NumTab[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}; //0~9,A,b,C,d,E,F  co może wyświetlać wyświetlacz

const byte ROWS = 4; //zmienne potrzebne do klawiatury
const byte COLS = 3;

byte rowPins[ROWS] = {D0, D1, D2, 1};
byte colPins[COLS] = {D4, D5, D6};

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
Keypad board = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);


char key;
bool counting = 0;
int sum = 0;
uint16_t place[4]; 
uint8_t i;
unsigned long long next_seconds = 0;
unsigned long long prev_seconds = 0;

void setup() {
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  pinMode(buzzer, OUTPUT);
}

void loop() {
    key = board.getKey();
    if(key && (key != '*') && (key != '#') && (i < 4)){ //przechwytywanie klawiszy
      if(i>2) place[3] = place[2];
      if(i>1) place[2] = place[1];
      if(i>0) place[1] = place[0];
      place[0] = key -'0';
      ++i;
    }
    else if(key == '*') resetProgram();
    else if(key == '#' && !(counting)){
      counting = 1;
      sum = place[3] * 1000 + place[2] * 100 + place[1] * 10 + place[0] +1;
    }
    else if(counting){
      next_seconds = millis();
      if((next_seconds - prev_seconds) >= 1000){ //co jedną sekundę
        --sum;
        countingDisplay();
        prev_seconds = next_seconds;
      }
      else if(sum == 0){
        alarm();
        resetProgram();
      }
    }
    refreshDisplay();
}

void refreshDisplay(){
    tm1637.display(0,NumTab[place[3]]);
    tm1637.display(1,NumTab[place[2]]);
    tm1637.display(2,NumTab[place[1]]);
    tm1637.display(3,NumTab[place[0]]);
}

void countingDisplay(){ //funkcja odpowiedzialna za pokazywanie odliczania na wyświetlaczu
  if(place[0] > 0)
    --place[0];
  else if(place[1] > 0){
    place[0] = 9;
    --place[1];
  }
  else if(place[2] > 0){
    place[0] = 9;
    place[1] = 9;
    --place[2];
  }
  else if(place[3] > 0){
    place[0] = 9;
    place[1] = 9;
    place[2] = 9;
    --place[3];
  }
}

void alarm(){
  for(int j = 0; j < 6; ++j){
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(10);
  }
}

void resetProgram(){
    i = 0;
    sum = 0;
    counting = 0;
    place[0] = 0;
    place[1] = 0;
    place[2] = 0;
    place[3] = 0;
    tm1637.display(0,NumTab[0]);
    tm1637.display(1,NumTab[0]);
    tm1637.display(2,NumTab[0]);
    tm1637.display(3,NumTab[0]);
}
