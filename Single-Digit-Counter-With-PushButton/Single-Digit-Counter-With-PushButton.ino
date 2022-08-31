/* This is a valid comment

  A sketch to work with a 7 segment display and 8-bit shift register and a push button.
  The 7 segment displays numbers starting countStart to countEnd with a delay equals to countDelay between each update
  When the push button is pushed for a short period , counting reset happens
  When the push button is pushed for a long period , counting order (ascending, descending) toggles.

  ## Interrupt Version

  by : Abdelrahman Galal

*/

const byte dataPin = 2;
const byte pushButtonPin = 3;
const byte shiftClockPin = 4;
const byte memoryClockPin = 7;
volatile int countStart = 0; //counter start point
volatile int countEnd = 9; //counter end point
const int countDelay = 1000;
unsigned long countPreviousTime = 0 ; //last time the counter digit was changed
volatile unsigned int pushButtonCount = 0;  //measure how relatively long the push button is ON
byte leds[] = {252, 96, 218, 242, 102, 182, 190, 224, 254, 246};

//setup function
void setup()
{
  pinMode(dataPin, OUTPUT);
  pinMode(shiftClockPin, OUTPUT);
  pinMode(memoryClockPin, OUTPUT);
  pinMode(pushButtonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pushButtonPin), checkPushButton, LOW );
}

//loop function
void loop()
{
  count(countStart, countEnd, countDelay);
}

//count function from countStart till countEnd with a delay delayTime between each 7 segemnt digit change.
void count(int countStart, int countEnd, unsigned long delayTime)
{
  for (int i = countStart ; i <= countEnd ; i++)
  {
    outSeven(leds[abs(i)]);
    //Ask for delay and break if the push button is pressed (ON)
    if (delayMillis(delayTime) || pushButtonCount )
    {
      pushButtonCount = 0;
      break;
    }
  }
}

//function to change the shown digit of the 7 segment
void outSeven(byte led)
{
  digitalWrite(memoryClockPin, LOW);
  shiftOut(dataPin, shiftClockPin, LSBFIRST, led);
  digitalWrite(memoryClockPin, HIGH);
}

//function to perfrom required delay in millisecond
int delayMillis(unsigned long delay)
{
  unsigned long countCurrentTime;
  signed long countDelta = 0;
  signed long previousCountDelta;
  while (1)
  {
    countCurrentTime = millis();
    //keep the old delta to use in case of millis() overflow
    previousCountDelta = countDelta;
    //diff between the current time and last time the 7 segment digit was updated
    countDelta = countCurrentTime - countPreviousTime;
    //handle the case when millis() overflow and countDelta becomes negative
    if ( countDelta < 0 )
    {
      countPreviousTime = -1 * previousCountDelta;
      countDelta = countCurrentTime - countPreviousTime;

    }
    //return when delay millis have passed
    if ( countDelta >= delay )
    {
      countPreviousTime = countCurrentTime;
      return 0;
    }
  }
}

void checkPushButton()
{
  pushButtonCount++;
  //if the push duration is long enough
  if ( pushButtonCount == 150 )
  {
    int tmp = countStart;
    countStart = -1 * countEnd;
    countEnd = -1 * tmp;
  }
}
