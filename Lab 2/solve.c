/*
 *  DANIEL RIVERA RUIZ
 *  drr342@nyu.edu
 *  Multicore Processors
 *  Spring 2019
 *  Lab 2
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

// This constant was defined because the 100,000 problem wasn't converging to 0.001 accuracy,
// it got stuck at 0.004 and oscilated infinitely...
#define MAX_ITER 30

/***** Globals ******/
float **a; /* The coefficients */
float *x;  /* The unknowns */
float *b;  /* The constants */
float *sum;
float *x_old;
float err; /* The absolute relative error */
int num;  /* number of unknowns */
int equations_per_rank;
int offset;

/****** Function declarations */
int read_input (char* filename, const int rank, const int size);
int allocate_memory ();
void writeSolution (int nit);

/* Function definitions: ****/
/*****************************************************************/

int read_input (char* filename, const int rank, const int size) {
    // Open input file
    int error = 0;
    FILE *in = fopen(filename, "r");
    if (!in)
        error = 1;
    MPI_Allreduce(MPI_IN_PLACE, &error, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
    if (error) {
        if (rank == 0)
            fprintf(stderr, "Couldn't read input file %s\n", filename);
        MPI_Finalize();
        exit(2);
    }

    // get num and err
    fscanf(in, "%d ", &num);
    fscanf(in, "%f ", &err);

    if (size > num) {
        if (rank == 0)
            fprintf(stderr, "More processes (%d) than unknowns(%d) not supported!\n", size, num);
        MPI_Finalize();
        exit(3);
    }

    // get equations and offset in x per rank
    equations_per_rank = num / size;
    if (rank < num - (equations_per_rank * size))
        equations_per_rank++;
    MPI_Scan(&equations_per_rank, &offset, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    offset -= equations_per_rank;

    // allocate memory
    error = allocate_memory();
    MPI_Allreduce(MPI_IN_PLACE, &error, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
    if (error) {
        if (rank == 0)
            fprintf(stderr, "Couldn't allocate memory!\n");
        MPI_Finalize();
        exit(3);
    }

    // get x_old and position in file
    for (int i = 0; i < num; i++)
        fscanf(in, "%f ", &x_old[i]);
    long globalstart = ftell(in);

    // get file size and size of line
    fseek(in, 0, SEEK_END);
    long filesize = ftell(in);
    int size_of_line = (filesize - globalstart + 1) / num;

    // get start position for rank
    if (rank != 0)
        globalstart += (long)size_of_line * (long)offset - ((long)size_of_line / 2L);

    // set start position in file
    fseek(in, globalstart, SEEK_SET);
    // move position in file to real start
    if (rank != 0)
        while (fgetc(in) != '\n') {}
    // read input data and test convergence
    int bigger = 1;
    for(int i = 0; i < equations_per_rank; i++) {
        float sum_row = 0.0;
        for(int j = 0; j < num; j++) {
            fscanf(in, "%f ", &a[i][j]);
            sum_row += fabs(a[i][j]);
        }
        float aii = fabs(a[i][i + offset]);
        sum_row -= aii;
        if (aii < sum_row) {
            bigger = 0;
            break;
        }
        fscanf(in, "%f ", &b[i]);
    }

    MPI_Allreduce(MPI_IN_PLACE, &bigger, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
    if (!bigger) {
        if (rank == 0)
            fprintf(stderr, "Problem will not converge!\n");
        MPI_Finalize();
        exit(4);
    }

    fclose(in);
    return 0;
}

int allocate_memory () {
    a = (float**) malloc(equations_per_rank * sizeof(float*));
    x = (float*) malloc(num * sizeof(float));
    b = (float*) malloc(equations_per_rank * sizeof(float));
    sum = (float*) malloc(equations_per_rank * sizeof(float));
    x_old = (float*) malloc(num * sizeof(float));
    if (!a || !x || !b || !sum || !x_old)
        return 1;
    for (int i = 0; i < equations_per_rank; i++) {
        a[i] = (float*) malloc(num * sizeof(float));
        if (!a[i])
            return 1;
    }
    return 0;
}

void writeSolution (int nit) {
    FILE * fp;
    char output[100] ="";
    sprintf(output, "%d.sol", num);
    fp = fopen(output, "w");

    if(!fp) {
        printf("Cannot create the file %s\n", output);
        return;
    }
    for (int i = 0; i < num; i++)
        fprintf(fp, "%f\n", x_old[i]);

    printf("total number of iterations: %d\n", nit);
    fclose(fp);
}

int main(int argc, char **argv) {

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0)
            fprintf(stderr, "Usage: %s filename\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    read_input(argv[1], rank, size);

    // if (rank == 0)
    //     printf("data loaded successfully!\n");

    // solve problem
    int nit = 0; /* number of iterations */
    float max_err;
    int *equations_all = (int*) malloc(size * sizeof(int));
    int *offset_all = (int*) malloc(size * sizeof(int));
    MPI_Allgather(&equations_per_rank, 1, MPI_INT, equations_all, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Allgather(&offset, 1, MPI_INT, offset_all, 1, MPI_INT, MPI_COMM_WORLD);
    do {
        nit++;
        max_err = 0.0;
        for (int i = 0; i < equations_per_rank; i++) { 
            sum[i] = 0.0;
            for (int j = 0; j < num; j++)
                sum[i] += a[i][j] * x_old[j];
            x[i + offset] = (b[i] - (sum[i] - a[i][i + offset] * x_old[i + offset])) / a[i][i + offset];
        }
        for (int i = 0; i < equations_per_rank; i++) {
            max_err = fabs((x[i + offset] - x_old[i + offset]) / x[i + offset]);
            if (max_err > err)
                break;
        }
        MPI_Allgatherv(&x[offset], equations_per_rank, MPI_FLOAT, x_old, equations_all, offset_all, MPI_FLOAT, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, &max_err, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);
        // if (rank == 0)
        //     printf("iteration %d, error = %g\n", nit, max_err);
    } while (max_err > err && nit < MAX_ITER);

    /* Writing results to file */
    if (rank == 0)
        writeSolution(nit);

    // free memory
    free(equations_all);
    free(offset_all);
    for (int i = 0; i < equations_per_rank; i++)
        free(a[i]);
    free(a);
    free(x);
    free(b);
    free(sum);
    free(x_old);   

    MPI_Finalize();
    return 0;
}