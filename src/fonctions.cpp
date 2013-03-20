#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cxcore.h>
#include <cvwimage.h>

double cherche_min(double ** tableau, int taille_tableau, int nb_entrees)
{
	double min = tableau[0][0];
	int I,J;
	for(I=0; I<taille_tableau; I++){
		for(J=0; J<nb_entrees; J++){
			if (tableau[I][J] < min)
				min = tableau[I][J];
		}
	}
	
	return min;
}

double cherche_max(double ** tableau, int taille_tableau, int nb_entrees)
{
	double max = tableau[0][0];
	int I,J;
	for(I=0; I<taille_tableau; I++){
		for(J=0; J<nb_entrees; J++){
			if (tableau[I][J] > max)
				max = tableau[I][J];
		}
	}
	
	return max;
}

void image_seuil_glrt(IplImage* img_glrt, double** tableau, double seuil, int *compteur, int taille_tableau, int nb_entrees_tableau)
{
	int I,J;
	for(I=0; I<taille_tableau; I++){
		for(J=0;J<nb_entrees_tableau;J++){
			
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
	
	return;	
}

void tableau_coord_bin(IplImage* img_bin, int** tableau, int taille_tableau, int nb_entrees_tableau)
{
	int I,J;
	int ind_tab=0;
	for(I=0; I<taille_tableau; I++){
		for(J=0; J<nb_entrees_tableau; J++){
			
			CvScalar pix_glrt = cvGet2D(img_bin,I,J);
				
			if (pix_glrt.val[0]==0){
				tableau[ind_tab][0]=I;
				tableau[ind_tab][1]=J;
				ind_tab=ind_tab+1;
			}
		}
	}
	
	return;
}

void image_tableau(IplImage* img_glrt,double** tableau, int taille_tableau, int nb_entrees_tableau)
{
	int I,J;
	double min = cherche_min(tableau, taille_tableau, nb_entrees_tableau);
	double max = cherche_max(tableau, taille_tableau, nb_entrees_tableau);
	double coeff = 255.0/(max-min);
	
	for(I=0;I<taille_tableau;I++)
    {
		for(J=0;J<nb_entrees_tableau;J++)
        {
			CvScalar pix_glrt = cvGet2D(img_glrt,I,J);
			pix_glrt.val[0] = (unsigned char)(coeff*(tableau[I][J]-min));
			cvSet2D(img_glrt,I,J,pix_glrt);
		}
	}
	
	return;
}

double calcul_glrt_pixel(IplImage* src, int I, int J, double variance_blanc, double variance_noir, int hauteur_fenetre, int largeur_fenetre)
{
	double somme_1=0, somme_2=0;
	double moyenne_1=0, moyenne_2=0, moyenne_3=0;
	double GLRT=0;
	int ind_1=0,ind_2=0;
	double S_1=0, S_2=0, S_3=0;;
	
	int taille_cadre=largeur_fenetre*2+hauteur_fenetre*2-4;
	int taille_int_cadre=(largeur_fenetre-2)*(hauteur_fenetre-2);
	
	double tableau_1[taille_cadre];
	double tableau_2[taille_int_cadre];
	for (int k=0; k<taille_cadre; k++)
		tableau_1[k]=0;
	for (int k=0; k<taille_int_cadre; k++)
		tableau_2[k]=0;
	
	for(int i=0;i<hauteur_fenetre;i++){
		for(int j=0;j<largeur_fenetre;j++){
						
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
	moyenne_1=somme_1/taille_cadre;
	moyenne_2=somme_2/taille_int_cadre;
	moyenne_3=(somme_1+somme_2)/(largeur_fenetre*hauteur_fenetre);
	
	for(int k=0; k<taille_cadre; k++)
		S_1=S_1+(tableau_1[k]-moyenne_1)*(tableau_1[k]-moyenne_1);
		
	for(int k=0; k<taille_int_cadre; k++)
		S_2=S_2+(tableau_2[k]-moyenne_2)*(tableau_2[k]-moyenne_2);
		
	for(int k=0; k<taille_cadre; k++)
		S_3=S_3+(tableau_1[k]-moyenne_3)*(tableau_1[k]-moyenne_3);
	for(int k=0; k<taille_int_cadre; k++)
		S_3=S_3+(tableau_2[k]-moyenne_3)*(tableau_2[k]-moyenne_3);
	
	//Calcul de rapport de vraisemblance :
	GLRT = -(S_1/variance_noir) - (S_2/variance_blanc) + (S_3/variance_blanc);
	
	return GLRT;
}

double calcul_glrt_2_pixel(IplImage* src, int I, int J, double variance_blanc,double variance_noir, double variance_gris, int hauteur_fenetre, int largeur_fenetre)
{

	double somme_1 =0, somme_2=0, somme_3=0;
	double moyenne_1=0, moyenne_2=0,moyenne_4=0,moyenne_3=0;
	double GLRT=0;
	int ind_1=0, ind_2=0, ind_3=0;
	double S_1=0, S_2=0, S_3=0, S_4=0;
	
	int taille_bord=largeur_fenetre*hauteur_fenetre-(largeur_fenetre-4)*(hauteur_fenetre-4);
	int taille_cadre=(largeur_fenetre-4)*2+(hauteur_fenetre-4)*2-4;
	int taille_int_cadre=(largeur_fenetre-6)*(hauteur_fenetre-6);
	
	double tableau_1[taille_cadre];
	double tableau_2[taille_bord];
	double tableau_3[taille_int_cadre];
	for (int k=0; k<taille_cadre; k++)
		tableau_1[k]=0;
	for (int k=0; k<taille_bord; k++)
		tableau_2[k]=0;
	for (int k=0; k<taille_int_cadre; k++)
		tableau_3[k]=0;
	
	for(int i=0;i<hauteur_fenetre;i++){
		for(int j=0;j<largeur_fenetre;j++){
			
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

void calcul_glrt_1_image(IplImage* src, double** tableau_GLRT, double variance_blanc,double variance_noir, int hauteur_fenetre, int largeur_fenetre, int hauteur_image, int largeur_image)
{
	double GLRT=0;
	
	for(int I=0;I<hauteur_image-hauteur_fenetre;I++)
	{
	  for(int J=0;J<largeur_image-largeur_fenetre;J++)
		{				
			GLRT=calcul_glrt_pixel(src,I,J,variance_noir,variance_blanc,hauteur_fenetre,largeur_fenetre);
			tableau_GLRT[I][J]=GLRT;
		}
	}
	
	return;
}

void calcul_glrt_2_image(IplImage* src, double** tableau_GLRT, int** tableau_coord_seuil, int taille_tableau_coord_seuil, double variance_blanc,double variance_noir, double variance_gris, int hauteur_fenetre, int largeur_fenetre, int hauteur_image, int largeur_image)
{
	double GLRT=0;
	
	for(int I=0;I<hauteur_image-hauteur_fenetre;I++){
	  for(int J=0;J<largeur_image-largeur_fenetre;J++){
		  
			for(int k=0;k<taille_tableau_coord_seuil;k++){
				if (tableau_coord_seuil[k][0]==I && tableau_coord_seuil[k][1]==J){
						
					GLRT=calcul_glrt_2_pixel(src,I-2,J-2,variance_blanc,variance_noir,variance_gris,hauteur_fenetre,largeur_fenetre);
					tableau_GLRT[I][J]=GLRT;
				}
			}
		}
	}
	
	return;
}

void ecriture_fichier_histogramme(double** tableau, int taille_tableau, int nb_entrees_tableau, int nb_bins)
{
	FILE* fichier = NULL;
	fichier = fopen("histo_rep.txt", "w+");
	
	if (fichier != NULL){
		double min = cherche_min(tableau, taille_tableau, nb_entrees_tableau);
		double max = cherche_max(tableau, taille_tableau, nb_entrees_tableau);
		
		double pas = (max-min)/nb_bins;
		
		int I,J,k;
		for(k=0;k<nb_bins;k++){
			
			int valeur = 0;
			
			for(I=0; I<taille_tableau; I++){
				for(J=0; J<nb_entrees_tableau; J++){
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
    
    return;
}

int cherche_i_min(int** tableau_coord, int taille_tableau)
{
	int i_min = tableau_coord[0][0];
	
	for(int k=0;k<taille_tableau;k++){
		if (tableau_coord[k][0] < i_min)
			i_min = tableau_coord[k][0];
	}
	
	return i_min;
}

int cherche_i_max(int** tableau_coord, int taille_tableau)
{
	int i_max = 0;
	for(int k=0;k<taille_tableau;k++){
		if (tableau_coord[k][0] > i_max)
			i_max = tableau_coord[k][0];
	}
	
	return i_max;
}	

int cherche_j_min(int** tableau_coord, int taille_tableau)
{
	int j_min = tableau_coord[0][1];
	
	for(int k=0;k<taille_tableau;k++){
		if (tableau_coord[k][1] < j_min)
			j_min = tableau_coord[k][1];
	}
	
	return j_min;
}

int cherche_j_max(int** tableau_coord, int taille_tableau)
{
	int j_max = 0;
	for(int k=0;k<taille_tableau;k++){
		if (tableau_coord[k][1] > j_max)
			j_max = tableau_coord[k][1];
	}
	
	return j_max;
}		

void histo_reponses_i(int** tableau, int taille_tableau, int* histo_i, int taille_histo_i)
{
	int i_min = cherche_i_min(tableau,taille_tableau);
	
	for (int k=0; k<taille_tableau; k++){
		int indice_i = tableau[k][0]-i_min;
		histo_i[indice_i] += 1;
	}
	
	FILE* fichier_i = NULL;
	fichier_i = fopen("histo_ordonnees.txt", "w+");
	
	if (fichier_i != NULL){
		for (int k=0; k < taille_histo_i ; k++)
			fprintf(fichier_i,"%d\t%d\n",i_min+k,histo_i[k]);
		
		fclose(fichier_i);
	}
	else{
        printf("Impossible d'ouvrir le fichier");
    }
	
	return;
}

void histo_reponses_j(int** tableau, int taille_tableau, int* histo_j, int taille_histo_j,int ordonnee_min,int ordonnee_max)
{
	int j_min = cherche_j_min(tableau,taille_tableau);
	
	for (int k = 0 ; k < taille_tableau ; k++){
		if ((tableau[k][0]>ordonnee_min) && (tableau[k][0]<ordonnee_max)){
			int indice_j = tableau[k][1] - j_min;
			histo_j[indice_j] += 1;
		}
	}
	
	FILE* fichier_j = NULL;
	fichier_j = fopen("histo_abscisses.txt", "w+");
	
	if (fichier_j != NULL){
		for (int k=0; k < taille_histo_j ; k++)
			fprintf(fichier_j,"%d\t%d\n",j_min+k,histo_j[k]);
		
		fclose(fichier_j);
	}
	else{
        printf("Impossible d'ouvrir le fichier");
    }
	
	return;
}

void classe_i_max(int* histo_i, int taille_histo_i, int* tableau_classe_i, int taille_tableau_classe_i, int i_min)
{
	int classe_actuelle = 0;
	int nb_zero_de_suite = 0;
	int trouve_autre_chose_que_zero = 0;
	
	for (int k=0; k < taille_histo_i; k++){
		if (histo_i[k] != 0){
			trouve_autre_chose_que_zero =1;
			nb_zero_de_suite = 0;			
		}
		if ((trouve_autre_chose_que_zero==1) && (histo_i[k]==0)){
			nb_zero_de_suite = nb_zero_de_suite + 1;
		}
		
		if ((nb_zero_de_suite>2) && (trouve_autre_chose_que_zero==1)){
			tableau_classe_i[classe_actuelle]=i_min+k;
			classe_actuelle=classe_actuelle+1;
			nb_zero_de_suite=0;
			trouve_autre_chose_que_zero = 0;
		}
		if (k==taille_histo_i -1){
			tableau_classe_i[classe_actuelle]=i_min+k;
		}
	}
	
	FILE* fichier_classe_i = NULL;
	fichier_classe_i = fopen("histo_ordonnees_classees.txt", "w+");
	
	if (fichier_classe_i!=NULL){
		for (int classe_actuelle=0; classe_actuelle<taille_tableau_classe_i ; classe_actuelle++){
			fprintf(fichier_classe_i,"%d\t%d\n",classe_actuelle,tableau_classe_i[classe_actuelle]);
		}
	
		fclose(fichier_classe_i);
	}
	else{
        printf("Impossible d'ouvrir le fichier");
    }
    
	return;
}
	
void classe_j_max(int* histo_j, int taille_histo_j, int** tableau_classe_j, int taille_tableau_classe_j, int nb_entrees_tableau_classe_j, int j_min)
{
	int lettre_actuelle = 0;
	int colonne_actuelle = 0;
	int nb_zero_de_suite = 0;
	int trouve_autre_chose_que_zero = 0;
	
	for (int k=0; k < taille_histo_j; k++){
		if (histo_j[k] != 0){
			trouve_autre_chose_que_zero =1;
			nb_zero_de_suite = 0;			
		}
		if (((trouve_autre_chose_que_zero ==1) && (histo_j[k] == 0)) || (lettre_actuelle==nb_entrees_tableau_classe_j)){
			nb_zero_de_suite = nb_zero_de_suite + 1;
		}
		
		if ((nb_zero_de_suite > 9) && (trouve_autre_chose_que_zero == 1) && (lettre_actuelle<nb_entrees_tableau_classe_j)){
			tableau_classe_j[colonne_actuelle][lettre_actuelle]=j_min+k;
			lettre_actuelle=lettre_actuelle+1;
			nb_zero_de_suite=0;
			trouve_autre_chose_que_zero=0;
		}
		
		if ((nb_zero_de_suite > 40)){
			lettre_actuelle=0;
			colonne_actuelle = colonne_actuelle + 1;
			nb_zero_de_suite=0;
			trouve_autre_chose_que_zero = 0;
		}
		
		if (k == taille_histo_j -1){
			tableau_classe_j[colonne_actuelle][lettre_actuelle]=j_min+k;
		}
	}
	
	FILE* fichier_classe_j = NULL;
	fichier_classe_j = fopen("histo_abscisses_classees.txt", "w+");
	
	if (fichier_classe_j != NULL){
	
		for (int colonne_actuelle=0; colonne_actuelle<taille_tableau_classe_j ; colonne_actuelle++){
			for (int lettre_actuelle=0;lettre_actuelle<nb_entrees_tableau_classe_j;lettre_actuelle++){
				fprintf(fichier_classe_j,"%d\t%d\t%d\n",colonne_actuelle,lettre_actuelle,tableau_classe_j[colonne_actuelle][lettre_actuelle]);
			}
		}
		
		fclose(fichier_classe_j);
	}
	else{
        printf("Impossible d'ouvrir le fichier");
    }
	
	return;
}

char correspondance_lettre(int chiffre, char* tableau_alphabet)
{
	char lettre = tableau_alphabet[chiffre % 26];
	return lettre;
}

void etiquetage_ordonnees(int k, int** tableau_coord, int* tableau_classe_i, int *ligne_trouvee, int *no_rep, int taille_tableau_coord, int taille_tableau_classe_i)
{
	if (tableau_coord[k][0] < tableau_classe_i[0]){
		*no_rep = 1;
		*ligne_trouvee=0;
	}
	if (tableau_coord[k][0]> tableau_classe_i[25]){
		*no_rep = 1;
		*ligne_trouvee = 26;
	}			
		
	for (int i=0; i<taille_tableau_classe_i-2;i++){
		if ((tableau_coord[k][0] > tableau_classe_i[i]) && (tableau_coord[k][0] < tableau_classe_i[i+1])){
			*no_rep = 0;
			*ligne_trouvee = i+1;
		}
	}
	
	return;
}

void etiquetage_abscisses(int k, int** tableau_coord, int** tableau_classe_j, int *colonne_trouvee, int *lettre_trouvee, int taille_tableau_coord, int taille_tableau_classe_j, int nb_entrees_tableau_classe_j)
{
	if (tableau_coord[k][1] < tableau_classe_j[0][0]){
		*colonne_trouvee=0;
		*lettre_trouvee=0;
	}
	
	if (tableau_coord[k][1] > tableau_classe_j[taille_tableau_classe_j-1][nb_entrees_tableau_classe_j-1]){
		*colonne_trouvee = taille_tableau_classe_j-1;
		*lettre_trouvee = nb_entrees_tableau_classe_j-1;
	}
	
	for (int c=0; c<taille_tableau_classe_j; c++){ 
		for (int l=0; l<nb_entrees_tableau_classe_j;l++){
					
			if ((l!=nb_entrees_tableau_classe_j-1) && (tableau_coord[k][1] > tableau_classe_j[c][l]) && (tableau_coord[k][1] < tableau_classe_j[c][l+1])){
				*colonne_trouvee = c;
				*lettre_trouvee = l+1;
			}
			
			if ((l==nb_entrees_tableau_classe_j-1) && (c!=taille_tableau_classe_j-1) && (tableau_coord[k][1] > tableau_classe_j[c][l]) && (tableau_coord[k][1] < tableau_classe_j[c+1][0])){
				*colonne_trouvee = c+1;
				*lettre_trouvee = 0;
			}
		}
	}

	return;
}

void etiquetage_pixel(int indice,int** tableau_coord, int* tableau_classe_i, int** tableau_classe_j, char* tableau_reponses, int taille_tableau_coord, int taille_tableau_classe_i, int taille_tableau_classe_j, int nb_entrees_tableau_classe_j, int taille_tableau_reponses, char* tableau_alphabet)
{
	int ligne, colonne, lettre, no_rep;
	int num_question;
	char reponse;
				
	etiquetage_ordonnees(indice,tableau_coord,tableau_classe_i,&ligne,&no_rep,taille_tableau_coord,taille_tableau_classe_i);
	
	if (no_rep == 0){
		
		etiquetage_abscisses(indice,tableau_coord,tableau_classe_j,&colonne,&lettre,taille_tableau_coord,taille_tableau_classe_j,nb_entrees_tableau_classe_j);
	
		num_question = ligne + colonne*(taille_tableau_reponses/taille_tableau_classe_j);
		reponse = correspondance_lettre(lettre,tableau_alphabet);
		
		if ((tableau_reponses[num_question-1] == '0') || (tableau_reponses[num_question-1] == reponse)){
			tableau_reponses[num_question-1]=reponse;			
		}
		else{
			tableau_reponses[num_question-1]='1';
		}
	}
	
	return;
}

void ecriture_etiquetage(char* tableau_reponses, int taille_tableau_reponses)
{
	FILE* fichier_reponses = NULL;
	fichier_reponses = fopen("reponses_qcm2.txt", "w+");
	
	if (fichier_reponses != NULL){
		
		for (int k=0; k<taille_tableau_reponses;k++){
			fprintf(fichier_reponses,"%d\t%c\n",k+1,tableau_reponses[k]);
		}
			
		fclose(fichier_reponses);
	}
	else{
        printf("Impossible d'ouvrir le fichier");
    }
	
	return;		
}

void comparaison_reponses(char* tableau_reponses, char* tableau_reponses_vraies, int taille_tableau, int *score)
{
	*score=0;
	
	FILE* fichier_correction = NULL;
	fichier_correction = fopen("reponses_qcm_corrige.txt", "w+");
	
	if (fichier_correction != NULL){
	
		for (int k=0; k<taille_tableau; k++){
			if (tableau_reponses[k]=='0')
				fprintf(fichier_correction,"Question %d\tPas de réponse\n",k+1);
			if (tableau_reponses[k]=='1')
				fprintf(fichier_correction,"Question %d\tNul\n",k+1);
			if (tableau_reponses[k]==tableau_reponses_vraies[k]){
				fprintf(fichier_correction,"Question %d\tBonne réponse ! (%c)\n",k+1,tableau_reponses_vraies[k]);
				*score=*score+1;
			}
			if ((tableau_reponses[k]!=tableau_reponses_vraies[k]) && (tableau_reponses[k]!='0') && (tableau_reponses[k]!='1'))
				fprintf(fichier_correction,"Question %d\tFAUX (bonne réponse : %c)\n",k+1,tableau_reponses_vraies[k]);
		}
				
		fprintf(fichier_correction,"\nSCORE : %d/%d",*score,taille_tableau);
	
	fclose(fichier_correction);
	}
	else{
        printf("Impossible d'ouvrir le fichier");
    }
	
	return;		
}
			
void lecture_fichier_reponses_vraies(char* tableau_reponses_vraies, int taille_tableau_reponses_vraies)
{	
	int c;
	FILE* fichier_reponses_vraies=NULL;
	fichier_reponses_vraies = fopen("test_reponses.txt","r");
	
	if (fichier_reponses_vraies!=NULL){
		
		for (int k=0; k<taille_tableau_reponses_vraies; k++){
			
			c=getc(fichier_reponses_vraies);
			while (c=='\n'){
				c=getc(fichier_reponses_vraies);
			}
			tableau_reponses_vraies[k]=c;
		}
		
	fclose(fichier_reponses_vraies);
		
	}
	else{
        printf("Impossible d'ouvrir le fichier");
    }		
	
	return;
}

	
		
	
	
