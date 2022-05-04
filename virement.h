/* struct virement used between server and client */
typedef struct
{
  int num_destinataire;
  int num_expediteur;
  double montant;
} Payment;