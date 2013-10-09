#include "mpi.h"
#include <stdint.h>
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
     command x_min y_min x_max y_max n_xpts n_ypts max_iter ncores
  **/
    const int STOP = 14587312;
	const double ROUND_TO_NEAREST_INT = 0.5;
  const double x_min = atof(argv[1]);
  const double y_min = atof(argv[2]);
  const double x_max = atof(argv[3]);
  const double y_max = atof(argv[4]);
  const int n_xpts = atoi(argv[5]);
  const int n_ypts = atoi(argv[6]);
  const int max_iter = atoi(argv[7]);
  const int nnodes = atoi(argv[8]);
  const int ncores = atoi(argv[9]);
  int ntasks, rank;
  time_t t1, t2;
  int i, j;
  png_byte **values;
  int mpi_check = MPI_Init(&argc, &argv);
  if(mpi_check != MPI_SUCCESS) {
      MPI_Abort(MPI_COMM_WORLD, mpi_check);
      return 1;
  }
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Status status;
  if(rank == 0) {
      double x_loc = x_min;
      double y_loc = y_max;
      double x_inc = (x_max-x_min)/(n_xpts-1);
      double y_inc = (y_max-y_min)/(n_ypts-1);
      int init_count = 0;
      int outcome;
    int **thread_tracker = (int**) malloc((ntasks-1)*sizeof(int*));
    for(i = 0; i < ntasks-1; i++) {
      thread_tracker[i] = (int*) malloc(2*sizeof(int));
    }
      values = (png_byte**) calloc(n_ypts, sizeof(png_byte*));
      for(i = 0; i < n_ypts; i++) {
	  values[i] = (png_byte *) calloc(n_xpts , sizeof(png_byte));
      }
      t1 = time(NULL);
      for(i = 0; i < n_ypts; i++) {
	  x_loc = x_min;
	  for(j = 0; j < n_xpts; j++) {
	      if(init_count < ntasks-1) {
		  double xy[2] = {x_loc, y_loc};
		  MPI_Send(&xy, 2, MPI_DOUBLE, init_count+1, 0, MPI_COMM_WORLD);
		  thread_tracker[init_count][0] = i;
		  thread_tracker[init_count][1] = j;
		  init_count++;
		  x_loc += x_inc;
	      }
	      else {
		  MPI_Recv(&outcome, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		  int current_thread = status.MPI_SOURCE;
		  int recvd_i = thread_tracker[current_thread-1][0];
		  int recvd_j = thread_tracker[current_thread-1][1];
		  values[recvd_i][recvd_j] = 255*outcome;
		  double xy[2] = {x_loc, y_loc};
		  MPI_Send(xy, 2, MPI_DOUBLE, current_thread, 0, MPI_COMM_WORLD);
		  x_loc += x_inc;
		  thread_tracker[current_thread-1][0] = i;
		  thread_tracker[current_thread-1][1] = j;
	      }
	  }
	  y_loc -= y_inc;
      }
      t2 = time(NULL);
      double xy[2] = {STOP, STOP};
      for(int thread = 0; thread < ntasks-1; thread++) {
	  MPI_Send(xy, 2, MPI_DOUBLE, thread+1, 0, MPI_COMM_WORLD);
      }
      for(i = 0; i < ntasks-1; i++) {
	free(thread_tracker[i]);
      }
      free(thread_tracker);
  }
  else {
      double xy[2];
      MPI_Recv(xy, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
      while(((int) (xy[0] + ROUND_TO_NEAREST_INT)) != STOP) {
	  int outcome = in_set(&xy[0], &xy[1], &max_iter);
	  MPI_Send(&outcome, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	  MPI_Recv(xy, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
      }
  }
  MPI_Finalize();

  if(rank == 0) {
    double elapsed_time = difftime(t2, t1);
    char times_outputfilename[1024];
    snprintf(times_outputfilename, sizeof times_outputfilename, "%s%s", "./data/mpi_mandelbrodt_times", ".csv");
    FILE *times_file = fopen(times_outputfilename, "a");
    fprintf(times_file, "%-7d\t%-8d\t%-2d\t%f\n", n_xpts, max_iter, nnodes*ncores, elapsed_time);
    fclose(times_file);

    char png_filename[1024];
    snprintf(png_filename, sizeof png_filename, "%s%i%s%i%s%i%s", "./figs/mpi_plot_", n_xpts, "_", nnodes, "_", ncores, ".png");
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

    for(i = 0; i < n_ypts; i++) {
      free(values[i]);
    }
    free(values);
    return 0;
  }
}
