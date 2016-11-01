#include <EEPROM.h>
#include "arduino.h"
#include "e_codeTool.h"

#define addressCode 0

int16 tmp;

int32 timeEnregistreCode;

int16 ReadCode(void)
{
  int16 ret = EEPROM.read(addressCode);
  ret |= (EEPROM.read(addressCode+1) << 8);

  return (ret);
}

void WriteCode(int16 code)
{
  EEPROM.write (addressCode, code & 0xFF);
  EEPROM.write (addressCode+1, (code & 0xFF00)>>8);
}

int16 InvCode (int16 code)
{
  int16 ret = 0;
  
  ret = ((( code & 0x000F ) << 12 ) |
          (( code & 0x00F0 ) << 4 ) |
          (( code & 0x0F00 ) >> 4 ) |
          (( code & 0xF000 ) >> 12 ));
          
  return ret;
}

// retourn true  le traitement est terminee (bon ou mauvais)
typedef enum { TEST_COD_INV, NEW_CODE } etat_enr_cod_T;
etat_enr_cod_T etatEnrCod = TEST_COD_INV;
void enregistreCodeInit(void)
{
  // Init
  timeEnregistreCode = timeFixMinute (1);
  etatEnrCod = TEST_COD_INV;
  initAcquisitionClavier();
}

State_Acquisition RetourAcquisitionClavier;
boolean enregistreCode (void)
{
  int16 codeLu;
  boolean ret = false;
  if (!TimeElapsed(timeEnregistreCode))
  {
    switch (etatEnrCod){
      case TEST_COD_INV:

         /*Récupération touche clavier*/
         RetourAcquisitionClavier = AcquisitionClavier (&codeLu);
        switch (RetourAcquisitionClavier){
        case CODE_COMPLETE:
          if (codeLu == 0xDCBA)
          {
            etatEnrCod = NEW_CODE;
            ret = false;
          }
          else
          {
            //le code re-entre n'est pas le code inverse => abandon
            IhmIWant = MAUVAIS_CODE;
            ret = true;
          }
        break;
        case WRONG_INPUT:
        case ABORT:
        case TIME_EXPIRED:
          IhmIWant = MAUVAIS_CODE;
          ret = true;
        break;
        default:
          ret = false;
        }
      break;
      case NEW_CODE:
        /*Récupération touche clavier*/
        RetourAcquisitionClavier = AcquisitionClavier (&codeLu);     
        switch ( RetourAcquisitionClavier ) {
          case CODE_COMPLETE:
            if ( codeLu != InvCode(codeLu) )
            {
              WriteCode (codeLu);
              // Verifie que le code s'est bien inscrit en mémoire
              if ( codeLu == ReadCode() )
              {
                IhmIWant = ENREGISTRE_SUCCES;
              }
              else
              {
                IhmIWant = MAUVAIS_CODE;
              }
              ret = true;
            }
            else
            {
              // Code symetrique entree interdit !
              IhmIWant = MAUVAIS_CODE;
              ret = true;
            }
        break;
        case WRONG_INPUT:
        case ABORT:
        case TIME_EXPIRED:
          IhmIWant = MAUVAIS_CODE;
          ret = true;
        break;
        }
      break;
      default:
      break;
    }    
  }
  else
    // temps ecoulé
    ret = true;
   
  return (ret);
}  

