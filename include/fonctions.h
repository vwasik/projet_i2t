double cherche_min(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen);
double cherche_max(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen);

IplImage* image_seuil_glrt(IplImage* img_glrt, double** tableau, double seuil, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen);

double calcul_glrt(IplImage* src, int I, int J, double variance_1,double variance_2, double variance_3, int hauteur_fenetre, int largeur_fenetre);

void ecriture_fichier_histogramme(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen, int nb_bins);