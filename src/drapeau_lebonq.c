/*=============================================================*/
/*= E.Incerti (incerti@univ-eiffel.fr)                        =*/
/*= M2/IMAC3 - ANIMATION / MOTEUR PHYSIQUE                    =*/
/*= Modèle de corde - masses soumises à la gravité            =*/
/*=============================================================*/
#include <g3x.h>

#include <Link.h>
#include <PMat.h>
#include <Mesh.h>

static double wxmin = -10., wymin = -10., wxmax = +10., wymax = +10.;

static int Fe, Fa = 5;
static double k, z;
static double kg, zg;
static double kt, zt;
static double kc, zc;
static double m, r, s;
static double alpha,beta; //factor for torssion and courbure
static int weakness_number;

static int nbm = 0, nblgeo = 0, nblgrav = 0, nbltor = 0, nblcourb = 0, nblwind = 0, nblauto = 0, width = 50, height = 40;
int w_x, h_z;
static PMat *Mtab = NULL;
static Link *LGeometrieLink = NULL;
static Link *LTorssionLink = NULL;
static Link *LCourbureLink = NULL;
static Link *LGravitytab = NULL;
static Link *LWindtab = NULL;
static Link *LAutoCollide = NULL;
static Mesh mesh;
double h;

double wind_factor = 3;
Vector wind = {10., 3., 1.};
struct timeval tv;
bool autocolision = false;

void reset() {

  for (int i = 0; i < width * height; i++) {
    int x = i % width;
    int z_pos = i / width;
    Point p = {x * r, 0.1, z_pos * r};

    (Mtab + i)->pos = p;
    (Mtab + i)->vit = (Vector){0., 0., 0.};
    (Mtab + i)->frc = (Vector){0., 0., 0.};
  }

  //Reconstruct broken links
  for (int i = 0; i < nblgeo; i++) {
    (LGeometrieLink + i)->on_off = true;
  }

  for (int i = 0; i < nbltor; i++) {
    (LTorssionLink + i)->on_off = true;
  }

  for (int i = 0; i < nblcourb; i++) {
    (LCourbureLink + i)->on_off = true;
  }

  //Random seed
  struct timeval tv_random;
  gettimeofday(&tv_random, NULL);
  srand(tv_random.tv_usec);

  //Apply weakness
  for (int i = 0; i < weakness_number; i++) {
    int r = rand() % nblgeo;
    LGeometrieLink[r].on_off = false;
  }
  for (int i = 0; i < weakness_number; i++) {
    int r = rand() % nbltor;
    LTorssionLink[r].on_off = false;
  }
  for (int i = 0; i < weakness_number; i++) {
    int r = rand() % nblcourb;
    LCourbureLink[r].on_off = false;
  } 
}

void auto_col(){
  autocolision = !autocolision;
}

void init(void) {
  nbm = width * height;
  nblgeo = nbm * 2 - width - height;
  nbltor = nbm * 2 - width*2 - height*2 +2;
  nblcourb = nbm *2 - width*2 - height*2 - 4;
  nblgrav = nbm;
  nblwind = nbm;
  nblauto = nbm*(nbm-1)/2;

  if (!(Mtab = (PMat *)calloc(nbm, sizeof(PMat))))
    exit(1);
  if (!(LGeometrieLink = (Link *)calloc(nblgeo, sizeof(Link))))
    exit(1);
  if (!(LTorssionLink = (Link *)calloc(nbltor, sizeof(Link))))
    exit(1);
  if (!(LCourbureLink = (Link *)calloc(nblcourb, sizeof(Link))))
    exit(1);
  if (!(LGravitytab = (Link *)calloc(nblgrav, sizeof(Link))))
    exit(1);
  if (!(LWindtab = (Link *)calloc(nblwind, sizeof(Link))))
    exit(1);
  if (!(LAutoCollide = (Link *)calloc(nblauto, sizeof(Link))))
    exit(1);

  m = 10;
  r = 0.25;// * (wxmax - wxmin) / (nbm - 1);

  initMesh(&mesh,0.15,-15.0,15.0);

  printf("Size mesh x = %d, y = %d, z = %d\n", mesh.nx, mesh.ny, mesh.nz);

  // def des masses les w premieres sont fixes et les idx%w==0 aussi
  for (int i = 0; i < width * height; i++) {
    int x = i % width;
    int z_pos = i / width;
    Point p = {x * r, 0., z_pos * r};

    //Higher x is, lower mass is
    if(x < width/4){
      m = 10;
    }
    else if(x < width/2){
      m = 7.5;
    }
    else if(x < width*3/4){
      m = 5;
    }
    else{
      m = 2.5;
    }

    int pos_mesh = convert_to_1d(p.x,p.y,p.z);

    if (i % width == 0){
      Fixe(Mtab + i, p,pos_mesh,i);}
    else{
      Masse(Mtab + i, p,m,pos_mesh,i);//x*((1.0-6.0)/(width-1))+6.0);
      //printf("x = %d , m = %f\n",x,  x*((1.0-6.0)/(width-1))+6.0);
    }

    addNode(&mesh,pos_mesh,i);

  }


  Fe = 9100; //9100
  h = 1. / Fe;

  k = 0.410; //0.410 or 0.510
  z = 0.010; //0.010
  alpha = 0.35; // 0.35
  beta = 0.58; // 0.58
  s = 3;

  kg = k * m * Fe * Fe;
  zg = z * m * Fe;

  kt = beta * kg;
  zt = beta * zg;

  kc = alpha * kg;
  zc = alpha * zg;

  Link *L = LGeometrieLink;
  PMat *M = Mtab;
  //Tenseur de geometrie
  for (int i = 0; i < nbm; i++) {
    int x = i % width;
    int z_pos = i / width;
    if (x + 1 < width) {
      RessortFrein(L, kg, zg, s);
      Connect(M, L, M + 1);
      L++;
    }
    if (z_pos + 1 < height) {
      RessortFrein(L, kg, zg, s);
      Connect(M, L, M + width);
      L++;
    }
    M++;
  }

  L = LTorssionLink;
  M = Mtab;
  //Tenseur de torssion
  for (int i = 0; i < nbm; i++) {
    int x = i % width;
    int z_pos = i / width;

    if (x + 1 < width && z_pos + 1 < height) {
      RessortFrein(L, kt, zt, s);
      Connect(M, L, M + width + 1);
      L++;
    }
    if (x + 1 < width && z_pos - 1 >= 0) {
      RessortFrein(L, kt, zt, s);
      Connect(M, L, M - width + 1);
      L++;
    }
    M++;
  }

  L = LCourbureLink;
  M = Mtab;
  //Tenseur de courbure
  for (int i = 0; i < nbm; i++) {
    int x = i % width;
    int z_pos = i / width;

    if (x + 2 < width) {
      RessortFrein(L, kc, zc, s);
      Connect(M, L, M + 2);
      L++;
    }
    if (z_pos + 2 < height) {
      RessortFrein(L, kc, zc, s);
      Connect(M, L, M + width * 2);
      L++;
    }
    M++;
  }

  L = LGravitytab;
  for (M = Mtab; M < Mtab + nblgrav; M++) {
    FrcConst(L, (Vector){0., 0., -2.5 * Fe});
    Connect(M, L, NULL);
    L++;
  }

  L = LWindtab;
  for (M = Mtab; M < Mtab + nblwind; M++) {
    FrcConst(L, wind);
    Connect(M, L, NULL);
    L++;
  }

  L = LAutoCollide;
  M = Mtab;
  int linkIndex = 0;
  for (int i = 0; i < nbm; i++) {
    for (int j = i + 1; j < nbm; j++) {
      RessortFreinSeuil(L,0.5,0.12,s);
      Connect(M+i, L, M + j);
      //printf("Link %d created between particles %d and %d\n", linkIndex, i, j);
      L++;
      linkIndex++;
    }

  }



  //Put to false random link in 3 tabs
  weakness_number = 0;
  for (int i = 0; i < weakness_number; i++) {
    int r = rand() % nblgeo;
    LGeometrieLink[r].on_off = false;
  }
  for (int i = 0; i < weakness_number; i++) {
    int r = rand() % nbltor;
    LTorssionLink[r].on_off = false;
  }
  for (int i = 0; i < weakness_number; i++) {
    int r = rand() % nblcourb;
    LCourbureLink[r].on_off = false;
  } 

}

void ctrl(void) {
  /* les paramètres réglables */
  g3x_CreateScrollv_i("Fa", &Fa, 1, 20, 1, "Fa : frequence d'affichage");
  g3x_CreateScrollv_i("Fe", &Fe, 100, 10000, 1, "Fe : frequence de calcul");
  g3x_CreateScrollv_i("Cassure", &weakness_number, 0,   nbm * 2 - width - height, 1, "Nombre de liens cassés");
  g3x_CreateScrollv_d("Wind", &wind_factor, 0., 10., 1, "Force du vent");
  g3x_CreateScrollv_d("Seuil", &s, 1.1, 10., 1, "Seuil de dechirement");
  
  g3x_CreateScrollh_d("z ", &z, 0.00, 2., 1, "z  : viscosite interne  ");
  g3x_CreateScrollh_d("k ", &k, 0.01, 2., 1, "k  : raideur   interne  ");
  g3x_CreateScrollh_d("alpha ", &alpha, 0.25, 1., 1, "alpha  : facteur pour le calcul de la force de courbure ");
  g3x_CreateScrollh_d("beta ", &beta, 0.50, 1., 1, "beta  : facteur pour le calcul de la force de torsion ");
  g3x_CreatePopUp("reset", reset, "reset des positions");
  g3x_CreatePopUp("Auto-Colli", auto_col, "Activation de l'autocollision");
}

void draw(void) {
  Link *L;

  h = 1. / Fe;

  kg = k * m * Fe * Fe;
  zg = z * m * Fe;

  kt = beta * kg;
  zt = beta * zg;

  kc = alpha * kg;
  zc = alpha * zg;

  
  for (L = LGeometrieLink; L < LGeometrieLink + nblgeo; L++) {
    L->s = s;
    if(L->on_off == false) continue;
    L->k = kg;
    L->z = zg;

    double vit_x = fabs(L->M1->vit.x)+30;
    double vit_y = fabs(L->M1->vit.y)+30;
    double vit_z = fabs(L->M1->vit.z)+30;

    g3x_Material((G3Xcolor){vit_x/60,vit_y/60,vit_z/60,0.00}, 0.2, 0.6, 0.9, 0.999, .0);

    if(L->M2 != NULL && L->M1 != NULL) {
      glBegin(GL_LINES);
      glVertex3f(L->M1->pos.x, L->M1->pos.y, L->M1->pos.z);
      glVertex3f(L->M2->pos.x, L->M2->pos.y, L->M2->pos.z);
      glEnd();
    }
  }

  for (L = LTorssionLink; L < LTorssionLink + nbltor; L++) {
    L->s = s;
    if(L->on_off == false) continue;

    L->k = kt;
    L->z = zt;

    double vit_x = fabs(L->M1->vit.x)+30;
    double vit_y = fabs(L->M1->vit.y)+30;
    double vit_z = fabs(L->M1->vit.z)+30;

    g3x_Material((G3Xcolor){vit_x/60,vit_y/60,vit_z/60,0.00}, 0.2, 0.6, 0.9, 0.999, .0);
    if(L->M2 != NULL && L->M1 != NULL) {
      glBegin(GL_LINES);
      glVertex3f(L->M1->pos.x, L->M1->pos.y, L->M1->pos.z);
      glVertex3f(L->M2->pos.x, L->M2->pos.y, L->M2->pos.z);
      glEnd();
    }
  }

  for (L = LCourbureLink; L < LCourbureLink + nblcourb; L++) {
    L->s = s;
    if(L->on_off == false) continue;

    L->k = kc;
    L->z = zc;

    double vit_x = fabs(L->M1->vit.x)+30;
    double vit_y = fabs(L->M1->vit.y)+30;
    double vit_z = fabs(L->M1->vit.z)+30;

    g3x_Material((G3Xcolor){vit_x/60,vit_y/60,vit_z/60,0.00}, 0.2, 0.6, 0.9, 0.999, .0);
    if(L->M2 != NULL && L->M1 != NULL) {
      glBegin(GL_LINES);
      glVertex3f(L->M1->pos.x, L->M1->pos.y, L->M1->pos.z);
      glVertex3f(L->M2->pos.x, L->M2->pos.y, L->M2->pos.z);
      glEnd();
    }

    //We dont show them not to overload the screen
  }

  

  g3x_SetRefreshFreq(Fa);
}

void anim(void) {

  Link *L;
  for (L = LGeometrieLink; L < LGeometrieLink + nblgeo; L++) {
    if(L->on_off == false) continue;
    L->setup(L);
  }
  for (L = LTorssionLink; L < LTorssionLink + nbltor; L++) {
    if(L->on_off == false) continue;
    L->setup(L);
  }
  for (L = LCourbureLink; L < LCourbureLink + nblcourb; L++) {
    if(L->on_off == false) continue;
    L->setup(L);
  }
  for (L = LGravitytab; L < LGravitytab + nblgrav; L++) {
    L->setup(L);
  }
  for (L = LWindtab; L < LWindtab + nblwind; L++) {
    L->setup(L);
  }

  struct timeval cr_tv;
  gettimeofday(&cr_tv, NULL);
  //print both time and difference
  if(cr_tv.tv_sec - tv.tv_sec > 4l) { //Tout les x secondes
    gettimeofday(&tv, NULL);
    //compute a random wind
    wind.x = ((rand() % 20)/10.0 )* wind_factor * Fe;// Small wind on x to allow us ti have a straight flag
    wind.y = ((rand() % 5)/10.0 )* wind_factor* Fe; 
    wind.z = ((rand() % 5)/10.0 )* wind_factor * Fe; //Small wind on z
    //rand 1 or -1
    wind.x *= (rand() % 2) ? 1 : -1;
    wind.y *= (rand() % 2) ? 1 : -1;
    wind.z *= (rand() % 2) ? 1 : -1;

    for (L = LWindtab; L < LWindtab + nblwind; L++) {
      L->frc = wind;
    }
    printf("Wind : x => %f, y => %f, Z => %f\n", wind.x/Fe, wind.y/Fe, wind.z/Fe);    
  }

  PMat *M;
  int pos_mesh;
  for (M = Mtab; M < Mtab + nbm; M++) {
    pos_mesh = M->pos_mesh;
    M->setup(M);
    if(pos_mesh != M->pos_mesh) { // Si le position de la masse a change dans le mesh alors on update la list du mesh
      removeNode(&mesh,pos_mesh, M->id_part); // On supprime l'ancienne position
      addNode(&mesh,M->pos_mesh, M->id_part); // On ajoute la nouvelle position
    }
  }

  if(autocolision == true) {

  //for in particles of mesh
  for(int i = 0; i < mesh.nx * mesh.ny * mesh.nz; i++) {
    if(mesh.nb_particules[i] >= 2){
      //printf("mesh[%d] = %d\n", i, mesh.nb_particules[i]);
      //printList(mesh.particles[i]); 

      int m1 = mesh.particles[i]->data;
      int m2 = mesh.particles[i]->next->data;
      int offset = 0;

    if (m1 < m2) {
        offset = (m1*nbm) - ((m1*(m1+1))/2) + (m2-m1-1);
    } else {
        offset = (m2*nbm) - ((m2*(m2+1))/2) + (m1-m2-1);
    }
      L = LAutoCollide + offset;
      L->setup(L);
    }
  }}

}

void quit(void) {
  free(Mtab);
  free(LGeometrieLink);
  free(LTorssionLink);
  free(LCourbureLink);
  free(LGravitytab);
  free(LWindtab);
}

int main(int argc, char *argv[]) {
  w_x = 3 * width, h_z = 3 * height;
  int pixwidth = (wxmax - wxmin) * 30, pixheight = (wymax - wymin) * 30;
  g3x_InitWindow(*argv, pixwidth, pixheight);

  //get time in sec

  gettimeofday(&tv, NULL);

  g3x_SetCameraCartesian((G3Xpoint){38.710,46.752,13.405},(G3Xpoint){0.0,0.0,2.5});
  g3x_SetInitFunction(init);
  g3x_SetCtrlFunction(ctrl);
  g3x_SetDrawFunction(draw);
  g3x_SetAnimFunction(anim);
  g3x_SetExitFunction(quit);

  return g3x_MainStart();
}
