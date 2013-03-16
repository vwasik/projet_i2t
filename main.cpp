
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <cvwimage.h>

#include "./include/fonctions.h"

int main(int argc, char *argv[])
{
	// Initialisation
	
	IplImage* scan;
	IplImage* scan_rep;
	IplImage* img_glrt_scan;
	IplImage* img_glrt_scan_rep;
	IplImage* img_glrt;
	IplImage* img_glrt_rep;
	
	int largeur_fenetre = 11; // petite fenêtre de parcours
	int hauteur_fenetre = 18;
	
	int I,J;
	
	double variance_noir=8*8; // variance si on est dans le cadre
	double variance_blanc=8*8; // variance si on est à l'intérieur du cadre
	double variance_gris=14*14;//1;
	double GLRT;
	double seuil_glrt_1 = 3000;
	double seuil_glrt_2 = -6000;//-590;//-6000;
	
	
	// Lecture de l'image scan
	
	scan = cvLoadImage(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
	if(!scan){
		printf("Impossible à ouvrir");
		exit(0);
	}
	cvNamedWindow( "Scan", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Scan", scan );
	
	// Lecture de l'image des réponses
	
	scan_rep = cvLoadImage(argv[2],CV_LOAD_IMAGE_GRAYSCALE);
	if(!scan_rep){
		printf("Impossible à ouvrir");
		exit(0);
	}
	cvNamedWindow( "Scan réponses", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Scan réponses", scan_rep );
	
	img_glrt_scan =cvCreateImage(cvGetSize(scan),IPL_DEPTH_8U,1);
	
	img_glrt_scan_rep =cvCreateImage(cvGetSize(scan_rep),IPL_DEPTH_8U,1);

	img_glrt =cvCreateImage(cvSize((scan->width)-largeur_fenetre,(scan->height)-hauteur_fenetre),IPL_DEPTH_8U,1);
	img_glrt_rep =cvCreateImage(cvSize((scan_rep->width)-largeur_fenetre,(scan_rep->height)-hauteur_fenetre),IPL_DEPTH_8U,1);

	
	//Allocations
	
	double** tableau_glrt_scan = (double**)malloc(sizeof *tableau_glrt_scan * scan->height);
	for (int k = 0; k < scan->height; k++)
        tableau_glrt_scan[k] = (double*)malloc(sizeof **tableau_glrt_scan * scan->width);
        
	double** tableau_glrt_scan_rep = (double**)malloc(sizeof *tableau_glrt_scan_rep * scan_rep->height);
	for (int k = 0; k < scan_rep->height; k++)
        tableau_glrt_scan_rep[k] = (double*)malloc(sizeof **tableau_glrt_scan_rep * scan_rep->width);
        
	
	//Parcours de l'image scannée
	
	for(I=0;I<scan->height-hauteur_fenetre;I++)
	{
	  for(J=0;J<scan->width-largeur_fenetre;J++)
		{				
			GLRT=calcul_glrt(scan,I,J,variance_noir,variance_blanc,hauteur_fenetre,largeur_fenetre);
			tableau_glrt_scan[I][J]=GLRT;
			//printf("(%d,%d) : %f\n", I,J,GLRT);
		}
	}
	
	image_tableau(img_glrt,tableau_glrt_scan,scan->height-hauteur_fenetre,scan->width-largeur_fenetre);
	cvNamedWindow( "GLRT", CV_WINDOW_AUTOSIZE );
	cvShowImage( "GLRT", img_glrt );
	
	//cvSaveImage("Image_GLRT_version_1.pnm",img_glrt);
	
	// Image GLRT seuillée scan
	
	int compteur=0;
	
	image_seuil_glrt(img_glrt_scan,tableau_glrt_scan,seuil_glrt_1,&compteur,scan->height,scan->width,hauteur_fenetre,largeur_fenetre);
	printf("%d",compteur);
	cvNamedWindow( "Seuil GLRT", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Seuil GLRT", img_glrt_scan );
	
	int** tableau_coord_seuil = (int**)malloc(sizeof *tableau_coord_seuil * compteur);
	for (int k = 0; k <compteur; k++)
        tableau_coord_seuil[k] = (int*)malloc(sizeof **tableau_coord_seuil * 2);
        
    tableau_coord_bin(img_glrt_scan,tableau_coord_seuil,scan->height,scan->width,hauteur_fenetre,largeur_fenetre);
	
	// Creation des histogrammes des coordonnées et étiquetage :
	
	int i_min = cherche_i_min(tableau_coord_seuil,compteur);
	int i_max = cherche_i_max(tableau_coord_seuil,compteur);
	int j_min = cherche_j_min(tableau_coord_seuil,compteur);
	int j_max = cherche_j_max(tableau_coord_seuil,compteur);
	
	
	int taille_histo_i = i_max-i_min;
	int taille_histo_j = j_max-j_min;
	
	int* histo_i = (int*)malloc(sizeof(int) * taille_histo_i);
	int* histo_j = (int*)malloc(sizeof(int) * taille_histo_j);
	for (int k=0;k<taille_histo_j;k++){
		histo_j[k]=0;
	}
	for (int k=0;k<taille_histo_i;k++){
		histo_i[k]=0;
	}
	
	histo_reponses_i(tableau_coord_seuil,compteur,histo_i,taille_histo_i);
	
	/*
	int** tableau_classe_i = (int**)malloc(sizeof *tableau_classe_i * 27);
	for (int k = 0; k < 27; k++)
        tableau_classe_i[k] = (int*)malloc(sizeof **tableau_classe_i * 25);
    */
    
    int* tableau_classe_i = (int*)malloc(sizeof(int) * 27);
        
    classe_i_max(histo_i, taille_histo_i, tableau_classe_i,27,i_min);
    
    printf("\n%d,%d\n",tableau_coord_seuil[0][0],tableau_coord_seuil[0][1]);
    
    histo_reponses_j(tableau_coord_seuil,compteur,histo_j,taille_histo_j,tableau_classe_i[0],tableau_classe_i[25]);
    
    int** tableau_classe_j = (int**)malloc(sizeof *tableau_classe_j * 4);
	for (int k = 0; k <4; k++)
        tableau_classe_j[k] = (int*)malloc(sizeof **tableau_classe_j * 5);
        
    classe_j_max(histo_j,taille_histo_j,tableau_classe_j,4,5,j_min);
	
	
	//cvSaveImage("Image_GLRT_scan_version_1.pnm",img_glrt_scan);
	
	
	//Parcours de l'image des réponses
	
	for(I=0;I<scan_rep->height-hauteur_fenetre-4;I++)
	{
		for(J=0;J<scan_rep->width-largeur_fenetre-4;J++)
		{
			tableau_glrt_scan_rep[I][J]=-920000;
			
			for(int k=0;k<compteur;k++){
				if (tableau_coord_seuil[k][0]==I && tableau_coord_seuil[k][1]==J){
					GLRT=calcul_glrt_2(scan_rep,I-2,J-2,variance_noir,variance_blanc,variance_gris,hauteur_fenetre+4,largeur_fenetre+4);
					tableau_glrt_scan_rep[I][J]=GLRT;
					//printf("(%d,%d) : %f\n", I,J,tableau_glrt_scan_rep[I][J]);
				}
			}
		}
	}
	/*
	printf("\n(817,52) : %f\n", tableau_glrt_scan_rep[817][52]);
	printf("(817,53) : %f\n", tableau_glrt_scan_rep[817][53]);
	printf("(817,54) : %f\n", tableau_glrt_scan_rep[817][54]);
	* */
	
	ecriture_fichier_histogramme(tableau_glrt_scan_rep,scan_rep->height,scan_rep->width,hauteur_fenetre+4,largeur_fenetre+4,1000);

	
	image_tableau(img_glrt_rep,tableau_glrt_scan_rep,scan_rep->height-(hauteur_fenetre+4),scan_rep->width-(largeur_fenetre+4));
	cvNamedWindow( "GLRT rep", CV_WINDOW_AUTOSIZE );
	cvShowImage( "GLRT rep", img_glrt_rep );
	
	//Histogramme
	//ecriture_fichier_histogramme(tableau_glrt_scan,scan->height,scan->width,hauteur_fenetre,largeur_fenetre,25);
	
	
	// Image GLRT seuillée scan des réponses
	int compteur2=0;
	image_seuil_glrt(img_glrt_scan_rep,tableau_glrt_scan_rep,seuil_glrt_2,&compteur2,scan_rep->height,scan_rep->width,hauteur_fenetre+4,largeur_fenetre+4);
	cvNamedWindow( "Seuil GLRT réponses", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Seuil GLRT réponses", img_glrt_scan_rep );
		
	//cvSaveImage("Image_GLRT_seuil_rep_tout.pnm",img_glrt_scan_rep);

	
	//Libération
    for(int k = 0; k < scan->height; k++)
	{
        free(tableau_glrt_scan[k]);
	}
	free(tableau_glrt_scan);
	
	for(int k = 0; k < scan_rep->height; k++)
	{
        free(tableau_glrt_scan_rep[k]);
	}
	free(tableau_glrt_scan_rep);
	
	for(int k = 0; k < compteur; k++)
	{
        free(tableau_coord_seuil[k]);
	}
	free(tableau_coord_seuil);
	
	
	for(int k = 0; k < 4; k++)
	{
        free(tableau_classe_j[k]);
	}
	free(tableau_classe_j);
	
	
	free(tableau_classe_i);
	
	free(histo_i);
	
	cvReleaseImage(&img_glrt_scan);
	cvReleaseImage(&img_glrt);
	cvReleaseImage(&scan);
	cvReleaseImage(&img_glrt_scan_rep);
	cvReleaseImage(&scan_rep);
	cvWaitKey(0);
	
	return 0;
}
