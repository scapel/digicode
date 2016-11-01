/**
Code pour digicode activant un relai
Auteur: Sebastien Capel
Date: 2015

Reprogrammable en tapant l'inverse du code suivi de #, puis ABCD #, puis nouveau code #
Code pour Arduino NANO.
Cabler un usb2serie de l'ordinateur vers la carte RX->TX et TX->RX
Configuration LS de reprogramation : Vitesse 19200 Baud, Bit de données 8, Parite: aucune, Control de Flux Aucune, Bit d'arret 1
Configuration API Arduino (Version API 1.0.6): Type de carte Arduino pro or pro mini (5V 16Mz) Atmega 328, Programmeur USB tiny ISP

*/


#include "types.h"
//#include <Keypad.h>
#include "arduino.h"
#include "acquisitionClavier.h"
#include "switchLed.h"
#include "IHMManagement.h"
#include "e_codeTool.h"
#include "gesPrint.h"

/*Pin sortie relai parte garage*/
int16 porteGarage = 12;//a remplacer par 12 car 13 c'est la led sur la carte arduino.
/*Etat de la fonction principale*/
typedef enum {INIT,CLAVIER,COMMANDE_PORTE_ACTIVE,CODE_ERONNE,ENREGISTRE_CODE,ATTENTE_SANCTION} EtatDigitCode_T;

EtatDigitCode_T etatDigicod = INIT;
EtatDigitCode_T etatDigicodPrec;

//code entre par l'utilisateur
//int16 Code;

void setup(){

  Serial.println("Version2");
  Serial.begin(9600);   //Serial monitor
  Serial.println("Test de clavier V0");
  
  //pin porteGarage
  pinMode (porteGarage,OUTPUT);
  digitalWrite (porteGarage,HIGH);
  
  //
  State_Acquisition RetourAcquisitionClavier;
  
  etatDigicodPrec = INIT;
  
  // initialise le systeme d'acquisition au clavier
  initAcquisitionClavier ();
  // initialise le module LED
  InitSwitchLed();
  
  //Pour debug, a chaque reset, ecrase le code en EEPROM par celui ci.
  //WriteCode(0x4321);
}

int8 cptMauvaisCode;
EtatIHM_T IhmIWant = VEILLE;
//

int32 timeComandeActive;
int32 timeAttente;
int32 timeLargeurPulse;


Clavier_Out_T retourClavier;
Clavier_Out_T retourClavierPrec = INPUT_NOK;

int16 CodeMem,CodeMemInv;
void loop()
{
  int16 CodeSaisie;
  State_Acquisition RetourAcquisitionClavier;

  
  switch (etatDigicod){
    case INIT:
      gesPrint("Phase Principale: Init");

      // initialise le systeme d'acquisition au clavier
      //initAcquisitionClavier ();
      //Recupere le code en eeprom
      CodeMem = ReadCode();
      Serial.println("Recuperation du code:");
      Serial.println(CodeMem,HEX);

      CodeMemInv = InvCode(CodeMem);
      Serial.println("Code Inverse:");
      Serial.println(CodeMemInv,HEX);
      cptMauvaisCode = 0;
      etatDigicod = CLAVIER;
      
      initAcquisitionClavier();
    break;
    case CLAVIER:
      if (etatDigicodPrec != CLAVIER)
      {
        IhmIWant = VEILLE;
        Serial.println("Phase Principale: Clavier");
        etatDigicodPrec = CLAVIER;
      }
      retourClavier = clavier(&CodeSaisie);
      if (retourClavier != retourClavierPrec)
      {
        Serial.println("retour fonction clavier");
        Serial.println(retourClavier);
        retourClavierPrec = retourClavier;
      }
      switch (retourClavier){
        case INPUT_OK:
          //Compare le code
          Serial.println("Code saisie:");
          Serial.println(CodeSaisie);
          Serial.println("Code mem:");
          Serial.println(CodeMem,HEX);
          Serial.println("Code Inv:");
          Serial.println(CodeMemInv,HEX);
          if (CodeSaisie == CodeMem)
          {
            etatDigicod = COMMANDE_PORTE_ACTIVE;
          }
          else if (CodeSaisie == CodeMemInv)
          {
            etatDigicod = ENREGISTRE_CODE;
          }
          else
          {
            etatDigicod = CODE_ERONNE;
          }
        break;
        case INPUT_NOK:
          etatDigicod = CODE_ERONNE;
        break;
        case NO_INPUT:
        break;
        default:
        break;
      }
    break;
    case COMMANDE_PORTE_ACTIVE:
      IhmIWant = COMMANDE_ACTIVE;
      if (etatDigicodPrec != COMMANDE_PORTE_ACTIVE)
      {
        Serial.println("Phase Principale: Commande porte active");
        timeLargeurPulse = timeFixMiliSeconde(1000);
        timeComandeActive = timeFixSeconde(20);
        /*Commute le relai*/
        digitalWrite (porteGarage,LOW);
        etatDigicodPrec = COMMANDE_PORTE_ACTIVE;
      } 
      else
      {
        if (TimeElapsed(timeLargeurPulse))
        {
          /*fin de la pulse*/
          digitalWrite (porteGarage,HIGH);
        }
          // Fin de la pulse
        if (TimeElapsed(timeComandeActive) == false)
        {
          
          /*Récupération touche clavier*/
          RetourAcquisitionClavier = AcquisitionClavier (&CodeSaisie);
            
          if ( RetourAcquisitionClavier == NEW_KEY_PRESS )
          {
            //redeclanche une pulse
            timeLargeurPulse = timeFixMiliSeconde(1000);
            /*Commute le relai*/
            digitalWrite (porteGarage,LOW);
          }
        }
        else
        {
          //fin de l'étape, reposition l'etat du relai a sa valeur par defaut au cas ou.
          digitalWrite (porteGarage,HIGH);
          etatDigicod = INIT;
        }
      }
    break;
    case CODE_ERONNE:
      if (etatDigicodPrec != CODE_ERONNE)
      {
        Serial.println("Phase Principale: Code errone");
        etatDigicodPrec = CODE_ERONNE;
      }
      IhmIWant = MAUVAIS_CODE;
      cptMauvaisCode++;
      if (cptMauvaisCode >= 3)
        etatDigicod = ATTENTE_SANCTION;
      else
        etatDigicod = CLAVIER;
    break;
    case ENREGISTRE_CODE:
      IhmIWant = ENREGISTRE;
      if (etatDigicodPrec != ENREGISTRE_CODE)
      {
        Serial.println("Phase Principale: Enregistre Code");
        enregistreCodeInit();
        etatDigicodPrec = ENREGISTRE_CODE;
      }
      if ( enregistreCode() )
      {
        etatDigicod = INIT;
      }
    break;
    case ATTENTE_SANCTION:
      IhmIWant = ATTENTE;
      if (etatDigicodPrec != ATTENTE_SANCTION)
      {
        Serial.println("Phase Principale: Attente");
        timeAttente = timeFixMinute(15);
        etatDigicodPrec = ATTENTE_SANCTION;
      }
      else
      {
        if (TimeElapsed(timeAttente))
          etatDigicod = INIT;
      }
    break;
  }

//IHM
  IHMManagementPeriod(IhmIWant);
  SwitchLedPeriod();

  
}


