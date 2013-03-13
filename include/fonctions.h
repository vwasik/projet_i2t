//double cherche_min(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen);
//double cherche_max(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen);
double cherche_max(double ** tableau, int taille_tableau, int nb_entrees);
double cherche_min(double ** tableau, int taille_tableau, int nb_entrees);

void image_seuil_glrt(IplImage* img_glrt, double** tableau, double seuil, int *compteur, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen);

//double calcul_glrt(IplImage* src, int I, int J, double variance_1,double variance_2, double variance_3, int hauteur_fenetre, int largeur_fenetre);
double calcul_glrt(IplImage* src, int I, int J, double variance_noir,double variance_blanc, int hauteur_fenetre, int largeur_fenetre);


//double calcul_glrt_2(IplImage* src, int I, int J, double variance_1,double variance_2, double variance_3, double variance_4, int hauteur_fenetre, int largeur_fenetre);
double calcul_glrt_2(IplImage* src, int I, int J, double variance_blanc,double variance_noir, double variance_gris, int hauteur_fenetre, int largeur_fenetre);


void ecriture_fichier_histogramme(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen, int nb_bins);

//void image_tableau(IplImage* img_glrt,double** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen);
void image_tableau(IplImage* img_glrt,double** tableau, int taille_tableau, int nb_entrees);

void tableau_coord_bin(IplImage* img_bin, int **tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen);

void histo_reponses(int **tableau, int taille_tableau);
