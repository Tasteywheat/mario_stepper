/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Guitar TAB used to determine notes (I can't really read sheet music, so it was easier to convert guitar tab to the notes): 
 *  https://tabs.ultimate-guitar.com/tab/misc_computer_games/super_mario_brothers_theme_tabs_189720 
 *  
 * Sheet Music used for the timing: 
 *  http://www.mariopiano.com/mario-sheet-music-overworld-main-theme.html
 *    
 * Frequencies of notes: https://pages.mtu.edu/~suits/notefreqs.html
 * 
 * Description:
 *  This code plays a Mario Bros overworld theme snippet on a stepper motor using interrupts. 
 *  
 * Hardware Used:
 *  Arduino Duemilanove or equvalent
 *  Stepper motor (I used a bipolar one)
 *  BigEasy Driver with the STEP pin connected to STEP_PIN (D4 by default)
 *  
*/

#define TIMER_HZ 62500
#define MAX_TIMER_INTERVAL 65534
#define STEP_PIN 4

//Any frequencies not in this range result in silence
#define AUDIBLE_MIN 20          //Human ear low is 20 hertz
#define AUDIBLE_MAX 20000       //Human ear high is 20000 hertz

#define NONE 0

//Quater note, half note and whole note durations milliseconds
#define Q_NOTE 150
#define Q_NOTE_PLUS 200 //Quarter note with a "." (quarter note plus a bit extra, I chose 33% based off sheet music)
#define H_NOTE 300
#define W_NOTE 600
#define PLUCK 30       //The time, in milliseconds, a non sustained note will go silent when played repeatedly. 


//Note frequencies see https://pages.mtu.edu/~suits/notefreqs.html for reference 
#define REST 1  //Used to produce a pause/silence to stop repetitive notes from blurring into one
#define E4 329
#define G4 392
#define GS4 415
#define A4 440
#define AS4 466
#define B4 494
#define C5 523
#define D5 587
#define DS5 622
#define E5 659
#define F5 698
#define FS5 740
#define G5 784
#define A5 880
#define B5 988
#define C6 1046

struct Note 
{
  uint16_t freq;       //Frequency of the note
  uint16_t len;        //Total length of the Note/Beat (how long before the next note is played)
  uint16_t restLen;    //The time taken to switch from one note to the next (frequencies between 20Hz and 20KHz)
                       //If the beat is a REST restLen is the total length of the REST
};

Note MarioTheme[] = {
//Opening
{E5, Q_NOTE, PLUCK}, {E5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {E5, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK}, {E5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} ,
{G5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {REST, 0, H_NOTE}, {G4, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE} , {REST, 0, H_NOTE},

//First verse
{C5, Q_NOTE, PLUCK}, {REST, 0, H_NOTE}, {G4, Q_NOTE, PLUCK},  {REST, 0, H_NOTE}, 
{E4, Q_NOTE, PLUCK}, {REST, 0, H_NOTE}, {A4, Q_NOTE, PLUCK},  {REST, 0, Q_NOTE},
{B4, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE}, {AS4, Q_NOTE, PLUCK}, {A4, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE}, 

{G4, Q_NOTE_PLUS, PLUCK}, {E5, Q_NOTE_PLUS, PLUCK}, {G5, Q_NOTE_PLUS, PLUCK}, {A5, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE} , {F5, Q_NOTE, PLUCK}, {G5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE},
{E5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK}, {D5, Q_NOTE, PLUCK},
{B4, Q_NOTE, PLUCK}, {REST, 0, H_NOTE},

//First verse
{C5, Q_NOTE, PLUCK}, {REST, 0, H_NOTE}, {G4, Q_NOTE, PLUCK},  {REST, 0, H_NOTE}, 
{E4, Q_NOTE, PLUCK}, {REST, 0, H_NOTE}, {A4, Q_NOTE, PLUCK},  {REST, 0, Q_NOTE},
{B4, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE}, {AS4, Q_NOTE, PLUCK}, {A4, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE}, 

{G4, Q_NOTE_PLUS, PLUCK}, {E5, Q_NOTE_PLUS, PLUCK}, {G5, Q_NOTE_PLUS, PLUCK}, {A5, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE} , {F5, Q_NOTE, PLUCK}, {G5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE},
{E5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK}, {D5, Q_NOTE, PLUCK},
{B4, Q_NOTE, PLUCK}, {REST, 0, H_NOTE},          

//Second Verse
{REST, 0, H_NOTE}, {G5, Q_NOTE, PLUCK}, {FS5, Q_NOTE, PLUCK}, {F5, Q_NOTE, PLUCK},
{DS5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {E5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE},
{GS4, Q_NOTE, PLUCK}, {A4, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE},
{A4, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {D5, Q_NOTE, PLUCK},

{REST, 0, H_NOTE}, {G5, Q_NOTE, PLUCK}, {FS5, Q_NOTE, PLUCK}, {F5, Q_NOTE, PLUCK},
{DS5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE}, {E5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE},
{C6, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE}, {C6, Q_NOTE, PLUCK}, {C6, Q_NOTE, PLUCK},
{REST, 0, H_NOTE}, {REST, 0, Q_NOTE},  

{REST, 0, H_NOTE}, {G5, Q_NOTE, PLUCK}, {FS5, Q_NOTE, PLUCK}, {F5, Q_NOTE, PLUCK},
{DS5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {E5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE},
{GS4, Q_NOTE, PLUCK}, {A4, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE},
{A4, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {D5, Q_NOTE, PLUCK},

{REST, 0, H_NOTE}, {DS5, Q_NOTE, PLUCK}, {REST, 0, H_NOTE}, {D5, Q_NOTE, PLUCK},
{REST, 0, H_NOTE}, {C5, Q_NOTE, PLUCK}, {REST, 0, H_NOTE}, {REST, 0, Q_NOTE},  
{REST, 0, W_NOTE},

//Final Verse (that can fit anyways)
{C5, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK}, {D5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} ,
{E5, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {A4, Q_NOTE, PLUCK},
{G4, Q_NOTE, PLUCK}, {REST, 0, H_NOTE}, {REST, 0, Q_NOTE},  

{C5, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK}, {D5, Q_NOTE, PLUCK}, {E5, Q_NOTE, PLUCK},
{REST, 0, W_NOTE}, {REST, 0, W_NOTE},

{C5, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK},
{REST, 0, Q_NOTE} , {C5, Q_NOTE, PLUCK}, {D5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} ,
{E5, Q_NOTE, PLUCK}, {C5, Q_NOTE, PLUCK}, {REST, 0, Q_NOTE} , {A4, Q_NOTE, PLUCK},
{G4, Q_NOTE, PLUCK}, {REST, 0, H_NOTE}, {REST, 0, Q_NOTE}  
};

enum state {PLAYING, RESTING, IDLING};
const uint8_t NOTE_COUNT = (sizeof(MarioTheme)/sizeof(Note) );
volatile long timerInterval = 0;
volatile uint16_t delta = 0; 
volatile state currentState = IDLING;
volatile long currentPos = 0;

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);

  Serial.begin(115200);

  Serial.println("Setting compare interrupt A");
  cli();
  TCCR1A = 0;
  //CTC mode (WGM12) and 256 prescaler (CS12) gives us 62.5KHz on a 16MHz arduino
  TCCR1B = (1<<WGM12) | (1<<CS12);
  OCR1A = MAX_TIMER_INTERVAL;
  TCNT1 = 0;

  Serial.println("Enabling compare interrupt A");
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

void loop() {
  switch(currentState){
    case IDLING:
      idleState();
      break;
    case PLAYING:
      playingState();
      break;
    case RESTING:
      restingState();
      break;
  }
  //Didn't really want to use the delay funciton however using millis()
  //and computing the elapsed time made the loop to slow
  delta++;
  delay(1); 
}

void idleState(){
  setTone(MarioTheme[currentPos].freq);
  if(MarioTheme[currentPos].freq != REST){
    currentState = PLAYING;
  }else{
    currentState = RESTING;
  }
}

void playingState(){
  uint16_t toneLength = MarioTheme[currentPos].len - MarioTheme[currentPos].restLen;
  if(delta >= toneLength){
    setTone(REST);
    delta = 0;
    currentState = RESTING;
  }
}

void restingState(){
  if(delta >= MarioTheme[currentPos].restLen){
    currentPos++;
    if(currentPos >= NOTE_COUNT) currentPos = 0;
    delta = 0;
    currentState = IDLING;
  }
}

ISR(TIMER1_COMPA_vect){ 
  //Set pin 4 high 
  PORTD |= (1 << PORTD4);
  //BigEasy driver requires 2us delay for the change to register
  delay_1us();
  delay_1us();
  //Set pin 4 low
  PORTD &= ~(1 << PORTD4);
  delay_1us();
  delay_1us();
}

void setTone(int16_t theFrequency){
  
  if ((theFrequency > AUDIBLE_MIN) && (theFrequency < AUDIBLE_MAX)){
    timerInterval = (TIMER_HZ / theFrequency);
    
    if(timerInterval > MAX_TIMER_INTERVAL)
      timerInterval = MAX_TIMER_INTERVAL;
      
  }else{
    timerInterval = MAX_TIMER_INTERVAL; 
  }
  
  OCR1A = timerInterval;
  if(TCNT1 > OCR1A) TCNT1 = 0;
 
}

void delay_1us()
{
  __asm__ __volatile__ (
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop");
}
