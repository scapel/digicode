#include "Arduino.h"
#include "switchLed.h"
#include "types.h"
#include "timeControl.h"

#define PasDuFade 4

//Variable globale au module
int16 PowerLed[2];
boolean FadeLed[2];
boolean FadeLedRunning[2] = {false,false};
boolean InitLed[2] = {false,false};
boolean EtatLed[2];
int32 TimeFade[2];

void InitSwitchLed ()
{
  // Gpio correspondant aux leds en sortie.
    pinMode(PosLed[LEDS_ROUGES], OUTPUT);
    pinMode(PosLed[LEDS_BLEUS], OUTPUT);
}

void SwitchLed ( boolean Etat, boolean Fade, Leds_T Led )
{
  if (!Fade)
  {
    // led directement allumée au max
    PowerLed[Led] = 255 * Etat;
  }
  else
  {
    // led a l'etat oposé pour augmenter cressendo
    PowerLed[Led] = 255 * ( 1 - Etat) ;    
  }
  
  /*Initialise les vars utiles pour la fonction periodique*/
  FadeLed[Led] = Fade;
  FadeLedRunning[Led] = false;
  InitLed[Led] = true;
  EtatLed[Led] = Etat;
  
}

void SwitchLedPeriod (void)
{
  uint8 Led;
  
  // gere les deux groupes de leds
  for (Led = 0; Led < 2; Led++)
  {
    /*Test si une demande d'etat de led a ete demandé via SwitchLed */
    if ( InitLed[Led] )
    {
      /*Si un Fade a ete demande*/
      if ( FadeLed[Led] )
      {
        /*test si le Fade nest pas en cours */
        if ( !FadeLedRunning[Led] )
        {
          FadeLedRunning [Led] = true;
          TimeFade [Led ] = timeFixMiliSeconde ( 10 );
        }
        else
        {
          // Fade en cours
          if ( TimeElapsed (TimeFade [Led ]) )
          {
            //reprogramme le prochain reveil!
            TimeFade [Led ] = timeFixMiliSeconde ( 10 );
            
            if ( EtatLed [Led] )
            {
              // fade croissant (allumage de la led)
              if ( PowerLed [Led] + PasDuFade < 255 )
              {
                PowerLed [Led] = PowerLed [Led] + PasDuFade;
              }
              else
              {
                PowerLed [Led] = 255;
                InitLed[Led] = false;
              }
            }
            else
            {
              // fade decroissant
              if ( (PowerLed [Led] - PasDuFade) > 0 )
              {
                PowerLed [Led] = PowerLed [Led] - PasDuFade;
              }
              else
              {
                InitLed[Led] = false;
                PowerLed [Led] = 0;
              }
            }
          }
        }
      }
      // ici position la pwm en sortie
      analogWrite ( PosLed[Led] , PowerLed[Led] );
    }
  }
}

