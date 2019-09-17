#include "general_functions.h"

void init_program(int c, char* m, int n, int b) {
	printf("valor de c: %d\n", c);
	printf("valor de n: %d\n", n);
	printf("valor de b: %d\n", b);

	for(int i=1; i<=c; i++){
		char aux1[30] = "testImages/imagen_";
		char iAsString[100];
		sprintf(iAsString, "%d", i);

		// printf("Imagenes a analizar: %s\n", strcat(strcat(aux1, iAsString), ".png"));

		read_image(strcat(strcat(aux1, iAsString), ".png"));
	}
}

void read_image (char *png_file){
    png_structp	png_ptr;
    png_infop info_ptr;
    FILE * fp;
    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    int interlace_method;
    int compression_method;
    int filter_method;
    int j;
    png_bytepp rows;
    fp = fopen (png_file, "r");
    printf("Archivo a analizar: %s \n", png_file);
    if (! fp) {
		printf("Error! no abrí archivo");
		abort();
    }
    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (! png_ptr) {
		printf("Error! no hice el png ptr");
		abort();
    }
    info_ptr = png_create_info_struct (png_ptr);
    if (! png_ptr) {
		printf("Error! no hice el info ptr");
		abort();
    }

    png_init_io (png_ptr, fp);
    png_read_png (png_ptr, info_ptr, 0, 0);
    png_get_IHDR (png_ptr, info_ptr, & width, & height, & bit_depth,
		  & color_type, & interlace_method, & compression_method,
		  & filter_method);
    rows = png_get_rows (png_ptr, info_ptr);
    printf ("Width is %d, height is %d\n", width, height);
    int rowbytes;
    rowbytes = png_get_rowbytes (png_ptr, info_ptr);
    printf ("Row bytes = %d\n", rowbytes);
    for (j = 0; j < height; j++) {
	int i;
	png_bytep row;
	row = rows[j];
	for (i = 0; i < rowbytes; i++) {
	    png_byte pixel;
	    pixel = row[i];
	    if (pixel < 64) {
		printf ("##");
	    }
	    else if (pixel < 128) {
		printf ("**");
	    }
	    else if (pixel < 196) {
		printf ("..");
	    }
	    else {
		printf ("  ");
	    }

	}
	printf ("\n");
    }
}