#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <time.h>
#include "png.h"
#include "writepng.h"

int in_set(const double *x, const double *y, const int *max_iter) {
    double complex pt = *x + (*y)*I;
    double complex z = 0;
    for(int i = 0; i < (*max_iter); i++) {
	z = z*z + pt;
    }
    if(cabs(z) > 2) {
	return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    /**
       instead of properly parsing argumnets, let's go with
       command x_min y_min x_max y_max n_xpts n_ypts max_iter nthreads
    **/
    const double x_min = atof(argv[1]);
    const double y_min = atof(argv[2]);
    const double x_max = atof(argv[3]);
    const double y_max = atof(argv[4]);
    const int n_xpts = atoi(argv[5]);
    const int n_ypts = atoi(argv[6]);
    const int max_iter = atoi(argv[7]);
    const int nthreads = atoi(argv[8]);
    png_byte **values = (png_byte**) malloc(n_ypts*sizeof(png_byte*));
    for(int i = 0; i < n_xpts; i++) {
	values[i] = (png_byte *) malloc(n_xpts*sizeof(png_byte));
    }
    double x_inc = (x_max-x_min)/(n_xpts-1);
    double y_inc = (y_max-y_min)/(n_ypts-1);
    int i, j;
    time_t t1, t2;
    t1 = time(NULL);
#pragma omp parallel for num_threads(nthreads) private(i, j) schedule(dynamic)
	for(i = 0; i < n_ypts; i++) {
	    for(j = 0; j < n_xpts; j++) {
		double x_loc = x_min+j*x_inc;
		double y_loc = y_max-i*y_inc;
		values[i][j] = 255*in_set(&x_loc, &y_loc, &max_iter);
	    }
	}
    t2 = time(NULL);
    double elapsed_time = difftime(t2, t1);

    char times_outputfilename[1024];
    snprintf(times_outputfilename, sizeof times_outputfilename, "%s%s", "./data/omp_mandelbrodt_times", ".csv");
    FILE *times_file = fopen(times_outputfilename, "a");
    fprintf(times_file, "%-7d\t%-8d\t%-2d\t%f\n", n_xpts, max_iter, nthreads, elapsed_time);
    fclose(times_file);

    char png_filename[1024];
    snprintf(png_filename, sizeof png_filename, "%s%i%s%i%s", "./figs/omp_plot_", n_xpts, "_", nthreads, ".png");
    FILE *png_img = fopen(png_filename, "wb");
    png_structp ppng = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop ipng = png_create_info_struct(ppng);
    if (setjmp(png_jmpbuf(ppng))) {
        png_destroy_write_struct(&ppng, &ipng);
        return 2;
    }
    png_init_io(ppng, png_img);
    png_set_IHDR(ppng, ipng, n_xpts, n_ypts, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(ppng, ipng);
    for(i = 0; i < n_ypts; i++) {
	png_write_row(ppng, values[i]);
    }
    png_write_end(ppng, NULL);

    for(i = 0; i < n_xpts; i++) {
	free(values[i]);
    }
    free(values);
    return 0;
}
