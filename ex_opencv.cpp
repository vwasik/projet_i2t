#include <stdio.h> // Pour le printf()...

/* Les deux bibliothèques nécessaires d'opencv :
	- cv.h contient les structures et fonctions de manipulation d'images
	- highgui.h contient les fonctions d'affichage des images
*/
#include <cv.h>
#include <highgui.h>

int main(int argc, char *argv[])
{
 // Initialization :
	IplImage* src;
	
	// Read in the source image to be searched
	src = cvLoadImage("calvin_et_hobbes.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	
	if(!src){
		printf("arrrg");
		exit(0);
	}
	
	cvNamedWindow( "Image", CV_WINDOW_AUTOSIZE );
	printf("toto");
	cvShowImage( "Image", src );
	printf("tutu");
	
	cvWaitKey(0);
	
	return 0;
}
