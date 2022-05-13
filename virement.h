#ifndef _VIREMENT_H_
#define _VIREMENT_H_
/* struct virement used between server and client */
typedef struct
{
  int num_destinataire;
  int num_expediteur;
  int montant;
} Virement;

#endif
