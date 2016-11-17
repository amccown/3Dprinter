
/*
 * Test sketch for using the sparkfun stepper motor controller with
 * DVD drive stepper motor assembly.  
*/



#include <platformTroll.h>



//Declare pin functions on Redboard
#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define EN  6

#define OK_REPLY "ok\n" // this is the reply to acknowledge a valid command
#define BAUD (115200) // How fast is the Arduino talking?
#define MAX_BUF (64) // What is the longest message Arduino can store?

#define TOP_TO_START_OFFSET (10) // so 10 mm

char buffer[MAX_BUF]; // where we store the message until we get a ';'
int sofar;            // how much is in the buffer
int tr = 100;         // the travel rate of our z axis.  Generally 100
  

// constant for incrementing positions
#define NUM_OF_STEPS 500

//Declare variables for functions
char mode_abs=1;  // absolute mode?
float px, py;  // location


void setup() {

  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
  
  platformTroll_init(stp,dir,MS1,MS2,EN); // init my platformTroll
  Serial.begin(BAUD);       //Open Serial connection for debugging
  help();                   // The M100 message command  
}


/**
 * display helpful information
 */
void help() {
  Serial.println(F("Commands:"));
  Serial.println(F("G00 [X(steps)] [Y(steps)] [F(feedrate)]; - linear move"));
  Serial.println(F("G01 [X(steps)] [Y(steps)] [F(feedrate)]; - linear move"));
  Serial.println(F("G04 P[seconds]; - delay"));
  Serial.println(F("G90; - absolute mode"));
  Serial.println(F("G91; - relative mode"));
  Serial.println(F("G92 [X(steps)] [Y(steps)]; - change logical position"));
  Serial.println(F("M18; - disable motors"));
  Serial.println(F("M100; - this help message"));
  Serial.println(F("M114; - report position and feedrate"));
}


/*
 * Shutter controls.  
 */

 void shutter_open()
 {
  
 }

 void shutter_close()
 {
  
 }


/*
 * rogerRoger()
 * 
 * Sends the "ok\n" command via serial, to signal acknowledgment of the 
 * last command sent.  
 */
void rogerRoger()
{
  sofar = 0;
  Serial.print(OK_REPLY);
}


/**
 * Set the logical position
 * @input npx new position x
 * @input npy new position y
 */
void position(float npx,float npy) {
  // here is a good place to add sanity tests
  px=npx;
  py=npy;
}


/**
 * write a string followed by a float to the serial line.  Convenient for debugging.
 * @input code the string.
 * @input val the float.
 */
void output(const char *code,float val) {
  Serial.print(code);
  Serial.println(val);
}


/**
 * print the current position, feedrate, and absolute mode.
 */
void where() {
  output("X",px);
  output("Y",py);
//  output("F",fr);
  Serial.println(mode_abs?"ABS":"REL");
} 


/**
 * delay for the appropriate number of microseconds
 * @input ms how many milliseconds to wait
 */
void pause(long ms) {
  delay(ms/1000);
  delayMicroseconds(ms%1000);  // delayMicroseconds doesn't work for values > ~16k.
}

/*
 * Helper function 
 * set_TravelRate
 * Not sure when this value will get used, but setting it here
 */

void set_travelRate(int tr_in)
{
  tr = tr_in;
}

/**
 * Look for character /code/ in the buffer and read the float that immediately follows it.
 * @return the value found.  If nothing is found, /val/ is returned.
 * @input code the character to look for.
 * @input val the return value if /code/ is not found.
 **/
 
 float parsenumber(char code, float val)
 {
   for(int i=0; i < sofar ; i++)
   {
    if(buffer[i] == code)
    {
      return atof(&buffer[i+1]);
    }
   }
   return val;
 }
 

 /*
float parsenumber(char code,float val) {
  char *ptr=buffer;
  while(ptr>1 && *ptr && ptr<buffer+sofar) {
    if(*ptr==code) {
      return atof(ptr+1);
    }
    ptr=strchr(ptr,' ');
  }
  return val;
} 
*/





/**
 * Read the input buffer and find any recognized commands. One G or M command per line.
 */
void processCommand() {
  // look for commands that start with 'G'
  Serial.println("processCommand");
  int cmd=parsenumber('G',-1);
  float height_to_drop;
  Serial.println(cmd);
  //Serial.println(cmd);
  switch(cmd) {
  case 1: // move in a line
  {
    height_to_drop = parsenumber('Z', 0);      //get our height to drop
    set_travelRate(parsenumber('F',tr));       // sets the travel rate
    platformTroll_layerDown(height_to_drop);   // calls our layer function
  }
    break;
  case 21:  // set units to millimeters. 
    platformTroll_init(stp,dir,MS1,MS2,EN); // init my platformTroll
    break;
    
  case 28:
    platformTroll_returnToStart();
   // platformTroll_layerUp(TOP_TO_START_OFFSET); // move up some extra just in case
    break;
  //case 90: // absolute mode
   // mode_abs=1; 
  //  break; 
  case 91: // relative mode
    mode_abs=false; 
    break; 
  case 162:
    platformTroll_returnToStart();
    break;
  default: break;
  }

  // look for commands that start with 'M'
  cmd=parsenumber('M',-1);
  switch(cmd) {
  case 18: // turns off power to steppers (releases the grip)
    platformTroll_shutDown();
    break;
  case 280:
    if(parsenumber('S', -1) == 500) // this is the S500 command open shutter
      shutter_open();               // if we get a value of 500, open it
    else                            // for any other value, close it
      shutter_close();
    break;
  case 100: help(); break;
  //case 114: where(); break; // prints px, py, fr, and mode.
  default: break;
  }

  // if the string has no G or M commands it will get here and the Arduino will silently ignore it
}


/*
 * Main loop. This just listens for commands on the Serial line. 
 */
void loop() {
  // listen for commands
  if( Serial.available() ) 
  { 
    // store the byte as long as there's room in the buffer.
    // if the buffer is full some data might get lost
    char c;
    if(sofar < MAX_BUF)
    {
      c = Serial.read();          // get it
      buffer[sofar++]=c;
    }
    // if we got a return character (\n) the message is done.
    if(c=='\n') 
    {
      // strings must end with a \0.  ???
      buffer[sofar]=0;
      processCommand(); // do something with the command
      rogerRoger();
    }
  }
}















 
