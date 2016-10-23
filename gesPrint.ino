#include "gesPrint.h"

void gesPrint(char *phrase){
  if (debug)
    Serial.println(phrase);
}
