#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cxcore.h>
#include <cvwimage.h>

double cherche_min(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen)
{
	double min = tableau[0][0];
	int I,J;
	for(I=0;I<hauteur_img-hauteur_fen;I++)
        {
          for(J=0;J<largeur_img-largeur_fen;J++)
            {
				if (tableau[I][J] < min){
					min = tableau[I][J];
				}
			}
		}
	
	return min;
}

double cherche_max(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen)
{
	double max = tableau[0][0];
	int I,J;
	for(I=0;I<hauteur_img-hauteur_fen;I++)
        {
          for(J=0;J<largeur_img-largeur_fen;J++)
            {
				if (tableau[I][J] > max){
					max = tableau[I][J];
				}
			}
		}
	
	return max;
}

IplImage* image_seuil_glrt(IplImage* img_glrt, double** tableau, double seuil, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen)
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
				}
				else{
					pix_glrt.val[0]=255;
					cvSet2D(img_glrt,I,J,pix_glrt);
				}
			}
		}
	
	return img_glrt;
}

double calcul_glrt(IplImage* src, int I, int J, double variance_1,double variance_2, double variance_3, int hauteur_fenetre, int largeur_fenetre)
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
	double tableau_2[(largeur_fenetre-1)*(hauteur_fenetre-1)];
	
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
	moyenne_2=somme_2/((largeur_fenetre-1)*(hauteur_fenetre-1));
	moyenne_3=(somme_1+somme_2)/(largeur_fenetre*hauteur_fenetre);
	
	for(int k=0; k<largeur_fenetre*2+hauteur_fenetre*2-4; k++)
		S_1=S_1+(tableau_1[k]-moyenne_1)*(tableau_1[k]-moyenne_1);
	for(int k=0; k<(largeur_fenetre-1)*(hauteur_fenetre-1); k++)
		S_2=S_2+(tableau_2[k]-moyenne_2)*(tableau_2[k]-moyenne_2);
	for(int k=0; k<largeur_fenetre*2+hauteur_fenetre*2-4; k++)
		S_3=S_3+(tableau_1[k]-moyenne_3)*(tableau_1[k]-moyenne_3);
	for(int k=0; k<(largeur_fenetre-1)*(hauteur_fenetre-1); k++)
		S_3=S_3+(tableau_2[k]-moyenne_3)*(tableau_2[k]-moyenne_3);
	
	//Calcul de rapport de vraisemblance :
	
	GLRT = -(S_1/variance_1) - (S_2/variance_2) + (S_3/variance_3);
	
	return GLRT;
}

void ecriture_fichier_histogramme(double ** tableau, int hauteur_img, int largeur_img, int hauteur_fen, int largeur_fen, int nb_bins)
{
	FILE* fichier = NULL;
	fichier = fopen("histo.txt", "w+");
	
	if (fichier != NULL)
    {
		double min = cherche_min(tableau, hauteur_img, largeur_img, hauteur_fen, largeur_fen);
		double max = cherche_max(tableau, hauteur_img, largeur_img, hauteur_fen, largeur_fen);
		
		double pas = (max-min)/nb_bins;
		
		int I,J,k;
		for(k=0;k<nb_bins;k++)
		{
			int valeur = 0;
			
			for(I=0;I<hauteur_img-hauteur_fen;I++)
			{
				for(J=0;J<largeur_img-largeur_fen;J++)
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
	
	
	
	
	
