#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    pid_t childID, childID2, childID3;

    int fd[2], fd1[2], fd2[2]; // fd[0] for reading
                               // fd[1] for writing
    // fd and fd1 are for reading part, fd2 sends multplied matrix to the main process    

    // struct to keep matrix variables
    typedef struct
    {
        int row, col;
        int matrix[10][10];
    } Matrix;
    // checking the pipes
    if (pipe(fd) == -1)
    {
        printf("An error happened\n");
        return 1;
    }
    if (pipe(fd1) == -1)
    {
        printf("An error happened\n");
        return 1;
    }
    if (pipe(fd2) == -1)
    {
        printf("An error happened\n");
        return 1;
    }

    childID = fork();
    if (childID == 0)
    {
        // child1
        //  reading matrixA
        Matrix matrixA;

        FILE *p = NULL;                         //reads from the input text and assigns to the matrix struct. 
        int i, j;
        p = fopen(argv[1], "r");
        char *fname = argv[1];
        fscanf(p, "%d", &matrixA.row);
        fscanf(p, "%d", &matrixA.col);

        //printf("arow: %d\n", matrixA.row);
        //printf("acol: %d\n", matrixA.col);

        if (p != NULL)
        {
            for (i = 0; i < matrixA.row; i++)
            {
                for (j = 0; j < matrixA.col; j++)
                {
                    fscanf(p, "%d", &matrixA.matrix[i][j]);
                    //printf("%d ", matrixA.matrix[i][j]);
                }
                //printf("\n");
            }
        }
        else
        {
            printf("*** %s file can not be found!", fname);
        }
        fclose(p);

        close(fd[0]);                           // it writes matrixA to use in matrix multplication section.
        write(fd[1], &matrixA, sizeof(Matrix));
        close(fd[1]);
    }
    else
    {
        childID2 = fork();
        if (childID2 == 0)
        {
            // child2
            // reading matrixB

            Matrix matrixB;
            // int brow = matrixB.row, bcol = matrixB.col;

            FILE *p = NULL;                     //reads from the input text and assigns to the matrix struct. 
            int i, j;
            p = fopen(argv[2], "r");
            char *fname = argv[2];
            fscanf(p, "%d", &matrixB.row);
            fscanf(p, "%d", &matrixB.col);

            //printf("brow: %d\n", matrixB.row);
            //printf("bcol: %d\n", matrixB.col);

            if (p != NULL)
            {
                for (i = 0; i < matrixB.row; i++)
                {
                    for (j = 0; j < matrixB.col; j++)
                    {
                        fscanf(p, "%d", &matrixB.matrix[i][j]);
                        //printf("%d ", matrixB.matrix[i][j]);
                    }
                    //printf("\n");
                }
            }
            else
            {
                printf("*** %s file can not be found!", fname);
            }
            fclose(p);

            close(fd1[0]);

            write(fd1[1], &matrixB, sizeof(Matrix));     // it writes matrixA to use in matrix multplication section.
            close(fd1[1]);
        }
        else
        {
            childID3 = fork();
            if (childID3 == 0)
            {
                // child3
                // multiplication of matrices

                Matrix matrixNewA, matrixNewB, multipliedMatrix;

                // reading the matrixA
                close(fd[1]);
                read(fd[0], &matrixNewA, sizeof(Matrix));
                close(fd[0]);
                
                //printf("chlid1 den geldi arow, child3teyiz: %d\n", matrixNewA.col);

                // reading the matrixB
                close(fd1[1]);
                read(fd1[0], &matrixNewB, sizeof(Matrix));
                close(fd1[0]);
                
                //printf("chlid2 den geldi bcol, childd3teyiz: %d\n", matrixNewB.col);

                multipliedMatrix.row = matrixNewA.row;
                multipliedMatrix.col = matrixNewB.col;

                //printf("multi row:%d  , multi col:%d \n", multipliedMatrix.row, multipliedMatrix.col);

                // MULTIPLYING
                // it reads from pipes which are fd and fd1. Then multiplies the both matrices
                if (matrixNewA.col != matrixNewB.row)
                {
                    printf("Matrix Multiplication is not possible!\n");
                }
                else
                {
                    for (int i = 0; i < matrixNewA.row; i++)
                    {
                        for (int j = 0; j < matrixNewB.col; j++)
                        {
                            int sum = 0;
                            for (int k = 0; k < matrixNewA.col; k++)
                            {
                                sum = sum + matrixNewA.matrix[i][k] * matrixNewB.matrix[k][j];
                            }
                            multipliedMatrix.matrix[i][j] = sum;
                        }
                    }
                }
                //it writes multiplied matrix for main process which is gonna write to the file.
                close(fd2[0]);
                write(fd2[1], &multipliedMatrix, sizeof(Matrix));
                close(fd2[1]);
            }
            else
            {
                // main process
                // write to the file.
                Matrix finalMatrix;

                // reading the multiplied matrix from the child3
                close(fd2[1]);
                read(fd2[0], &finalMatrix, sizeof(Matrix));
                close(fd2[0]);


                // writing to the file 
                FILE *fptr;
                fptr = fopen(argv[3], "w");

                fprintf(fptr, "%d %d\n", finalMatrix.row, finalMatrix.col);
                int i, j;
                for (i = 0; i < finalMatrix.row; i++)
                {
                    for (j = 0; j < finalMatrix.col; j++)
                    {
                        fprintf(fptr, "%d ", finalMatrix.matrix[i][j]);
                        // printf("%d ", outMatrix[i][j]);
                    }
                    fprintf(fptr, "\n");
                }
                printf("File has been created as %s \n", argv[3]);

                wait(NULL);
            }
        }
    }
    return 0;
}