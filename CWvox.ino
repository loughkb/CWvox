/*
 * CWvox voice keyer for Dah dit dah dit, dit dah dah
 * Version 1.0  Jan 17th, 2023.  By Kevin Loughin, KB9RLW
 * 
 * Senses audio from an external condenser microphone and
 * keys any modern radio through the key jack in CW mode.
 * Most headsets use these mics.  CWvox provides a bias voltage
 * on the mic line to power the mic.
 * 
 * Speak your morse into the microphone as the words dah and dit
 * for dash and dot.  The vocal length of the words dah and dit
 * are about the same ratio as properly sent CW.  
 * 
 * The CWvox will follow you, keying the radio for the duration of each 
 * spoken word. 
*/


//Harware defines for the current design
#define audiosensepin A7 // The input the audio comes into.
#define keypin 13       // The output that keys the radio.
#define sensitivitypin A6 // The input from the sensitivity pot.
#define LEDpin 10 // LED indicator pin  

// Declare variables and flags we will use in the program
int audiopresent = 0; // flag to track audio detection state
int audio = 0; // variable to store sensed audio value
int tail = 0; // audio debounce tail for clean keying
int sensitivity = 250; // variable to hold current sensetivity pot value
int keyed = 0; // Flag to track key state

void setup() 
{
  
//Serial.begin(9600); // enable serial for debugging output

// initialize the hardware pins
pinMode(audiosensepin, INPUT);
pinMode(keypin, OUTPUT);
digitalWrite(keypin, LOW); // Make sure we're not keying the radio at startup.
pinMode(LEDpin, OUTPUT);
digitalWrite(LEDpin, LOW); // Make sure LED is off.
pinMode(sensitivitypin, INPUT);
analogWrite(LEDpin, 50); // Turn the LED half on for power light.

  // Setup the interrupt for the timer interrupt routine
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}


// interrupt handler routine.  Handles the timing counters, runs 1000 times per second
SIGNAL(TIMER0_COMPA_vect)
{
  if (tail) // If we're in the tail period, count down
  {
    tail--;
  }
}

// This is the main program loop. Runs continuously:
void loop()
{
// First we read the hardware

audio = analogRead(audiosensepin);
sensitivity = analogRead(sensitivitypin);

//  Keying logic
if (audio > sensitivity) // audio detected.
  {
    tail = 18; // audio framing tail counter reset
    if (!keyed) // If not already keyed, key the rig
      {
        Keydown(); // key the radio and turn on LED
        keyed = 1; // set flag
      }
  }

if ((audio < sensitivity) && keyed && !tail) // audio lost, passed frame time and we're keyed
  {
    Keyup(); // release keying to radio and turn off LED
    keyed = 0; // set the flag
    
  }

}

// defined functions to simplify the main code loop

void Keydown()
{
  digitalWrite(keypin, HIGH);
  analogWrite(LEDpin, 255);
}

void Keyup()
{
  digitalWrite(keypin, LOW);
  analogWrite(LEDpin, 50);
}
