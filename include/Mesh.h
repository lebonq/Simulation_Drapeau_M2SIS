#ifndef  _MESH_
  #define _MESH_

typedef struct _node {
    int data;
    struct _node* next;
}Node;

typedef struct _collisionMesh {
    int nx;           // Nombre de cellules en x
    int ny;           // Nombre de cellules en y
    int nz;           // Nombre de cellules en z
    double min;
    double max;
    double cellsize;  // Taille de la cellule
    Node **particles;  // Tableau 2D des particules contenues dans chaque cellule (tableau de listes chaînées)
    int *nb_particules;
}Mesh;

void printList(Node* head) {
    Node *temp = head;
    while (temp != NULL) {
        printf("%d ", temp->data);
        temp = temp->next;
    }
}


void addNode(Mesh* mesh,int pos_node, int data) {
    Node *head = mesh->particles[pos_node];
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        //printf("should call at start\n");
    } else {
        Node *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
        //printf("should not be call at start\n");
    }
    mesh->particles[pos_node] = head;
    mesh->nb_particules[pos_node]++;
    //printList(head);
}

void removeNode(Mesh* mesh,int pos_node, int data) {
    Node *head = mesh->particles[pos_node];
    Node *temp = head;
    Node *prev = NULL;

    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (temp != NULL) {
        if (prev == NULL) {
            head = temp->next;
        } else {
            prev->next = temp->next;
        }
        free(temp);
    }
    mesh->particles[pos_node] = head;

    mesh->nb_particules[pos_node]--;
}

static int convert_to_1d(double x, double y, double z) {
    //id outside -15 15 dont treat it
    if (x < -15 || x > 15 || y < -15 || y > 15 || z < -15 || z > 15) {
        return -1;
    }
    double cell_size = 0.15f;
    int cell_x = (int)floor((x + 15) / cell_size);
    int cell_y = (int)floor((y + 15) / cell_size);
    int cell_z = (int)floor((z + 15) / cell_size);
    int c = cell_x + cell_y * 200 + cell_z * 200 * 200;
    return c;
}

static void initMesh(Mesh *mesh, double cellSize, double coord_min, double coord_max) {
    mesh->cellsize = cellSize;
    mesh->min = coord_min;
    mesh->max = coord_max;
    mesh->nx = (int)ceil((mesh->max - mesh->min) / mesh->cellsize);
    mesh->ny = mesh->nx;
    mesh->nz = mesh->nx;
    mesh->particles = (Node**)calloc(mesh->nx * mesh->ny * mesh->nz, sizeof(Node*));
    //fill particules with null
    memset(mesh->particles, 0, mesh->nx * mesh->ny * mesh->nz * sizeof(Node*));

    mesh->nb_particules = (int*)calloc(mesh->nx * mesh->ny * mesh->nz, sizeof(int));
}

#endif
