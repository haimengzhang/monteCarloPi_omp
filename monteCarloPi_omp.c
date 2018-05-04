#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define DEFAULT_THROWS 500000
#define PI     3.1415926535
#define DEFAULT_THREADS 1

/***  OMP ***/
 // number of threads to use
 unsigned int seedsx[DEFAULT_THREADS];
 unsigned int seedsy[DEFAULT_THREADS];

void seedxThreads() {
    int my_thread_id;
    unsigned int seedx;

    #pragma omp parallel private (seedx, my_thread_id)
    {
        my_thread_id = omp_get_thread_num();

        //create seed on thread using current time
           unsigned int seedx = (unsigned) time(NULL)+ 120975;

        //munge the seed using our thread number so that each thread has its
        //own unique seed, therefore ensuring it will generate a different set of numbers
        seedsx[my_thread_id] = (seedx & 0xFFFFFFF0) | (my_thread_id + 1);

        // printf("Thread %d has seedx %u\n", my_thread_id, seedsx[my_thread_id]);
    }

}
void seedyThreads() {
    int my_thread_id;
    unsigned int seedy;

    #pragma omp parallel private ( seedy, my_thread_id)
    {
        my_thread_id = omp_get_thread_num();

        //create seed on thread using current time
          unsigned int seedy = (unsigned) time(NULL)- 120975;

        //munge the seed using our thread number so that each thread has its
        //own unique seed, therefore ensuring it will generate a different set of numbers
        seedsy[my_thread_id] = (seedy & 0xFFFFFFF0) | (my_thread_id + 1);

        // printf("Thread %d has seedy %u\n", my_thread_id, seedsy[my_thread_id]);
    }

}
/***  OMP ***/

//----------------------------------------------Main--------------------------------------------
int main(int argc, char** argv) {
    long numSamples = DEFAULT_THROWS;
    long numInCircle = 0;       //number of throws in the unit circle
    double x, y;                //hold x,y position of each sample 'throw'
    unsigned int seedx;        //original thread seed used to generate seeds
    unsigned int seedy;

    /***  OMP ***/
    // We will use these to time our code
    double begin, end;
    double time_spent;
    int tid;
    /***  OMP ***/
    unsigned int numThreads;

    // take in how many 'throws', or samples
    if (argc > 1) {
        numSamples = strtol(argv[1], NULL, 10);
    }
    /* parse command-line arg for number of threads */
    if (argc > 2) {
      numThreads = atoi(argv[2]);
      seedsx[numThreads];
      seedsy[numThreads];
    }

    #ifdef _OPENMP
      // printf("OMP defined, numThreads = %d\n", numThreads);
      omp_set_num_threads(numThreads);
      seedxThreads();
      seedyThreads();
    #else
      // printf("OMP not defined\n");
    #endif

      int n;

      begin = omp_get_wtime(); // start the timing
  ///////////  work being done that we will time

    #pragma omp parallel num_threads(numThreads) default(none)\
    private (tid, n, x, y, seedx, seedy) \
    shared(numSamples, seedsx, seedsy)\
    reduction(+:numInCircle)

    // rand_r is lots of times of generating
    //srand is one time

    {
      tid = omp_get_thread_num();
      seedx = seedsx[tid];
      seedy = seedsy[tid];
      srand(seedx);
      srand(seedy);


    #pragma omp for
    for(n=0; n<numSamples; n++) {
      // generate randome numbers between 0.0 and 1.0
      x = (double)rand_r(&seedx)/RAND_MAX;
      y = (double)rand_r(&seedy)/RAND_MAX;

      if ( (x*x + y*y) <= 1.0 ) {
        numInCircle++;
      }
    }
  }
    double pi = 4.0 * (double)(numInCircle) / (numSamples);

// completion of work
  end = omp_get_wtime(); // end the timing
  time_spent = (double)(end - begin);
  //
  // printf("Calculation of pi using %ld samples: %15.14lf\n", numSamples, pi);
  // printf("Accuracy of pi calculation: %lf\n", pi - PI);
  // printf("Time spent: %15.12lf seconds\n", time_spent);
  //
  // printf("\n\n % %15s       %15s       %15s         %15s",
  //          "SampleDots","pi","Accuracy","Time (sec)\n");

  printf(" %d,         %ld,        %15.14lf,     %lf,     %15.12lf\n", numThreads, numSamples, pi, pi - PI, time_spent);


}
