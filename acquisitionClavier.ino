#include <Keypad.h>

#include "Arduino.h"
#include "types.h"

#include "acquisitionClavier.h"

// 10s en mms
#define DIX_SECONDES 10000
#define ROWS 4
#define COLS 4
// fonction local
byte Conv (char Key);


/***********/
/* CLAVIER */
/***********/
const char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

//Brancher le clavier sur 2 3 4 5 (colonnes) et 6 7 8 9 (lignes)
byte rowPins[ROWS] = {9, 8, 7, 6}; //Lignes
byte colPins[COLS] = {5, 4, 3, 2}; //Colonnes
// Connections des touches
/*
S1  contact 4 8
S2  contact 3 8
S3  contact 2 8
S4  contact 1 8
S5  contact 4 7
S6  contact 3 7
S9  contact 4 6
S10  contact 3 6
S11  contact 2 6
S12  contact 1 6
S13  contact 4 5
S14  contact 3 5
S15  contact 2 5
S16  contact  1 5
*/

// Initialiser une instance de la classe keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//variable globale du module
const int8 K_NB_TOUCHE = 4; // code a 4 chiffres
int8 NbTouche = 0; // Pour repérer le premier appuie sur le clavier
int8 NbCharEntree = 0; // nombre de chiffre entree pour le code

int32 Time_End_Input;
int16 Code = 0; // le code! (4 chars: la premiere touche allant se loger dans l'octet de poids fort)
// Fonction d'initialisation du module
void initAcquisitionClavier (void)
{

  Serial.println("Initialisation du module de saisie clavier");
  
    // Pour activer l'état HOLD
  unsigned int time_hold = 4;
  keypad.setHoldTime(time_hold);
  
  //Anti rebond
  unsigned int time_anti_rebond = 4;  //4 ms
  keypad.setDebounceTime(time_anti_rebond);
  
  //Initialisation des variables du module
  NbTouche = 0;
  NbCharEntree = 0;
  Code = 0;
}


// Fonciton principale d'acquisition clavier
State_Acquisition AcquisitionClavier ( int16 *Code_Out)
{
  
  //initialise le status de la fonction
  State_Acquisition Ret = NOTHING_APPEND;
    
  // horloge globale en ms
  int32 Time_Running = millis();
  
  // recuper l'appuie ou pas 
  char Key = keypad.getKey();
  
  // controle que le temps passe pour entree le code ne soit pas expire
  if ( NbTouche > 0 )
  {
    // une action est en cours
    Ret = CODE_ENTERING;
    
    if ( Time_Running > Time_End_Input )
    {
      initAcquisitionClavier();
      return (TIME_EXPIRED);
    }
  }
  
  // control si une touche etait presse
  if (Key != NO_KEY)
  {
    // Haaa une touche d'appuyée !
    NbTouche++;
    Ret = NEW_KEY_PRESS;
    
    // initialise le temps d'expiration de la saisie au clavier (10s)
    if (NbTouche == 1)
      Time_End_Input = Time_Running + DIX_SECONDES;

    // test si l'operateur abandonne la saisie    
    if (Key == '*')
    {
      initAcquisitionClavier();
      return (ABORT);
    }
    
    if (Key == '#')
    {
      if (NbCharEntree == 4)
        Ret = CODE_COMPLETE;
      else
        Ret = WRONG_INPUT;
      *Code_Out = Code;
      initAcquisitionClavier();
      return (Ret);
    }
    
    // Constitue le code   
    Code |= (int16)(Conv ( Key )) << ( 4 * NbCharEntree );
    Serial.println(Code);
    // nouveau charactere entree
    NbCharEntree ++;
    
    if ( NbCharEntree > K_NB_TOUCHE )
    {
      initAcquisitionClavier();
      Ret = WRONG_INPUT;
    }
  }
  
  return (Ret);  
}

// convertir de char en int8
byte Conv (char Key)
{
  byte ret;
  
  //Serial.println( "touche appuye");
  //Serial.println(Key);
  if ( Key >= '0' && Key <= '9' )
    ret = ( (int8)Key - (int8)'0' );
  else
    if ( Key >= 'A' && Key <= 'D' )
      ret = ( (int8)Key - (int8)'A' + 10 );
    else
      Serial.println( "ERROR!");
  //Serial.println( "Convertie en");
  //Serial.println(ret);
  
  return (ret);
}

State_Acquisition RetourAcquisitionPrec = CODE_COMPLETE;    

Clavier_Out_T clavier (int16 *Code_Out)
{  
  Clavier_Out_T ret;
  State_Acquisition RetourAcquisition;
  
  ret = NO_INPUT;
  
  RetourAcquisition = AcquisitionClavier (Code_Out);
  if (RetourAcquisition != RetourAcquisitionPrec)
  {
    RetourAcquisitionPrec = RetourAcquisition;
    Serial.println ("RetourAcquisition");
    //CODE_COMPLETE, CODE_ENTERING, NEW_KEY_PRESS , WRONG_INPUT , ABORT ,NOTHING_APPEND , TIME_EXPIRED
    switch (RetourAcquisition){
      case CODE_COMPLETE:
        Serial.println ("CODE_COMPLETE");
        Serial.println ("Code");
        Serial.println (*Code_Out,HEX);
        // 
        ret = INPUT_OK;
      break;
      case CODE_ENTERING:
        IhmIWant = SAISI_EN_COURS;
        Serial.println ("CODE_ENTERING");
      break;
      case NEW_KEY_PRESS:
        IhmIWant = NOUVELLE_TOUCHE;
        Serial.println ("NEW_KEY_PRESS");
      break;
      case WRONG_INPUT:
        Serial.println ("WRONG_INPUT");
        IhmIWant = MAUVAIS_CODE;
        ret = INPUT_NOK;
      break;
      case ABORT:
        Serial.println ("ABORT");
        //SwitchLed (false,false,LEDS_ROUGES);
        //SwitchLed (false,true,LEDS_BLEUS);
        
      break;
      case NOTHING_APPEND:
        //IhmIWant = ATTENTE;
        IhmIWant = VEILLE;
        Serial.println ("NOTHING_APPEND");
      break;
      case TIME_EXPIRED:
        Serial.println ("TIME_EXPIRED");
        ret = INPUT_NOK;
      break;
      default:
      break;
    }    
  }
  return (ret);
}

