#include "Arduino.h"
#include "types.h"

int32 timeFixMiliSeconde (int32 MiliSeconde)
{
  int32 Time_Running = millis();
  return ( Time_Running + MiliSeconde);
}
int32 timeFixSeconde (int32 Seconde)
{
  int32 Time_Running = millis();
  return ( Time_Running + (Seconde * 1000) );
}
int32 timeFixMinute (int32 Minute)
{
  int32 Time_Running = millis();
  return ( Time_Running + (Minute * 60 * 1000) );
}

boolean TimeElapsed ( int32 timeToCompare )
{
  int32 Time_Running = millis();
//  Serial.println(Time_Running);
//  Serial.println(timeToCompare);
  return ( (Time_Running - timeToCompare) > 0 );
}
