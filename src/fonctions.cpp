#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cxcore.h>
#include <cvwimage.h>

double cherche_min(double ** tableau, int taille_tableau, int nb_entrees)
{
	double min = tableau[0][0];
	int I,J;
	for(I=0;I<taille_tableau;I++)
        {
          for(J=0;J<nb_entrees;J++)
            {
				if (tableau[I][J] < min){
					min = tableau[I][J];
				}
			}
		}
	
	return min;
}

double cherche_max(double ** tableau, int taille_tableau, int nb_entrees)
{
	double max = tableau[0][0];
	int I,J;
	for(I=0;I<taille_tableau;I++)
        {
          for(J=0;J<nb_entrees;J++)
            {
				if (tableau[I][J] > max){
					max = tableau[I][J];
				}
			}
		}
	
	return max;
}

void image_seuil_glrt(IplImage* img_glrt, double** tableau, double seuil, int *compteur, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen)
{
	int I,J;
	for(I=0;I<hauteur_img-hauteur_fen;I++)
        {
          for(J=0;J<largeur_img-largeur_fen;J++)
            {
				CvScalar pix_glrt = cvGet2D(img_glrt,I,J);
				
				if (tableau[I][J] > seuil){
					pix_glrt.val[0]=0;
					cvSet2D(img_glrt,I,J,pix_glrt);
					*compteur = *compteur +1;
				}
				else{
					pix_glrt.val[0]=255;
					cvSet2D(img_glrt,I,J,pix_glrt);
				}
			}
		}
		
}

void tableau_coord_bin(IplImage* img_bin, int **tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen)
{
	int I,J;
	int ind_tab=0;
	for(I=0;I<hauteur_img-hauteur_fen;I++)
    {
		for(J=0;J<largeur_img-largeur_fen;J++)
        {
			CvScalar pix_glrt = cvGet2D(img_bin,I,J);
				
			if (pix_glrt.val[0]==0){
				tableau[ind_tab][0]=I;
				tableau[ind_tab][1]=J;
				ind_tab=ind_tab+1;
			}
		}
	}
}

void image_tableau(IplImage* img_glrt,double** tableau, int taille_tableau, int nb_entrees)
{
	int I,J;
	double min = cherche_min(tableau, taille_tableau, nb_entrees);
	printf("%f\n",min);
	double max = cherche_max(tableau, taille_tableau, nb_entrees);
	printf("%f\n",max);
	double coeff = 255.0/(max-min);
	
	for(I=0;I<taille_tableau;I++)
    {
		for(J=0;J<nb_entrees;J++)
        {
			CvScalar pix_glrt = cvGet2D(img_glrt,I,J);
			pix_glrt.val[0] = (unsigned char)(coeff*(tableau[I][J]-min));
			cvSet2D(img_glrt,I,J,pix_glrt);
		}
	}
	return;
}

double calcul_glrt(IplImage* src, int I, int J, double variance_noir,double variance_blanc, int hauteur_fenetre, int largeur_fenetre)
{
	int i,j;
	double somme_1 =0;
	double somme_2=0;
	double moyenne_1, moyenne_2, moyenne_3;
	double GLRT;
	int ind_1 =0;
	int ind_2=0;
	double S_1=0;
	double S_2=0;
	double S_3=0;
	
	double tableau_1[largeur_fenetre*2+hauteur_fenetre*2-4];
	double tableau_2[(largeur_fenetre-2)*(hauteur_fenetre-2)];
	
	for(i=0;i<hauteur_fenetre;i++){
		for(j=0;j<largeur_fenetre;j++){
						
			CvScalar scal = cvGet2D(src,I+i,J+j);
			
			if(i==0 || i==hauteur_fenetre-1 || j==0 || j==largeur_fenetre-1){
				tableau_1[ind_1]=(double)(scal.val[0]);
				somme_1= somme_1+(double)(scal.val[0]);
				ind_1=ind_1+1;
			}
			else {
				tableau_2[ind_2]=(double)(scal.val[0]);
				somme_2=somme_2+ (double)(scal.val[0]);
				ind_2=ind_2+1;
			}
		}
	}
				
	//Calcul des moyennes
	moyenne_1=somme_1/(largeur_fenetre*2+hauteur_fenetre*2-4);
	moyenne_2=somme_2/((largeur_fenetre-2)*(hauteur_fenetre-2));
	moyenne_3=(somme_1+somme_2)/(largeur_fenetre*hauteur_fenetre);
	
	for(int k=0; k<largeur_fenetre*2+hauteur_fenetre*2-4; k++)
		S_1=S_1+(tableau_1[k]-moyenne_1)*(tableau_1[k]-moyenne_1);
		
	for(int k=0; k<(largeur_fenetre-2)*(hauteur_fenetre-2); k++)
		S_2=S_2+(tableau_2[k]-moyenne_2)*(tableau_2[k]-moyenne_2);
		
	for(int k=0; k<largeur_fenetre*2+hauteur_fenetre*2-4; k++)
		S_3=S_3+(tableau_1[k]-moyenne_3)*(tableau_1[k]-moyenne_3);
	for(int k=0; k<(largeur_fenetre-2)*(hauteur_fenetre-2); k++)
		S_3=S_3+(tableau_2[k]-moyenne_3)*(tableau_2[k]-moyenne_3);
	
	//Calcul de rapport de vraisemblance :
	
	GLRT = -(S_1/variance_noir) - (S_2/variance_blanc) + (S_3/variance_blanc);
	
	return GLRT;
}

double calcul_glrt_2(IplImage* src, int I, int J, double variance_blanc,double variance_noir, double variance_gris, int hauteur_fenetre, int largeur_fenetre)
{
	
	// TODO : modifier la taille de la fenêtre : 1 pixel de cadre extérieur au lieu de 2 ??
	int i,j;
	double somme_1 =0;
	double somme_2=0;
	double somme_3=0;
	double moyenne_1, moyenne_2,moyenne_4,moyenne_3;
	double GLRT;
	int ind_1 =0;
	int ind_2=0;
	int ind_3=0;
	double S_1=0;
	double S_2=0;
	double S_3=0;
	double S_4=0;
	
	int taille_bord=largeur_fenetre*hauteur_fenetre-(largeur_fenetre-4)*(hauteur_fenetre-4);
	int taille_cadre=(largeur_fenetre-4)*2+(hauteur_fenetre-4)*2-4;
	int taille_int_cadre=(largeur_fenetre-6)*(hauteur_fenetre-6);
	
	double tableau_1[taille_cadre];
	double tableau_2[taille_bord];
	double tableau_3[taille_int_cadre];
	
	for(i=0;i<hauteur_fenetre;i++){
		for(j=0;j<largeur_fenetre;j++){
						
			CvScalar scal = cvGet2D(src,I+i,J+j);
		
			if((i==2 && 1<j && j<largeur_fenetre-2) || (i==hauteur_fenetre-3 && 1<j && j<largeur_fenetre-2) || (j==largeur_fenetre-3 && 1<i && i<hauteur_fenetre-2) || (j==2 && 1<i && i<hauteur_fenetre-2)){
				tableau_1[ind_1]=(double)(scal.val[0]);
				somme_1= somme_1+(double)(scal.val[0]);
				ind_1=ind_1+1;
			}
			if (i>2 && i<hauteur_fenetre-3 && j>2 && j<largeur_fenetre-3){
				tableau_3[ind_3]=(double)(scal.val[0]);
				somme_3= somme_3+(double)(scal.val[0]);
				ind_3=ind_3+1;
			}
			
			else {
				tableau_2[ind_2]=(double)(scal.val[0]);
				somme_2=somme_2+ (double)(scal.val[0]);
				ind_2=ind_2+1;
			}
		}
	}
				
	//Calcul des moyennes
	moyenne_1=somme_1/taille_cadre;
	moyenne_2=somme_2/taille_bord;
	moyenne_3=somme_3/taille_int_cadre;
	moyenne_4=(somme_2+somme_3)/(taille_bord+taille_int_cadre);
	
	for(int k=0; k<taille_cadre; k++)
		S_1=S_1+(tableau_1[k]-moyenne_1)*(tableau_1[k]-moyenne_1);
		
	for(int k=0; k<taille_bord; k++)
		S_2=S_2+(tableau_2[k]-moyenne_2)*(tableau_2[k]-moyenne_2);
		
	for(int k=0;k<taille_int_cadre;k++)
		S_3=S_3+(tableau_3[k]-moyenne_3)*(tableau_3[k]-moyenne_3);
		
	for(int k=0; k<taille_bord; k++)
		S_4=S_4+(tableau_2[k]-moyenne_4)*(tableau_2[k]-moyenne_4);
	for(int k=0; k<taille_int_cadre; k++)
		S_4=S_4+(tableau_3[k]-moyenne_4)*(tableau_3[k]-moyenne_4);
	
	//Calcul de rapport de vraisemblance :
	
	GLRT = -(-(S_4/variance_blanc) + (S_2/variance_blanc) + (S_3/variance_gris));
	
	return GLRT;
}

void ecriture_fichier_histogramme(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fenetre, int largeur_fenetre, int nb_bins)
{
	FILE* fichier = NULL;
	fichier = fopen("histo_rep.txt", "w+");
	
	if (fichier != NULL)
    {
		double min = cherche_min(tableau, hauteur_img-hauteur_fenetre, largeur_img-largeur_fenetre);
		double max = cherche_max(tableau, hauteur_img-hauteur_fenetre, largeur_img-largeur_fenetre);
		
		double pas = (max-min)/nb_bins;
		
		int I,J,k;
		for(k=0;k<nb_bins;k++)
		{
			int valeur = 0;
			
			for(I=0;I<hauteur_img-hauteur_fenetre;I++)
			{
				for(J=0;J<largeur_img-largeur_fenetre;J++)
				{
					if (tableau[I][J]>min+k*pas && tableau[I][J]<min+(k+1)*pas)
						valeur = valeur + 1;
				}
			}
			fprintf(fichier, "%f\t%d\n",min+k*pas,valeur);
		}
		
		fclose(fichier);
    }
    else
    {
        printf("Impossible d'ouvrir le fichier");
    }
}
	
void histo_reponses(int **tableau, int taille_tableau)
{
	int i_min = tableau[0][0];
	int i_max = 0;
	int j_min = tableau[0][1] ;
	int j_max = 0;
	for(int k=0;k<taille_tableau;k++){
		if (tableau[k][0] > i_max)
			i_max = tableau[k][0];
		if (tableau[k][0] < i_min)
			i_min = tableau[k][0];
		if (tableau[k][1] > j_max)
			j_max = tableau[k][1];
		if (tableau[k][1] < j_min)
			j_min = tableau[k][1];
	}
	
	int histo_i[(i_max - i_min)];
	int histo_j[(j_max - j_min)];
	for (int k = 0; k < i_max - i_min ;k++)
		histo_i[k]=0;
	for (int k = 0; k < j_max - j_min ;k++)
		histo_j[k]=0;
	
	for (int k = 0 ; k < taille_tableau ; k++){
		int indice_i = tableau[k][0] - i_min;
		histo_i[indice_i] += 1;
		int indice_j = tableau[k][1] - j_min;
		histo_j[indice_j] += 1;
	}
	
	FILE* fichier_i = NULL;
	fichier_i = fopen("histo_abscisses.txt", "w+");
	FILE* fichier_j = NULL;
	fichier_j = fopen("histo_ordonnees.txt", "w+");
	
	for (int k=0; k < i_max - i_min ; k++)
		fprintf(fichier_i,"%d\t%d\n",i_min+k,histo_i[k]);
	for (int k=0; k < j_max - j_min ; k++)
		fprintf(fichier_j,"%d\t%d\n",j_min+k,histo_j[k]);
	
	
	fclose(fichier_i);	
	fclose(fichier_j);
	
}	
	
	
