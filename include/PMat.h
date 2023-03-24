/*=============================================================*/
/*= E.Incerti (incerti@univ-mlv.fr)                           =*/
/*= M2/IMAC3 - ANIMATION / MODELES PHYSIQUES                  =*/
/*= - Système Masses/Ressort en dimension 3 : module matériel =*/
/*=============================================================*/

/*°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
  Intégration Numérique par Schéma Leapfrog
  -----------------------------------------
  l'état d'une particule est donné par
  pos : position courante,
  vit : vitesse courante : [1] v(t) = (p(t+1)-p(t))/dt   (implicite)
  frc : buffer de force  : [2] a(t) = f(t)/m = (v(t)-v(t-1))/dt (explicite)

  -> algo d'intégration :
  [2] => v(t+1)= v(t) + h*f(t)/m => vit <- vit + h*frc/m
  [1] => p(t+1)= p(t) + h*v(t+1) => pos <- pos + h*vit
°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°*/

#ifndef  _PMAT_
  #define _PMAT_

  /* identificateur de types                                                              */
  /* Masses Libres : distinguer les masses de surface des masses internes                 */
  #define _MASS  0
  /* on distingue les points fixes classiques (simple masse infinie, pontuelles)...       */
  #define _FIXE  1

  #ifndef _BOOL_
    #define _BOOL_
    #ifndef __cplusplus
      typedef enum {false,true} bool;
    #endif
  #endif

  typedef struct
  {
    double x,y,z;
  } Point, Vector;

  #define Vzero (Vector){0.,0.,0.}

  typedef struct _pmat
  {
    int    type;       /* type pour usages divers             */
   /*---------------------------------------------------------*/
    double m;          /* paramètre de masse ET rayon dans certains cas */
    Point  pos;        /* position courante                   */
    Vector vit;        /* vitesse  courante                   */
    Vector frc;        /* buffer de force                     */
    void (*setup)(struct _pmat*);  /* algorithme associé      */
   /*---------------------------------------------------------*/
  } PMat;

/*! pas d'échantillonnage : 1/F_ECH - définie par le Simulateur !*/
/*! pas très propre, mais pratique                              !*/
extern double h;

/*!---------------------------------------------------------------------------------------------!*/
/*!                                         ALGORITHMES                                         !*/
/*!---------------------------------------------------------------------------------------------!*/
/*! Algorithme de la masse libre 2D (2° loi de Newton)     !*/
/*! intégration : leapfrog                                 !*/
static void LeapFrog(PMat* M)
{
  /* 1° intégration : vitesse */
  M->vit.x += h*M->frc.x/M->m;
  M->vit.y += h*M->frc.y/M->m;
  M->vit.z += h*M->frc.z/M->m;
  /* 2° intégration : position */
  M->pos.x += h*M->vit.x;
  M->pos.y += h*M->vit.y;
  M->pos.z += h*M->vit.z;
  /* après la mise à jour de l'état, on vide le buffer de force     */
  M->frc = Vzero;
}

/*! Algorithme du point fixe (position constante) !*/
static void AlgoFixe(PMat* M)
{
  /* ne fait rien, à part vider le buffer de force */
  M->frc = Vzero;
}

/*!---------------------------------------------------------------------------------------------!*/
/*!                                        CONSTRUCTEURS                                        !*/
/*!---------------------------------------------------------------------------------------------!*/

/*! Création d'une masse libre !*/
extern void Masse(PMat* M, Point P0, double m)
{
  M->type = _MASS;
  M->setup= &LeapFrog;
  M->pos  = P0;
  M->vit  = Vzero;
  M->frc  = Vzero;
  M->m    = m;
}

/*! Création d'une masse fixe !*/
extern void Fixe(PMat *M, Point P0)
{
  M->type = _FIXE;
  M->setup= &AlgoFixe;
  M->pos  = P0;
  M->vit  = Vzero;
  M->frc  = Vzero;
  M->m    = 0.;
}


#endif
