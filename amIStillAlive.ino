//Am I alive?
/*This sketch is meant to use two thermistors to measure body temperature
  and ambient temperature to determine if the user of the device is dead
  If the user is alive it will alert them of the fact, and if they are dead
  it will alert them by other means*/

#define tempSlope -0.11494
#define tempIntercept 103.74
#define bodyPin 1 //use this pin for the body sensor
#define ambPin 2 //use this pin for the ambient temp sensor
#define ledPin 0 //use this pin to control the LED output
#define buzzerPin 1 //this must be a PWM pin in order to make use of the tone library
#define checkInterval 10 //this controls how often the code will check if you are alive or not
#define deathThreshold 15//if you are less than this many degrees above ambient temperature, you are proably dead


#define REST 0
#define B0  31
#define C1  33
#define CS1 35
#define D1  37
#define DS1 39
#define E1  41
#define F1  44
#define FS1 46
#define G1 49
#define GS1 52
#define A1  55
#define AS1 58
#define B1  62
#define C2  65
#define CS2 69
#define D2  73
#define DS2 78
#define E2  82
#define F2  87
#define FS2 93
#define G2  98
#define GS2 104
#define A2  110
#define AS2 117
#define B2  123
#define C3  131
#define CS3 139
#define D3  147
#define DS3 156
#define E3  165
#define F3  175
#define FS3 185
#define G3  196
#define GS3 208
#define A3  220
#define AS3 233
#define B3  247
#define C4  262
#define CS4 277
#define D4  294
#define DS4 311
#define E4  330
#define F4  349
#define FS4 370
#define G4  392
#define GS4 415
#define A4  440
#define AS4 466
#define B4  494
#define C5  523
#define CS5 554
#define D5  587
#define DS5 622
#define E5  659
#define F5  698
#define FS5 740
#define G5  784
#define GS5 831
#define A5  880
#define AS5 932
#define B5  988
#define C6  1047
#define CS6 1109
#define D6  1175
#define DS6 1245
#define E6  1319
#define F6  1397
#define FS6 1480
#define G6  1568
#define GS6 1661
#define A6  1760
#define AS6 1865
#define B6  1976
#define C7  2093
#define CS7 2217
#define D7  2349
#define DS7 2489
#define E7  2637
#define F7  2794
#define FS7 2960
#define G7  3136
#define GS7 3322
#define A7  3520
#define AS7 3729
#define B7  3951
#define C8  4186
#define CS8 4435
#define D8  4699
#define DS8 4978
/*************************************************
* Note Duration Constants  (not used in this app)
*************************************************/
#define FN 1    // 64th note
#define TN 2    // 32nd note
#define DTN 3   // dotted 32nd note
#define SN 4    // 16th note
#define DSN 6   // dotted 16th note
#define EN 8    // 8th note
#define DEN 12  // dotted 8th note
#define QN 16   // quarter note
#define DQN 24  // dotted quarter note
#define HN 32   // half note
#define DHN 48  // dotted half note
#define WN 64   // whole note
#define DWN 96  // dotted whole note
/*************************************************
* Music Content is stored in the following array.
* PROGMEM is needed because of the large data size
.
* "Prelude in C" Well
-
Tempered Clavier, J.S.Bach
*************************************************/
#define NOTECOUNT 25             // length of array
#include <avr/pgmspace.h>         // needed for PROGMEM
PROGMEM const uint16_t lengthArray[] = //contains the length of each note
{
  };

PROGMEM const uint16_t  noteArray[] = 
{
/*F4, GS4, AS4, C5, C5, AS4, GS4, AS4, //measures 1-2, notes 1-32
F4, GS4, GS4, GS4, AS4, AS4, F4, F4,
GS4, GS4, GS4, F4, E4, F4, F4, F4,
C5, C5, C5, C5, F4, GS4, GS4, GS4,
AS4, AS4, F4, F4, GS4, GS4, GS4, GS4, //measures 3-4, notes 33-64
E4, F4, F4, F4, F4, GS4, GS4, AS4, 
C4, E4, G4, C5, E5, G4, C5, E5,*/
C5, C5,REST, AS4, AS4,REST, GS4, GS4,REST, G4, G4,REST,
G4, GS4,GS4, GS4, GS4,REST, G4, GS4,GS4, GS4, GS4,REST,
/*C5, C5,REST, AS4, AS4,REST, GS4, GS4,REST, G4, G4,
G4, GS4, GS4, GS4,REST, G4, GS4, GS4, GS4,REST,
C5, C5,REST, AS4, AS4,REST, GS4, GS4,REST, G4, G4,
G4, GS4, GS4, GS4,REST, G4, GS4, GS4, GS4,REST,
C5, C5,REST, AS4, AS4,REST, GS4, GS4, G4, G4,
G4, GS4, GS4, GS4, G4, GS4, GS4, GS4,
C5, C5, AS4, AS4, GS4, GS4, G4, G4,
G4, GS4, GS4, GS4, G4, GS4, GS4, GS4,*/
};

#define NOTECOUNTTWO 32
PROGMEM const uint16_t noteArrayTwo[] = {
  A4,A4,REST,A4,A4,REST,A4,REST,A4,A4,REST,C5,C5,C5,REST,B4,REST,B4,B4,REST,A4,REST,A4,A4,REST,A4,REST,A4,A4,A4,A4,A4
};

volatile uint32_t toggle_count;
// TrinketTone:
// Generate a square wave on a given frequency &duration
// Call with frequency (in hertz) and duration (in milliseconds).
// Uses Timer1 in CTC mode.  Assumes PB1 already in OUPUT mode.
// Generated tone is non-blocking, so routine immediately
// returns while tone is playing.
void TrinketTone
(uint16_t frequency, uint32_t duration)
{ 
// scan through prescalars to find the best fit
uint32_t ocr = F_CPU/frequency/2;
uint8_t  prescalarBits = 1;
while (ocr>255)
{
prescalarBits++;
ocr /= 2; 
}
// CTC mode; toggle OC1A pin; set prescalar
TCCR1 = 0x90 | prescalarBits;
// Calculate note duration in terms of toggle count
// Duration will be tracked by timer1 ISR
toggle_count = frequency * duration / 500;
OCR1C = ocr
-
1;            
// Set the OCR 
bitWrite(TIMSK, OCIE1A, 1);      // enable interrupt
}
// Timer1 Interrupt Service Routine:
// Keeps track of note duration via toggle counter
// When correct time has elapsed, counter is disabled
ISR(TIMER1_COMPA_vect)
{
if
(toggle_count != 0)             // done yet?
toggle_count--;                  // no, keep counting
else                               // yes,
TCCR1 = 0x90;                    // stop the counter
}
// PlayBach:
// Plays "Prelude in C", which isheld in noteArray
// Uses PROGMEM to store array, due to large size
void PlayAlive()
{
int len = 140;                     // modify for speed
int dly = 140;                     // modify for articulation
for (int count=0; count<NOTECOUNT; count++)
{
TrinketTone( pgm_read_word( &noteArray[count] ), len);
delay(dly);

}
//TrinketTone(C4,1500);              // final note
//delay(1500);
}

void PlayFuneral(){
  int len = 250;                     // modify for speed
int dly = 140;                     // modify for articulation
for (int count=0; count<NOTECOUNTTWO; count++)
{
TrinketTone( pgm_read_word( &noteArrayTwo[count] ), len);
delay(dly);

}
//TrinketTone(C4,1500);              // final note
//delay(1500);
}

int tempBod = 0; //declare these as globals so we won't have to waste instructions declaring them each time through the loop
int tempAmb = 0;

char notes[] = "cdfda ag cdfdg gf "; // a space represents a rest
int beats[] = {1,1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,2};

float getTemp(int AnVal) {
  return (tempSlope * AnVal + tempIntercept);
}

void toneSequenceAlive() {
  PlayAlive();
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
}

void toneSequenceDead() {
  PlayFuneral();
  
}

void setup() {
  pinMode(2, INPUT); //declare one analog input for the body temperature sensor
  pinMode(4, INPUT); //declare analog input for the ambient temperature sensor

  pinMode(ledPin, OUTPUT); //declare the pin for the LED as an output
  //I think the speaker pin declaration will be taken care of by the tone library?
  
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  /*Here is the general order of business for the loop:
    0. wait the appropriate interval
    1. read each of the sensors
    2. find the difference in temperatures
    3. decide if that difference indicates death (if they aren't different enough)
    4. play the appropriate tone&light sequence*/

  //step 0:
  //while((millis() % (1000 * checkInterval)) < 10) {} //this should work???
  delay(1000 * checkInterval);

  //step 1:
  tempBod = analogRead(1);
  tempAmb = analogRead(2);



  //step 2&3&4:
  if (((tempBod) - (tempAmb)) > (deathThreshold)) {
    toneSequenceAlive();
  }
  else {
    toneSequenceDead();
  }

  int i, duration;
  int songLength = strlen(notes);
  int tempo = 150;
  
  for (i = 0; i < songLength; i++) // step through the song arrays
  {
    duration = beats[i] * tempo;  // length of note/rest in ms

    if (notes[i] == ' ')          // is this a rest?
    {
      delay(duration);            // then pause for a moment
    }
    else                          // otherwise, play the note
    {
      tone(buzzerPin, frequency(notes[i]), duration);
      delay(duration);            // wait for tone to finish
    }
    delay(tempo / 10);            // brief pause between notes
  }

  

  // We only want to play the song once, so we'll pause forever:
  //while (true) {}
  // If you'd like your song to play over and over,
  // remove the above 
}



// adapted from SparkFun Inventor Kit https://www.sparkfun.com/products/12001
// Please download the original source code for detail comment of the source code
// http://cdn.sparkfun.com/datasheets/Kits/SIK%20Guide%20Code.zip




int frequency(char note)
{
  // This function takes a note character (a-g), and returns the
  // corresponding frequency in Hz for the tone() function.

  int i;
  const int numNotes = 8;  // number of notes we're storing

  // The following arrays hold the note characters and their
  // corresponding frequencies. The last "C" note is uppercase
  // to separate it from the first lowercase "c". If you want to
  // add more notes, you'll need to use unique characters.

  // For the "char" (character) type, we put single characters
  // in single quotes.

  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};

  // Now we'll search through the letters in the array, and if
  // we find it, we'll return the frequency for that note.

  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return (frequencies[i]);    // Yes! Return the frequency
    }
  }
  return (0); // We looked through everything and didn't find it,
  // but we still need to return a value, so return 0.
}


