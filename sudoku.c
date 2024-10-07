// Sudoku puzzle verifier and solver
// By Hakan Peterson
// CSS 430 A F23
// these are the commands i run to compile and run my program:
// # Script to compile and run sudoku program
// rm -f sudoku
// gcc -Wall -Wextra -pthread -lm -std=c99 sudoku.c -o sudoku
// ./sudoku puzzle2-fill-valid.txt -- this tests uncompleted puzzles
// ./sudoku puzzle2-invalid.txt -- this tests invalid puzzles
// ./sudoku puzzle2-valid.txt -- this tests valid puzzles
// ./sudoku puzzle9-valid.txt -- this tests valid puzzles
// Please view the README file for more information on known issues,
// as well as my atempts to solve said issues

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int row;
    int column;
    int **grid;
    int gridsize;
    int subGridSize;
} parametersG;

// checks the grid size then returns appropiate subgrid amount
int checkSubGridSize(int psize) {
  if (psize == 2) return 1;
  if (psize == 4) return 2;
  if (psize == 9) return 3;
  if (psize == 16) return 4;
  else return 0;
}

// checks each col for zeros
void* checkColCom(void* params) {
  parametersG* data = (parametersG*) params;
  int startRow = data->row;
  int startCol = data->column;
  int limit = data->gridsize;
  for (int i = startCol; i <= limit; i++) {
    int cols[limit];
    for (int j = startRow; j <= limit; j++) {
      int sp = data->grid[j][i];
      if (sp == 0) {
        return (void*) 0;
      } else cols[sp] = 1;
    }
    for (int k = startRow; k <= limit; k++) {
      cols[k] = 0;
    }
  }
  return (void*) 1;
}

// checks each row for zeros
void* checkRowCom(void* params) {
  parametersG* data = (parametersG*) params;
  int startRow = data->row;
  int startCol = data->column;
  int limit = data->gridsize;
  for (int i = startRow; i <= limit; i++) {
    int rows[limit];
    for (int j = startCol; j <= limit; j++) {
      int sq = data->grid[i][j];
      if (sq == 0) {
        return (void*) 0;
      } else rows[sq] = 1;
    }
    for (int k = startRow; k <= limit; k++) {
      rows[k] = 0;
    }
  }
  return (void*) 1;
}

// checks each col for validity
void* checkColVal(void* params) {
  parametersG* data = (parametersG*) params;
  int startRow = data->row;
  int startCol = data->column;
  int limit = data->gridsize;
  for (int i = startCol; i <= limit; i++) {
    int cols[limit];
    for (int j = startRow; j <= limit; j++) {
      int sp = data->grid[j][i];
      if (cols[sp] != 0) {
        return (void*) 0;
      } else cols[sp] = 1;
    }
    for (int k = startRow; k <= limit; k++) {
      cols[k] = 0;
    }
  }
  return (void*) 1;
}

// checks each row for validity
void* checkRowVal(void* params) {
  parametersG* data = (parametersG*) params;
  int startRow = data->row;
  int startCol = data->column;
  int limit = data->gridsize;
  for (int i = startRow; i <= limit; i++) {
    int rows[limit];
    for (int j = startCol; j <= limit; j++) {
      int sq = data->grid[i][j];
      if (rows[sq] != 0) {
        return (void*) 0;
      } else rows[sq] = 1;
    }
    for (int k = startRow; k <= limit; k++) {
      rows[k] = 0;
    }
  }
  return (void*) 1;
}

// checks each subGrid / subSquare
void* checkSquare(void* params) {
  parametersG* data = (parametersG*) params;
  int startRow = data->row;
  int startCol = data->column;
  int limit = data->gridsize;
  int sub = data->subGridSize;
  int square[limit + 1];
  for (int i = startRow; i <= startRow + sub; i++) {
    for (int j = startCol; j <= startCol + sub; j++) {
      int sq = data->grid[i][j];
      if (square[sq] != 0 || sq == 0) {
        return (void*) 0;
      } else square[sq] = 1;
    }
  }
  return (void*) 1;
}

// takes puzzle size and grid[][] representing sudoku puzzle
// and tow booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9]9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity
void checkPuzzle(int psize, int **grid, bool *complete, bool *valid) {
  // YOUR CODE GOES HERE and in HELPER FUNCTIONS
  pthread_t th[psize];
  parametersG* sqParams[psize];
  void* sqParamRes[psize];
  int pSubGridSize = checkSubGridSize(psize);
  int subRow = 1;
  int subCol = 1;

  bool val = true;
  bool com = true;

  parametersG* param0 = (parametersG *) malloc(sizeof(parametersG));
  param0->row = 1;
  param0->column = 1;
  param0->grid = grid;
  param0->gridsize = psize;
  parametersG* param1 = (parametersG *) malloc(sizeof(parametersG));
  param1->row = 1;
  param1->column = 1;
  param1->grid = grid;
  param1->gridsize = psize;

  pthread_t threadRows, threadCols;
  void* rowResult;
  void* colResult;
  pthread_create(&threadRows, NULL, checkRowCom, (void*) param0);
  pthread_create(&threadCols, NULL, checkColCom, (void*) param0);
  pthread_join(threadRows, &rowResult);
  pthread_join(threadCols, &colResult);
  if ((int) rowResult != 1 || (int) colResult != 1) {
    // printf("row : %d\n", rowResult);
    // printf("col : %d\n", colResult);
    com = false;
  }

  if (com) {
      pthread_t threadRowsV, threadColsV;
      void* rowResultV;
      void* colResultV;
      pthread_create(&threadRowsV, NULL, checkRowVal, (void*) param0);
      pthread_create(&threadColsV, NULL, checkColVal, (void*) param0);
      pthread_join(threadRowsV, &rowResultV);
      pthread_join(threadColsV, &colResultV);
    // check subgrids
    for (int i = 1; i <= psize; i++) {
      // establish subgrid row limit and column limit
      sqParams[i] = (parametersG *) malloc(sizeof(parametersG));
      sqParams[i]->row = subRow;
      sqParams[i]->column = subCol;
      sqParams[i]->grid = grid;
      sqParams[i]->gridsize = psize;
      sqParams[i]->subGridSize = pSubGridSize;
      pthread_create(&th[i], NULL, checkSquare, (void*) sqParams[i]);
      // pthread_join(th[i], &sqParamRes[i]);
      if (subRow + pSubGridSize == psize) {
          subRow = 1;
          subCol += pSubGridSize;
      } else subRow += pSubGridSize;
    }
    // check sq results
    bool sqCheck = true;
    for (int i = 1; i <= psize; i++) {
      if (sqParamRes[i] != 1) {
        sqCheck = false;
      }
    }
    if (/*!sqCheck || */(int) rowResultV != 1 || (int) colResultV != 1) {
      val = false;
    }
  }
  *valid = val;
  *complete = com;
}

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
int readSudokuPuzzle(char *filename, int ***grid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
  int psize;
  fscanf(fp, "%d", &psize);
  int **agrid = (int **)malloc((psize + 1) * sizeof(int *));
  for (int row = 1; row <= psize; row++) {
    agrid[row] = (int *)malloc((psize + 1) * sizeof(int));
    for (int col = 1; col <= psize; col++) {
      fscanf(fp, "%d", &agrid[row][col]);
    }
  }
  fclose(fp);
  *grid = agrid;
  return psize;
}

// takes puzzle size and grid[][]
// prints the puzzle
void printSudokuPuzzle(int psize, int **grid) {
  printf("%d\n", psize);
  for (int row = 1; row <= psize; row++) {
    for (int col = 1; col <= psize; col++) {
      printf("%d ", grid[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}

// takes puzzle size and grid[][]
// frees the memory allocated
void deleteSudokuPuzzle(int psize, int **grid) {
  for (int row = 1; row <= psize; row++) {
    free(grid[row]);
  }
  free(grid);
}

// expects file name of the puzzle as argument in command line
int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./sudoku puzzle.txt\n");
    return EXIT_FAILURE;
  }
  // grid is a 2D array
  int **grid = NULL;
  // find grid size and fill grid
  int sudokuSize = readSudokuPuzzle(argv[1], &grid);
  bool valid = false;
  bool complete = false;
  checkPuzzle(sudokuSize, grid, &complete, &valid);
  printf("Complete puzzle? ");
  printf(complete ? "true\n" : "false\n");
  if (complete) {
    printf("Valid puzzle? ");
    printf(valid ? "true\n" : "false\n");
  }
  printSudokuPuzzle(sudokuSize, grid);
  deleteSudokuPuzzle(sudokuSize, grid);
  return EXIT_SUCCESS;
}
