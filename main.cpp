
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
	
	int largeur_fenetre = 11; // petite fenêtre de parcours
	int hauteur_fenetre = 18;
	
	int I,J;
	
	double variance_1=8*8; // variance si on est dans le cadre
	double variance_2=8*8; // variance si on est à l'intérieur du cadre
	double variance_3=8*8; // variance n'importe où ailleurs
	double GLRT;	
	double seuil_glrt = 3875;
	
	
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
			GLRT=calcul_glrt(scan,I,J,variance_1,variance_2,variance_3,hauteur_fenetre,largeur_fenetre);
			tableau_glrt_scan[I][J]=GLRT;
			//printf("(%d,%d) : %f\n", I,J,GLRT);
		}
	}
	
	//Parcours de l'image des réponses
	
	for(I=0;I<scan_rep->height-hauteur_fenetre;I++)
	{
	  for(J=0;J<scan_rep->width-largeur_fenetre;J++)
		{				
			GLRT=calcul_glrt(scan_rep,I,J,variance_1,variance_2,variance_3,hauteur_fenetre,largeur_fenetre);
			tableau_glrt_scan_rep[I][J]=GLRT;
			//printf("(%d,%d) : %f\n", I,J,GLRT);
		}
	}
	
	
	//Histogramme
	ecriture_fichier_histogramme(tableau_glrt_scan,scan->height,scan->width,hauteur_fenetre,largeur_fenetre,25);
	
	// Image GLRT seuillée scan
	img_glrt_scan = image_seuil_glrt(img_glrt_scan,tableau_glrt_scan,seuil_glrt,scan->height,scan->width,hauteur_fenetre,largeur_fenetre);
	cvNamedWindow( "Seuil GLRT", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Seuil GLRT", img_glrt_scan );
	
	// Image GLRT seuillée scan des réponses
	img_glrt_scan_rep = image_seuil_glrt(img_glrt_scan_rep,tableau_glrt_scan_rep,seuil_glrt,scan_rep->height,scan_rep->width,hauteur_fenetre,largeur_fenetre);
	cvNamedWindow( "Seuil GLRT réponses", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Seuil GLRT réponses", img_glrt_scan_rep );
		
	
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
	
	cvReleaseImage(&img_glrt_scan);
	cvReleaseImage(&scan);
	cvReleaseImage(&img_glrt_scan_rep);
	cvReleaseImage(&scan_rep);
	cvWaitKey(0);
	
	return 0;
}
