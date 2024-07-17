#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define MAXSIZE 10

    //GLOBAL VARIABLES FOR MATRICES AND THREADS
    int matrixA[MAXSIZE][MAXSIZE];
    int matrixB[MAXSIZE][MAXSIZE];
    int outMatrix[MAXSIZE][MAXSIZE];

    //KEEPING TO MULTIPLY AND PRINT TO THE FILE MATRICES 
    int arow, acol;
    int bcol, brow;

void *readFile(void *filename){
    FILE *p = NULL;
    char *fname = filename;         //reads from the input text and assigns it to the global matrix array. 
    int i, j;
    p = fopen(filename, "r");

    fscanf(p, "%d", &arow);         // it takes first two integer which are row and column count.
    fscanf(p, "%d", &acol);

    if (p  != NULL)
    {
        for (i = 0; i < arow; i++)
        {
            for (j = 0; j < acol; j++)
            {
                fscanf(p, "%d", &matrixA[i][j]);
                //printf("%d ", matrixA[i][j]);
            }
            //printf("\n");
        }
    }
    else
    {
        printf("*** %s file can not be found!", fname);
    }
    fclose(p);
    pthread_exit(0);

}

void *readFile2(void *filename){
    FILE *p = NULL;
    int i, j;
    p = fopen(filename, "r");           //reads from the input text and assigns it to the global matrix array. 
    char *fname = filename;
    fscanf(p, "%d", &brow);
    fscanf(p, "%d", &bcol);             // it takes first two integer which are row and column count.

    if (p  != NULL)
    {
        for (i = 0; i < brow; i++)
        {
            for (j = 0; j < bcol; j++)
            {
                fscanf(p, "%d", &matrixB[i][j]);
                //printf("%d ", matrixB[i][j]);
            }
            //printf("\n");
        }
    }
    else
    {
        printf("*** %s file can not be found!", fname);
    }
    fclose(p);
    pthread_exit(0);

}

void *multiplicationOfMatrix(){
    //Matrix Multiplication
    // if first matrix's column and second matrix's row are equal, it computes multiplication and assings final matrix to the global outmatrix.
	 if(acol != brow){
	 	printf("Matrix Multiplication is not possible!\n");
	 }else{
	 	for(int i=0; i<arow; i++){
	 		for(int j=0; j<bcol; j++){
	 			int sum = 0;
	 			for(int k=0; k<acol; k++){
	 				sum = sum + matrixA[i][k] * matrixB[k][j];
	 			}
                outMatrix[i][j] = sum;
	 		}
	 	}
	 }	
    
}  

int main (int argc, char *argv[]){

    // creates threads to do job(reading inputs and calculating)

    pthread_t readerth1; /* the thread identifier */
    pthread_attr_t attr; /* set of thread attributes */

    pthread_attr_init(&attr);


    pthread_create(&readerth1, &attr, readFile, argv[1]); // it reads first input.txt


    pthread_t readerth2; /* the thread identifier */
    pthread_create(&readerth2, &attr, readFile2, argv[2]); //it reads second input.txt

    pthread_join(readerth1, NULL);      // before multiplying they wait thread1 and thread2 to complete task.
    pthread_join(readerth2, NULL);

    pthread_t multiplierth; /* the thread identifier */
    pthread_create(&multiplierth, &attr, multiplicationOfMatrix,NULL); //it multiplies matrices.
    pthread_join(multiplierth, NULL); // it waits finishing job.

    // main process writes the multiplied matrix to the file.
    FILE *fptr;
    fptr = fopen(argv[3],"w");

    fprintf(fptr, "%d %d\n", arow, bcol);
    int i,j;
    for (i = 0; i < arow; i++)
    {
        for ( j= 0; j < bcol; j++)
        {
            fprintf(fptr,"%d ",outMatrix[i][j]);
            //printf("%d ", outMatrix[i][j]);
        }
        fprintf(fptr,"\n");
    }
        printf("File has been created as %s \n", argv[3]);

  
    

    return 0;
}   

