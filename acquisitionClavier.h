#include "types.h"

#ifndef ACQUISITIONCLAVIER_H

#define ACQUISITIONCLAVIER_H

// retour de la fonction principale
typedef enum { CODE_COMPLETE, CODE_ENTERING, NEW_KEY_PRESS , WRONG_INPUT , ABORT ,NOTHING_APPEND , TIME_EXPIRED } State_Acquisition;

/*Sortie de la fonction clavier*/
typedef enum {INPUT_OK,INPUT_NOK,NO_INPUT} Clavier_Out_T;

//
void initAcquisitionClavier (void);
Clavier_Out_T clavier (int16 *Code_Out);

// Fonciton principale d'acquisition clavier
State_Acquisition AcquisitionClavier ( int16 *Code_Out);

#endif
