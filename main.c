#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <time.h>

typedef int myarray_type;
const myarray_type ZERO_BIT = 0x00;
const myarray_type ONE_BIT = 0x01;
myarray_type in_set(const double *x, const double *y, const int *max_iter) {
  double complex pt = *x + (*y)*I;
  double complex z = 0;
  for(int i = 0; i < (*max_iter); i++) {
    z = z*z + pt;
  }
  if(cabs(z) > 2) {
    return ZERO_BIT;
  }
  return ONE_BIT;
}

int main(int argc, char *argv[]) {
  /**
     instead of properly parsing argumnets, let's go with
     command x_min y_min x_max y_max n_xpts n_ypts max_iter n_threads
  **/
  const double x_min = atof(argv[1]);
  const double y_min = atof(argv[2]);
  const double x_max = atof(argv[3]);
  const double y_max = atof(argv[4]);
  const int n_initial_xpts = atoi(argv[5]);
  const int n_initial_ypts = atoi(argv[6]);
  const int max_iter = atoi(argv[7]);
  const int n_threads = atoi(argv[8]);
  const int BITS_PER_ENTRY = 8*sizeof(myarray_type);
  const int n_xpts = n_initial_xpts - (n_initial_xpts%BITS_PER_ENTRY);
  const int n_ypts = n_initial_ypts;// - (n_initial_ypts%8);
  myarray_type **values = (myarray_type**) calloc(n_ypts, sizeof(myarray_type));
  for(int i = 0; i < n_ypts; i++) {
    values[i] = (myarray_type *) calloc(n_xpts/BITS_PER_ENTRY, sizeof(myarray_type));
  }
  int i, j;
  int j_byteindex, bitshift;
  for(i = 0; i < n_ypts; i++) {
      for(j = 0; j < n_xpts; j++) {
	  j_byteindex = j/BITS_PER_ENTRY;
	  bitshift = j%BITS_PER_ENTRY;
	  if(values[i][j_byteindex] & (0x01 << bitshift)) {
	      printf("%d", 1);
	  }
	  else {
	      printf("%d", 0);
	  }
      }
      printf("\n");
  }
  /**
  for(i = 0; i < n_ypts; i++) {
    for(j = 0; j < n_xpts; j++) {
      j_byteindex = j/BITS_PER_ENTRY;
      bitshift = j%BITS_PER_ENTRY;
      values[i][j_byteindex] &= (0 << bitshift);
    }
  }
  **/
  /**
  for(int i = 0; i < n_ypts; i++) {
    for(int j = 0; j < n_xpts/BITS_PER_ENTRY; j++) {
      values[i][j] = (myarray_type) '0';
    }
  }
  **/
  double x_loc = x_min;
  double y_loc = y_max;
  double x_inc = (x_max-x_min)/(n_xpts-1);
  double y_inc = (y_max-y_min)/(n_ypts-1);
  clock_t start = clock();
#pragma omp parallel default(shared) private(i, j, j_byteindex, bitshift) num_threads(n_threads)
  {
      for(i = 0; i < n_ypts; i++) {
	  x_loc = x_min;
#pragma omp for schedule(dynamic, BITS_PER_ENTRY) ordered
	  for(j = 0; j < n_xpts; j++) {
	      j_byteindex = j/BITS_PER_ENTRY;
	      bitshift = j%BITS_PER_ENTRY;
	      values[i][j_byteindex] |= (((myarray_type) in_set(&x_loc, &y_loc, &max_iter)) << bitshift);
	      x_loc += x_inc;
	  }
	  y_loc -= y_inc;
      }
  }
  clock_t end = clock();
  double elapsed_time = (end-start)/CLOCKS_PER_SEC;

  char mandset_outputfilename[1024];
  snprintf(mandset_outputfilename, sizeof mandset_outputfilename, "%s%d%s%d%s%d%s", "./data/omp_mandelbrodt_numthreads_", n_threads, "_nnodes_", n_xpts*n_ypts, "_maxiter_", max_iter , ".csv");
  FILE *mandsetvals_file = fopen(mandset_outputfilename, "w");
  /**
  for(i = 0; i < n_ypts; i++) {
    for(j = 0; j < n_xpts; j++) {
	   fprintf(mandsetvals_file, "%d", values[i][j]);
	   if(j != n_xpts-1) {
	   fprintf(mandsetvals_file, ",");
      }
    }
    fprintf(mandsetvals_file, "\n");
  }
  **/
  for(i = 0; i < n_ypts; i++) {
      for(j = 0; j < n_xpts; j++) {
	  j_byteindex = j/BITS_PER_ENTRY;
	  bitshift = j%BITS_PER_ENTRY;
	  if((1 << bitshift) == (values[i][j_byteindex] & (1 << bitshift))) {
	      printf("%i", 1);
	  }
	  else {
	      printf("%i", 0);
	  }
      }
      printf("\n");
  }
  printf("\n");
  fclose(mandsetvals_file);
  char times_outputfilename[1024];
  snprintf(times_outputfilename, sizeof times_outputfilename, "%s%s", "./data/omp_mandelbrodt_times", ".csv");
  FILE *times_file = fopen(times_outputfilename, "a");
  fprintf(times_file, "%-7d\t%-8d\t%-2d\t%f\n", n_xpts, max_iter, n_threads, elapsed_time);
  fclose(times_file);
  /**
  for(i = 0; i < n_ypts/8; i++) {
    free(values[i]);
  }
  free(values);
  **/
  return 0;
}
