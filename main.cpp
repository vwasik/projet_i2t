
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <cvwimage.h>

#include "./include/fonctions.h"

int main(int argc, char *argv[])
{
	// Initialisation
	
	IplImage* scan_vierge;
	IplImage* scan_rep;
	IplImage* img_glrt_seuil_vierge;
	IplImage* img_glrt_seuil_rep;
	IplImage* img_glrt_vierge;
	IplImage* img_glrt_rep;
	
	int largeur_fenetre = 11; // petite fenêtre de parcours pour glrt_1
	int hauteur_fenetre = 18;
	
	int largeur_fenetre_2 = largeur_fenetre + 4; // petite fenetre de parcours pour glrt_2 (cadre de 2 pixels autour de la première)
	int hauteur_fenetre_2 = hauteur_fenetre + 4;
	
	double variance_noir=8*8; // variance si on est dans le cadre
	double variance_blanc=8*8; // variance si on est à l'intérieur du cadre
	double variance_gris=1;//14*14;
	double GLRT;
	double seuil_glrt_vierge = 3000;
	double seuil_glrt_rep = -6000;//-590;
	
	int nb_questions = 100;
	int nb_reponses_par_question = 5;
	int nb_colonnes = 4;
	int nb_classes_i = nb_questions/nb_colonnes + 2;
	
	int taille_tableau_coord_seuil=0;
	int taille_tableau_coord_seuil_rep=0;
	
	char tableau_alphabet[26]={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
	
	
	// Lecture de l'image vierge
	
	scan_vierge = cvLoadImage(argv[1],CV_LOAD_IMAGE_GRAYSCALE);
	if(!scan_vierge){
		printf("Impossible à ouvrir");
		exit(0);
	}
	cvNamedWindow( "Scan vierge", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Scan vierge", scan_vierge );
	
	
	// Lecture de l'image des réponses
	
	scan_rep = cvLoadImage(argv[2],CV_LOAD_IMAGE_GRAYSCALE);
	if(!scan_rep){
		printf("Impossible à ouvrir");
		exit(0);
	}
	cvNamedWindow( "Scan réponses", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Scan réponses", scan_rep );
	
	img_glrt_vierge =cvCreateImage(cvSize((scan_vierge->width)-largeur_fenetre,(scan_vierge->height)-hauteur_fenetre),IPL_DEPTH_8U,1);
	img_glrt_rep =cvCreateImage(cvSize((scan_rep->width)-largeur_fenetre,(scan_rep->height)-hauteur_fenetre),IPL_DEPTH_8U,1);
	img_glrt_seuil_vierge =cvCreateImage(cvGetSize(scan_vierge),IPL_DEPTH_8U,1);
	img_glrt_seuil_rep =cvCreateImage(cvGetSize(scan_rep),IPL_DEPTH_8U,1);


	//Allocations
	
	int* tableau_classe_i = (int*)malloc(sizeof(int) * nb_classes_i);
	
	int** tableau_classe_j = (int**)malloc(sizeof *tableau_classe_j * nb_colonnes);
	for (int k = 0; k <nb_colonnes; k++)
        tableau_classe_j[k] = (int*)malloc(sizeof **tableau_classe_j * nb_reponses_par_question);
	
    int** tableau_reponses = (int**)malloc(sizeof *tableau_reponses * nb_questions);
    for (int k=0; k < nb_questions; k++)
		tableau_reponses[k] = (int*)malloc(sizeof **tableau_reponses * nb_reponses_par_question);
	for (int k=0; k<nb_questions; k++){
		for (int l=0; l<nb_reponses_par_question; l++){
			tableau_reponses[k][l]=0;
		}
	}
	
	double** tableau_glrt_scan_vierge = (double**)malloc(sizeof *tableau_glrt_scan_vierge * scan_vierge->height);
	for (int k = 0; k < scan_vierge->height; k++)
        tableau_glrt_scan_vierge[k] = (double*)malloc(sizeof **tableau_glrt_scan_vierge * scan_vierge->width);
        
	double** tableau_glrt_scan_rep = (double**)malloc(sizeof *tableau_glrt_scan_rep * scan_rep->height);
	for (int k = 0; k < scan_rep->height; k++)
        tableau_glrt_scan_rep[k] = (double*)malloc(sizeof **tableau_glrt_scan_rep * scan_rep->width);
	
	
	// Calcul du GLRT sur l'image vierge (repérage des cases vides)
	
	for(int I=0;I<scan_vierge->height-hauteur_fenetre;I++)
	{
	  for(int J=0;J<scan_vierge->width-largeur_fenetre;J++)
		{				
			GLRT=calcul_glrt(scan_vierge,I,J,variance_noir,variance_blanc,hauteur_fenetre,largeur_fenetre);
			tableau_glrt_scan_vierge[I][J]=GLRT;
		}
	}
	
	image_tableau(img_glrt_vierge,tableau_glrt_scan_vierge,scan_vierge->height-hauteur_fenetre,scan_vierge->width-largeur_fenetre);
	cvNamedWindow( "GLRT scan vierge", CV_WINDOW_AUTOSIZE );
	cvShowImage( "GLRT scan vierge", img_glrt_vierge );
	
	cvSaveImage("Image_GLRT_vierge.pnm",img_glrt_vierge);
	
	
	// Image GLRT seuillée vierge
	
	image_seuil_glrt(img_glrt_seuil_vierge,tableau_glrt_scan_vierge,seuil_glrt_vierge,&taille_tableau_coord_seuil,scan_vierge->height-hauteur_fenetre,scan_vierge->width-largeur_fenetre);
	cvNamedWindow( "Seuil GLRT scan vierge", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Seuil GLRT scan vierge", img_glrt_seuil_vierge );
	
	cvSaveImage("Image_GLRT_seuil_vierge.pnm",img_glrt_seuil_vierge);
	
	int** tableau_coord_seuil = (int**)malloc(sizeof *tableau_coord_seuil * taille_tableau_coord_seuil);
	for (int k = 0; k <taille_tableau_coord_seuil; k++)
        tableau_coord_seuil[k] = (int*)malloc(sizeof **tableau_coord_seuil * 2);
        
    tableau_coord_bin(img_glrt_seuil_vierge,tableau_coord_seuil,scan_vierge->height-hauteur_fenetre,scan_vierge->width-largeur_fenetre);
	
	
	// Creation des histogrammes des coordonnées et étiquetage des cases vides :
	
	int i_min = cherche_i_min(tableau_coord_seuil,taille_tableau_coord_seuil);
	int i_max = cherche_i_max(tableau_coord_seuil,taille_tableau_coord_seuil);
	int j_min = cherche_j_min(tableau_coord_seuil,taille_tableau_coord_seuil);
	int j_max = cherche_j_max(tableau_coord_seuil,taille_tableau_coord_seuil);
	
	int taille_histo_i = i_max-i_min;
	int taille_histo_j = j_max-j_min;
	
	int* histo_i = (int*)malloc(sizeof(int) * taille_histo_i);
	int* histo_j = (int*)malloc(sizeof(int) * taille_histo_j);
	for (int k=0;k<taille_histo_j;k++)
		histo_j[k]=0;
	for (int k=0;k<taille_histo_i;k++)
		histo_i[k]=0;
	
	histo_reponses_i(tableau_coord_seuil,taille_tableau_coord_seuil,histo_i,taille_histo_i);
	classe_i_max(histo_i, taille_histo_i, tableau_classe_i,nb_classes_i,i_min);
        
    histo_reponses_j(tableau_coord_seuil,taille_tableau_coord_seuil,histo_j,taille_histo_j,tableau_classe_i[0],tableau_classe_i[25]);
    classe_j_max(histo_j,taille_histo_j,tableau_classe_j,nb_colonnes,nb_reponses_par_question,j_min);
		
	
	// Calcul du GLRT sur les cases de l'image des réponses (repérage des cases pleines) :
	
	for(int I=0; I < scan_rep->height-hauteur_fenetre_2 ; I++){
		for(int J=0; J < scan_rep->width-largeur_fenetre_2 ; J++){
			
			tableau_glrt_scan_rep[I][J]=-1060000;
			
			for(int k=0;k<taille_tableau_coord_seuil;k++){
				if (tableau_coord_seuil[k][0]==I && tableau_coord_seuil[k][1]==J){
					GLRT=calcul_glrt_2(scan_rep,I-2,J-2,variance_noir,variance_blanc,variance_gris,hauteur_fenetre_2,largeur_fenetre_2);
					tableau_glrt_scan_rep[I][J]=GLRT;
				}
			}
		}
	}
	
	ecriture_fichier_histogramme(tableau_glrt_scan_rep,scan_rep->height-hauteur_fenetre_2,scan_rep->width-largeur_fenetre_2,1000);
	
	image_tableau(img_glrt_rep,tableau_glrt_scan_rep,scan_rep->height-hauteur_fenetre_2,scan_rep->width-largeur_fenetre_2);
	cvNamedWindow( "GLRT scan rep", CV_WINDOW_AUTOSIZE );
	cvShowImage( "GLRT sacn rep", img_glrt_rep );
	
	cvSaveImage("Image_GLRT_rep.pnm",img_glrt_rep);
	
	
	// Image GLRT seuillée des réponses
		
	image_seuil_glrt(img_glrt_seuil_rep,tableau_glrt_scan_rep,seuil_glrt_rep,&taille_tableau_coord_seuil_rep,scan_rep->height-hauteur_fenetre_2,scan_rep->width-largeur_fenetre_2);
	cvNamedWindow( "Seuil GLRT scan rep", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Seuil GLRT scan rep", img_glrt_seuil_rep );
	
	cvSaveImage("Image_GLRT_seuil_rep.pnm",img_glrt_seuil_rep);
	
	
	// Etiquetage des réponses
	
	int** tableau_coord_seuil_rep = (int**)malloc(sizeof *tableau_coord_seuil_rep * taille_tableau_coord_seuil_rep);
	for (int k = 0; k <taille_tableau_coord_seuil_rep; k++)
        tableau_coord_seuil_rep[k] = (int*)malloc(sizeof **tableau_coord_seuil_rep * 2);
	
	tableau_coord_bin(img_glrt_seuil_rep,tableau_coord_seuil_rep,scan_rep->height-hauteur_fenetre_2,scan_rep->width-largeur_fenetre_2);
	
	etiquetage(tableau_coord_seuil_rep,tableau_classe_i,tableau_classe_j,tableau_reponses,taille_tableau_coord_seuil_rep,nb_classes_i,nb_colonnes,nb_reponses_par_question,nb_questions,nb_reponses_par_question,tableau_alphabet);
	
	
	//Libération
	
	free(tableau_classe_i);
	
    for(int k = 0; k < nb_colonnes; k++)
        free(tableau_classe_j[k]);
	free(tableau_classe_j);

	for(int k = 0; k < nb_questions; k++)
        free(tableau_reponses[k]);
	free(tableau_reponses);
    
    for(int k = 0; k < scan_vierge->height; k++)
        free(tableau_glrt_scan_vierge[k]);
	free(tableau_glrt_scan_vierge);
	
	for(int k = 0; k < scan_rep->height; k++)
        free(tableau_glrt_scan_rep[k]);
	free(tableau_glrt_scan_rep);
	
	for(int k = 0; k < taille_tableau_coord_seuil; k++)
        free(tableau_coord_seuil[k]);
	free(tableau_coord_seuil);
	
	for(int k = 0; k < taille_tableau_coord_seuil_rep; k++)
        free(tableau_coord_seuil_rep[k]);
	free(tableau_coord_seuil_rep);
	
	free(histo_i);
	free(histo_j);
	
	cvReleaseImage(&img_glrt_seuil_vierge);
	cvReleaseImage(&img_glrt_vierge);
	cvReleaseImage(&scan_vierge);
	cvReleaseImage(&img_glrt_seuil_rep);
	cvReleaseImage(&scan_rep);
	cvWaitKey(0);
	
	return 0;
}
