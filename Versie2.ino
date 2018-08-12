typedef struct
{
  int r;
  int g;
  int b;
} RGB_type;

#define INHIBIT 3

#define RBLINKER 4
#define LBLINKER 5
#define BRAKE 6

#define PERIODIN 7
#define SIMOUT 8

#define RED 9
#define GREEN 10
#define BLUE 11

#define A 12
#define B 13

#define MODE_RPM 0
#define MODE_BRAKE 1
#define MODE_LBLINKER 2
#define MODE_RBLINKER 3

int frequentie = 0;
int frequentieOld = 0;

unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned int delta = 0;

unsigned long oldSimTime = 0;
unsigned long actSimTime = 0;

int bufferFilter [16] = {0};
int bufferIndex = 0;
int average = 0;

RGB_type channelOne = {0, 0, 255};
RGB_type channelTwo = {0, 0, 255};
RGB_type channelThree = {0, 0, 255};

int mode = 0;
int oldMode = 0;

int modeRPMgbColor = 0;
bool newDelta = false;

void setup()
{
  Serial.begin(9600);
  Serial.println("setup");

  pinMode(INHIBIT, OUTPUT);

  pinMode(RBLINKER, INPUT);
  pinMode(LBLINKER, INPUT);
  pinMode(BRAKE, INPUT);

  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);

  pinMode(SIMOUT, OUTPUT);
  pinMode(PERIODIN, INPUT);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  frequentieOld = digitalRead(7);

  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  SetChannelColor(channelOne);

  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  SetChannelColor(channelTwo);

  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  SetChannelColor(channelThree);
}

void loop()
{
  //  mode = MODE_RPM;
  //  if (digitalRead(BRAKE) == HIGH)
  //  {
  //    mode = MODE_BRAKE;
  //  }
  //  else
  //  {
  //    if (digitalRead(LBLINKER) == HIGH)
  //    {
  //      mode = MODE_LBLINKER;
  //    }
  //    else if (digitalRead(RBLINKER) == HIGH)
  //    {
  //      mode = MODE_RBLINKER;
  //    }
  //  }

  if (newDelta)
  {
    //    if (mode == MODE_RPM)
    //    {
    channelTwo.r = modeRPMgbColor;
    channelTwo.g = modeRPMgbColor;
    channelTwo.b = 255;
    channelThree.r = modeRPMgbColor;
    channelThree.g = modeRPMgbColor;
    channelThree.b = 255;
    //    }

    channelOne.r = modeRPMgbColor;
    channelOne.g = modeRPMgbColor;
    channelOne.b = 255;

    newDelta = false;
  }

  //  if (mode != oldMode)
  //  {
  //    Serial.println("mode:" + String(mode));
  //    switch (mode)
  //    {
  //      case MODE_RPM:
  //        break;
  //
  //      case MODE_BRAKE:
  //        channelTwo.r = 255;
  //        channelTwo.g = 0;
  //        channelTwo.b = 0;
  //
  //        channelThree.r = 255;
  //        channelThree.g = 0;
  //        channelThree.b = 0;
  //        break;
  //
  //      case MODE_LBLINKER:
  //        channelTwo.r = 255;
  //        channelTwo.g = 60;
  //        channelTwo.b = 0;
  //        break;
  //
  //      case MODE_RBLINKER:
  //        channelThree.r = 255;
  //        channelThree.g = 60;
  //        channelThree.b = 0;
  //        break;
  //
  //      default:
  //        break;
  //    }
  //
  //    oldMode = mode;
  //  }

  SimulateRPM();

  AveragePeriod();

  //        channelTwo.r = 0;
  //        channelTwo.g = 0;
  //        channelTwo.b = 0;
  //        channelThree.r = 0;
  //        channelThree.g = 0;
  //        channelThree.b = 0;

  WriteChannels();
}

void WriteChannels()
{
  //  Serial.println("Channel 1:" + String(channelOne.r) + "," + String(channelOne.g) + "," + String(channelOne.b));
  digitalWrite(INHIBIT, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  SetChannelColor(channelOne);
  digitalWrite(INHIBIT, HIGH);

  //  Serial.println("Channel 2:" + String(channelTwo.r) + "," + String(channelTwo.g) + "," + String(channelTwo.b));
  digitalWrite(INHIBIT, LOW);
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  SetChannelColor(channelTwo);
  digitalWrite(INHIBIT, HIGH);

  //  Serial.println("Channel 3:" + String(channelThree.r) + "," + String(channelThree.g) + "," + String(channelThree.b));
  digitalWrite(INHIBIT, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  SetChannelColor(channelThree);
  digitalWrite(INHIBIT, HIGH);
}

void SetChannelColor(RGB_type rgb)
{
  analogWrite(RED, rgb.r);
  analogWrite(GREEN, rgb.g);
  analogWrite(BLUE, rgb.b);
}


void AveragePeriod ()
{
  frequentie = digitalRead(7);

  if (frequentie == HIGH and frequentieOld == LOW)
  {
    //Dit is een positieve flank
    endTime = millis();
    delta = endTime - startTime;
    startTime = endTime;

    if (delta < 100)
    {
      bufferFilter[bufferIndex % 16] = delta;
      bufferIndex++;

      for (int i = 0; i < 16; i++)
      {
        average += bufferFilter[i];
      }
      average /= 16;

      newDelta  = true;
      modeRPMgbColor = map(average, 27, 5, 0, 255);
      //            Serial.println( String(average) + ',' + String(modeRPMgbColor));
    }
  }

  frequentieOld = frequentie;
}

int SimulateInterval()
{
  int a = analogRead(A0);
  int t = map(a, 0, 1023, 28, 8);
  return t;
}

void SimulateRPM()
{
  actSimTime = millis();
  digitalWrite(8, LOW);
  if (actSimTime - oldSimTime > SimulateInterval())
  {
    digitalWrite(8, HIGH);
    oldSimTime = actSimTime;
  }
}

