char correspondance_lettre(int chiffre, char* tableau_alphabet);

double cherche_max(double ** tableau, int taille_tableau, int nb_entrees);
double cherche_min(double ** tableau, int taille_tableau, int nb_entrees);
int cherche_j_max(int** tableau_coord, int taille_tableau);
int cherche_j_min(int** tableau_coord, int taille_tableau);
int cherche_i_max(int** tableau_coord, int taille_tableau);
int cherche_i_min(int** tableau_coord, int taille_tableau);

double calcul_glrt(IplImage* src, int I, int J, double variance_noir,double variance_blanc, int hauteur_fenetre, int largeur_fenetre);
double calcul_glrt_2(IplImage* src, int I, int J, double variance_blanc,double variance_noir, double variance_gris, int hauteur_fenetre, int largeur_fenetre);

void ecriture_fichier_histogramme(double** tableau, int taille_tableau, int nb_entrees_tableau, int nb_bins);

void image_tableau(IplImage* img_glrt,double** tableau, int taille_tableau, int nb_entrees_tableau);
void image_seuil_glrt(IplImage* img_glrt, double** tableau, double seuil, int *compteur, int taille_tableau, int nb_entrees_tableau);
void tableau_coord_bin(IplImage* img_bin, int** tableau, int taille_tableau, int nb_entrees_tableau);

void histo_reponses_i(int** tableau, int taille_tableau, int* histo_i, int taille_histo_i);
void histo_reponses_j(int** tableau, int taille_tableau, int* histo_j, int taille_histo_j,int ordonnee_min,int ordonnee_max);

void classe_i_max(int* histo_i, int taille_histo_i, int* tableau_classe_i, int taille_tableau_classe_i, int i_min);
void classe_j_max(int* histo_j, int taille_histo_j, int** tableau_classe_j, int taille_tableau_classe_j, int nb_entrees_tableau_classe_j, int j_min);

void etiquetage_ordonnees(int k, int** tableau_coord, int* tableau_classe_i, int* ligne_trouvee, int* no_rep, int taille_tableau_coord, int taille_tableau_classe_i);
void etiquetage_abscisses(int k, int** tableau_coord, int* tableau_classe_j, int* colonne_trouvee, int* lettre_trouvee, int taille_tableau_coord, int taille_tableau_classe_j, int nb_entrees_tableau_classe_j);
void etiquetage(int** tableau_coord, int* tableau_classe_i, int** tableau_classe_j, int** tableau_reponses, int taille_tableau_coord, int taille_tableau_classe_i, int taille_tableau_classe_j, int nb_entrees_tableau_classe_j, int taille_tableau_reponses, int nb_entrees_tableau_reponses, char* tableau_alphabet);
