#include "arduino.h"
#include "IHMManagement.h"

EtatIHM_T EtatIHM = VEILLE;
EtatIHM_T EtatIHMNext,EtatIHMPrec;


//variable veille
boolean B_LedVeille,B_BleuStart;
int32 timeFlashOff,timeFlashON,timeLedBleuStart;


void IHMManagementPeriod (EtatIHM_T EtatIHMWanted)
{
  // initialise etat IHMNext a l'etat actuel
  EtatIHMNext = EtatIHM;
//  Serial.println("Etat IHM:");
//  Serial.println(EtatIHM);  
  switch (EtatIHM)
  {
    case VEILLE:
    case VEILLE_ACTION:

      if (EtatIHMPrec != VEILLE_ACTION)
      {
        // premiere fois que l'on rentre dans ce step
        EtatIHMNext = VEILLE_ACTION;
        LedClignoteOff();
        SwitchLed (false,false,LEDS_ROUGES);
        LedClignoteInit(LEDS_BLEUS,5000,30,false,0);
      }
      
      LedClignote();
      if ( EtatIHMWanted != VEILLE )
        EtatIHMNext = EtatIHMWanted;
    
    break;
    case NOUVELLE_TOUCHE:
    case NOUVELLE_TOUCHE_ACTION:
      if (EtatIHMPrec != NOUVELLE_TOUCHE_ACTION)
      {
        // premiere fois que l'on rentre dans ce step
        EtatIHMNext = NOUVELLE_TOUCHE_ACTION;
        LedNouvelleToucheInit();
      }
      if ( (EtatIHMWanted != NOUVELLE_TOUCHE) && LedNouvelleTouche() )
        EtatIHMNext = EtatIHMWanted;
    break;
    case SAISI_EN_COURS:
    case SAISI_EN_COURS_ACTION:
      if (EtatIHMPrec != SAISI_EN_COURS_ACTION)
      {
        EtatIHMNext = SAISI_EN_COURS_ACTION;
        LedSaisieEnCours();
      }
      if ( EtatIHMWanted != SAISI_EN_COURS)
        EtatIHMNext = EtatIHMWanted;
    break;
    case MAUVAIS_CODE:
    case MAUVAIS_CODE_ACTION:
      if (EtatIHMPrec != MAUVAIS_CODE_ACTION)
      {
        // premiere fois que l'on rentre dans ce step
        EtatIHMNext = MAUVAIS_CODE_ACTION;
        SwitchLed (false,false,LEDS_BLEUS);
        LedClignoteOff();
        LedClignoteInit(LEDS_ROUGES,50,50,false,10);
      }
      if ( LedClignote() && (EtatIHMWanted != MAUVAIS_CODE) )
        EtatIHMNext = EtatIHMWanted;
    break;
    case ATTENTE:
    case ATTENTE_ACTION:
      if (EtatIHMPrec != ATTENTE_ACTION)
      {
        // premiere fois que l'on rentre dans ce step
        EtatIHMNext = ATTENTE_ACTION;
        LedClignoteOff();
        //(Leds_T led,int32 dureeLedOffMili,int32 dureeLedOnMili,boolean fade, int8 nbClignoteArg)
        LedClignoteInit(LEDS_ROUGES, 1500 , 2000, true, 0);
      }
      LedClignote();
      if ( EtatIHMWanted != ATTENTE ) 
        EtatIHMNext = EtatIHMWanted;
    break;
    case COMMANDE_ACTIVE:
    case COMMANDE_ACTIVE_ACTION:
      if (EtatIHMPrec != COMMANDE_ACTIVE_ACTION)
      {
        // premiere fois que l'on rentre dans ce step
        EtatIHMNext = COMMANDE_ACTIVE_ACTION;
        LedClignoteOff();
        //(Leds_T led,int32 dureeLedOffMili,int32 dureeLedOnMili,boolean fade, int8 nbClignoteArg)
        //LedClignoteInit(LEDS_ROUGES, 3000 , 3000, true, 0);
        LedClignoteInit(LEDS_BLEUS, 1000 , 2000, true, 0);
      }
      LedClignote();
      if ( EtatIHMWanted != COMMANDE_ACTIVE ) 
        EtatIHMNext = EtatIHMWanted;
    break;
    case ENREGISTRE:
    case ENREGISTRE_ACTION:
      if (EtatIHMPrec != ENREGISTRE_ACTION)
      {
        // premiere fois que l'on rentre dans ce step
        EtatIHMNext = ENREGISTRE_ACTION;
        LedClignoteOff();
        //(Leds_T led,int32 dureeLedOffMili,int32 dureeLedOnMili,boolean fade, int8 nbClignoteArg)
        LedClignoteInit(LEDS_ROUGES, 5000 , 5000, true, 0);
        timeLedBleuStart = timeFixMiliSeconde ( 5000 );
        B_BleuStart = true;
      }
      if (B_BleuStart && TimeElapsed(timeLedBleuStart))
      {
        LedClignoteInit(LEDS_BLEUS, 5000 , 5000, true, 0);
        B_BleuStart = false;
      }
        
      LedClignote();
      if ( EtatIHMWanted != ENREGISTRE ) 
        EtatIHMNext = EtatIHMWanted;
    break;
    case ENREGISTRE_SUCCES:
    case ENREGISTRE_SUCCES_ACTION:
      if (EtatIHMPrec != ENREGISTRE_SUCCES_ACTION)
      {
        // premiere fois que l'on rentre dans ce step
        EtatIHMNext = ENREGISTRE_SUCCES_ACTION;
        LedClignoteOff();
        //(Leds_T led,int32 dureeLedOffMili,int32 dureeLedOnMili,boolean fade, int8 nbClignoteArg)
        LedClignoteInit(LEDS_BLEUS, 50 , 50, false, 10);
      }
      // led clignote dans la condition car cette derniere remonte true qd le nombre de clignotement a ete atteind
      if ( LedClignote() && (EtatIHMWanted != ENREGISTRE_SUCCES) ) 
        EtatIHMNext = EtatIHMWanted;
    break;
    default:
    break;
  }
  // 
  EtatIHMPrec = EtatIHM;
  EtatIHM = EtatIHMNext;
}

int32 timeLedNouvelleTouche;
// LED NOUVELLE TOUCHE
void LedNouvelleToucheInit(void)
{
  timeLedNouvelleTouche = timeFixMiliSeconde(30);
  //void SwitchLed ( boolean Etat, boolean Fade, Leds_T Led );
  SwitchLed (false,false,LEDS_ROUGES);
  SwitchLed (false,false,LEDS_BLEUS);
}

boolean LedNouvelleTouche(void)
{
  return TimeElapsed(timeLedNouvelleTouche);
}

// LED SAISIE EN COURS
void LedSaisieEnCours()
{
//  Serial.println ("led allumee");
  SwitchLed (true,false,LEDS_BLEUS);
}

// LED CLIGNOTE
int32 dureeLedOff[2],dureeLedOn[2],timeLedOff[2],timeLedOn[2];
boolean B_Led[2];
//Leds_T ledAClignoter;
boolean B_fade[2];
int8 nbClignote[2];
int8 cpt_clignote[2];
boolean B_ledClignote[2];
boolean B_fin;

//Fonction a appeler pour stopper tous les clignotements
void LedClignoteOff(void)
{
  int8 led;
  for (led=0;led<2;led++)
  {
    B_ledClignote[led]=false;
  }
}
//Fonction a appeler pour demarrer un clignotement
void LedClignoteInit(Leds_T led,int32 dureeLedOffMili,int32 dureeLedOnMili,boolean fade, int8 nbClignoteArg)
{
  dureeLedOff[led] = dureeLedOffMili;
  dureeLedOn[led] = dureeLedOnMili;
  timeLedOff[led] = timeFixMiliSeconde(dureeLedOff[led]);
  B_Led[led] = false;
//  ledAClignoter = led;
  B_fade[led] = fade;
  nbClignote[led] = nbClignoteArg;
  cpt_clignote[led] = 0;
  B_ledClignote[led] = true;
  B_fin = false;
}
boolean LedClignote(void)
{
  int8 led;
  for (led=0;led<2;led++)
  {
    if (B_ledClignote[led])
    {
  //Serial.println("ledClignote etat");
  //Serial.println(B_Led);
      if (!B_Led[led])
      {
        if (TimeElapsed(timeLedOff[led]))
        {
//      Serial.println("eteind");
          B_Led[led] = true;
          SwitchLed (B_Led[led],B_fade[led],(Leds_T)led);
          timeLedOn[led] = timeFixMiliSeconde(dureeLedOn[led]);
        }
      }
      else
      {
        if (TimeElapsed(timeLedOn[led]))
        {
//        Serial.println("allume");
          B_Led[led] = false;
          SwitchLed (B_Led[led],B_fade[led],(Leds_T)led);
          timeLedOff[led] = timeFixMiliSeconde(dureeLedOff[led]);
          cpt_clignote[led]++;
        }    
      }
      B_fin |= cpt_clignote[led] > nbClignote[led];
    }
  }
  return (B_fin);
  
}


