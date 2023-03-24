#ifndef  _LINK_
  #define _LINK_

  #include <PMat.h>

  #define F_CONST 0
  #define F_RESS  1
  #define F_FREIN 2
  #define F_RESS_FREIN 3

  typedef struct _link
  {
    int       type;
    double  k,z,l,s;
    Vector  frc;  
    PMat   *M1,*M2;
    void  (*setup)(struct _link*);
    bool    on_off;     // flag actif/inactif pour rupture ou suppression
  }
  Link;


extern double h;

static double dist (Point A, Point B) { return sqrt((B.x-A.x)*(B.x-A.x)+(B.y-A.y)*(B.y-A.y)+(B.z-A.z)*(B.z-A.z)); }


static void AlgoFrcConst(Link* L)
{
  PMat* M=(L->M1?L->M1:L->M2);
  M->frc.x += L->frc.x;
  M->frc.y += L->frc.y;
  M->frc.z += L->frc.z;
  if (!L->M2) return;
  while (++M<=L->M2)
  {
    M->frc.x += L->frc.x;
    M->frc.y += L->frc.y;
    M->frc.z += L->frc.z;
  }
}

static void AlgoRessort(Link* L)
{
  double d;
  double e; 
  double f; 


  d = dist(L->M1->pos,L->M2->pos);
  e = (d==0.?1.:(1.-L->l/d));

  f = L->k*e*(L->M2->pos.x-L->M1->pos.x);
  L->M1->frc.x += f;
  L->M2->frc.x -= f;

  f = L->k*e*(L->M2->pos.y-L->M1->pos.y);
  L->M1->frc.y += f;
  L->M2->frc.y -= f;

  f = L->k*e*(L->M2->pos.z-L->M1->pos.z);
  L->M1->frc.z += f;
  L->M2->frc.z -= f;
}


static void AlgoFrein(Link* L)
{
  double f;

  f = L->z*(L->M2->vit.x-L->M1->vit.x);
  L->M1->frc.x += f;
  L->M2->frc.x -= f;

  f = L->z*(L->M2->vit.y-L->M1->vit.y);
  L->M1->frc.y += f;
  L->M2->frc.y -= f;

  f = L->z*(L->M2->vit.z-L->M1->vit.z);
  L->M1->frc.z += f;
  L->M2->frc.z -= f;
}

static void AlgoRessortFrein(Link* L)
{
  double d; // distance 
  double e; // Elongation 
  double f; // force

  d = dist(L->M1->pos,L->M2->pos);
 
  if (d>(L->s*L->l) && L->M1->type != _FIXE && L->M2->type != _FIXE) {L->on_off = false; return;}
  
  e = (d==0.?1.:(1.-L->l/d));

  f = L->k*e*(L->M2->pos.x-L->M1->pos.x) + L->z*(L->M2->vit.x-L->M1->vit.x);
  L->M1->frc.x += f;
  L->M2->frc.x -= f;

  f = L->k*e*(L->M2->pos.y-L->M1->pos.y) + L->z*(L->M2->vit.y-L->M1->vit.y);
  L->M1->frc.y += f;
  L->M2->frc.y -= f;

  f = L->k*e*(L->M2->pos.z-L->M1->pos.z) + L->z*(L->M2->vit.z-L->M1->vit.z);
  L->M1->frc.z += f;
  L->M2->frc.z -= f;
}

extern void FrcConst(Link* L, Vector frc)
{
  L->type   = F_CONST;
  L->setup  = &AlgoFrcConst;
  L->frc    = frc;
  L->k      = 0.;
  L->z      = 0.;
  L->s      = 0.;
  L->l      = 0.;
  L->on_off = true;
}

extern void Ressort(Link* L, double k)
{
  L->type   = F_RESS;
  L->setup  = &AlgoRessort;
  L->k      = k ;
  L->z      = 0.;
  L->s      = 0.;
  L->l      = 0.;
  L->on_off = true;
}

extern void Frein(Link* L, double z)
{
  L->type   = F_FREIN;
  L->setup  = &AlgoFrein;
  L->k      = 0.;
  L->z      = z ;
  L->s      = 0.;
  L->on_off = true;
}

extern void RessortFrein(Link* L, double k, double z,double s)
{
  L->type   = F_RESS_FREIN;
  L->setup  = &AlgoRessortFrein;
  L->k      = k ;
  L->z      = z ;
  L->s      = s;
  L->l      = 0.;
  L->on_off = true;
}


extern void Connect(PMat *M1, Link *L, PMat *M2)
{
  if(L->type==F_CONST)
  {
    L->M1=M1;
    L->M2=M2;
    return;
  }
  L->l = dist(M1->pos,M2->pos);
  L->M1=M1;
  L->M2=M2;
}

#endif
