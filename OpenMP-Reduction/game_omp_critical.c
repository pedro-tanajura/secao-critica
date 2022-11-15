#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <sys/time.h>

#define SIZE 2048
#define ITERATIONS 2000

#define THREADS 1
int SUM = 0;

void initializeGrid(bool **grid);
void freeGridMemory(bool **grid);
void createGlider(bool **grid);
void createRPentonimo(bool **grid);
int getNeighbors(bool **grid, int i, int j);
void calculateNewGridState(bool **grid1, bool **grid2, int startIdx, int endIdx);
int aliveCounter(bool **grid);
void executeGame(bool **grid1, bool **grid2);

void initializeGrid(bool **grid)
{
    int i, j;

    for (i = 0; i<SIZE; i++)
        grid[i] = malloc(SIZE * sizeof (bool)) ;

    for (i = 0; i<SIZE; i++)
        for (j = 0; j<SIZE; j++)
            grid[i][j] = 0;
}

void freeGridMemory(bool **grid)
{
    int i;

    for (i = 0; i<SIZE; i++)
        free(grid[i]);
    free(grid);
}

void createGlider(bool **grid)
{
    int lin = 1, col = 1;

    grid[lin  ][col+1] = 1;
    grid[lin+1][col+2] = 1;
    grid[lin+2][col  ] = 1;
    grid[lin+2][col+1] = 1;
    grid[lin+2][col+2] = 1;
}

void createRPentonimo(bool **grid)
{
    int lin =10, col = 30;

    grid[lin  ][col+1] = 1;
    grid[lin  ][col+2] = 1;
    grid[lin+1][col  ] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;
}

int getNeighbors(bool **grid, int i, int j)
{
    if(i != 0 && j != 0 && i != SIZE-1 && j != SIZE-1)
    {
        return  grid[i-1][j-1] +    //top left
                grid[i-1][j] +      //top middle
                grid[i-1][j+1] +    //top right
                grid[i][j-1] +      //middle left
                grid[i][j+1] +      //middle right
                grid[i+1][j-1] +    //bottom left
                grid[i+1][j] +      //bottom middle
                grid[i+1][j+1];     //bottom right
    }

    return  grid[(i-1+SIZE)%SIZE][(j-1+SIZE)%SIZE] +    //top left
            grid[(i-1+SIZE)%SIZE][j] +                  //top middle
            grid[(i-1+SIZE)%SIZE][(j+1)%SIZE] +         //top right
            grid[i][(j-1+SIZE)%SIZE] +                  //middle left
            grid[i][(j+1)%SIZE] +                       //middle right
            grid[(i+1)%SIZE][(j-1+SIZE)%SIZE] +         //bottom left
            grid[(i+1)%SIZE][j] +                       //bottom middle
            grid[(i+1)%SIZE][(j+1)%SIZE];               //bottom right
}

void calculateNewGridState(bool **grid1, bool **grid2, int startIdx, int endIdx)
{
    int i, j;
    int neighbors;

    for(i = startIdx; i<endIdx; i++)
    {
        for(j = 0; j<SIZE; j++)
        {
            neighbors = getNeighbors(grid1, i, j);

            if(grid1[i][j])
            {
                if(neighbors == 2 || neighbors == 3)
                {
                    grid2[i][j] = 1;
                    continue;
                }

                grid2[i][j] = 0;
                continue;
            }

            if(neighbors == 3)
            {
                grid2[i][j] = 1;
                continue;
            }

            grid2[i][j] = 0;
        }
    }
}

int aliveCounter(bool **grid)
{
    int i, j;

    int threadInterval = SIZE/THREADS;

    #pragma omp parallel private(i, j)
    {
        int id = omp_get_thread_num();

        for(i = id*threadInterval; i<(id*threadInterval) + threadInterval; i++)
            for(j = 0; j<SIZE; j++)
                #pragma omp critical
                    SUM += grid[i][j];
    }
    return SUM;
}

void executeGame(bool **grid1, bool **grid2)
{
    int i, startIdx, endIdx;
    bool **aux;
    int threadInterval = SIZE/THREADS;

    #pragma omp parallel shared(threadInterval, grid1, grid2, aux) private(i, startIdx, endIdx)
    {
        startIdx = omp_get_thread_num() * threadInterval;
        endIdx = startIdx + threadInterval;

        for(i = 0; i<ITERATIONS; i++)
        {
            calculateNewGridState(grid1, grid2, startIdx, endIdx);
            #pragma omp barrier

            #pragma omp single
            {
                aux = grid1;
                grid1 = grid2;
                grid2 = aux;
            }
            #pragma omp barrier
        }
    }
}

int main()
{
    omp_set_num_threads(THREADS);
    bool **grid1, **grid2;
    int i, totalSum = 0;
    float start, end, startSum, endSum;

    grid1 = malloc(SIZE * sizeof (bool*));
    initializeGrid(grid1);

    createGlider(grid1);
    createRPentonimo(grid1);

    grid2 = malloc(SIZE * sizeof (bool*));
    initializeGrid(grid2);

    start = omp_get_wtime();
    executeGame(grid1, grid2);
    end = omp_get_wtime();

    startSum = omp_get_wtime();
    totalSum = aliveCounter(grid1);
    endSum = omp_get_wtime();

    float totalTime = end - start;
    float totalSumTime = endSum - startSum;

    printf("Celulas vivas: %d\n", totalSum);
    printf("Tempo de execucao das iteracoes: %.6fs\n", totalTime);
    printf("Tempo de execucao da soma: %.6fs\n", totalSumTime);
    printf("Tempo total: %.6fs\n", totalTime + totalSumTime);

    freeGridMemory(grid1);
    freeGridMemory(grid2);

    return 0;
}
