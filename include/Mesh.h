typedef struct _node {
    int data;
    struct _node* next;
}Node;


static void addNode(Node* head, int data) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    } else {
        Node *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

static void removeNode(Node* head, int data) {
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
}


static void printList(Node* head) {
    Node *temp = head;
    while (temp != NULL) {
        printf("%d ", temp->data);
        temp = temp->next;
    }
}

static int convert_to_1d(float x, float y, float z) {
    //id outside -15 15 dont treat it
    if (x < -15 || x > 15 || y < -15 || y > 15 || z < -15 || z > 15) {
        return -1;
    }
    float cell_size = 0.15f;
    int cell_x = (int)floor((x + 15) / cell_size);
    int cell_y = (int)floor((y + 15) / cell_size);
    int cell_z = (int)floor((z + 15) / cell_size);
    int c = cell_x + cell_y * 200 + cell_z * 200 * 200;
    return c;
}

typedef struct _collisionMesh {
    int nx;           // Nombre de cellules en x
    int ny;           // Nombre de cellules en y
    int nz;           // Nombre de cellules en z
    double min;
    double max;
    double cellsize;  // Taille de la cellule
    Node *particles;  // Tableau 2D des particules contenues dans chaque cellule (tableau de listes chaînées)
}Mesh;

static void initMesh(Mesh *mesh, double cellSize, double coord_min, double coord_max) {
    mesh->cellsize = cellSize;
    mesh->min = coord_min;
    mesh->max = coord_max;
    mesh->nx = (int)ceil((mesh->max - mesh->min) / mesh->cellsize);
    mesh->ny = mesh->nx;
    mesh->nz = mesh->nx;
    mesh->particles = (Node*)calloc(mesh->nx * mesh->ny * mesh->nz, sizeof(Node));
}