#include <stdio.h>
#include <stdlib.h>
#include "sudokuSolver/Solver.h"

/*
*	Function : find_empty_cell
*	-------------------------
*	Find the next empty cell in the grid
*/
int find_empty_cell(int grid[9][9], int* call_row, int* call_col)
{
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			if (!grid[row][col])
			{
				*call_row = row;
				*call_col = col;
				return 1;
			}
		}
	}
	return 0;
}

/*
*	Function : is_valid_row
*	-----------------------
*	Check if the digit is already present in the row
*/
int is_valid_row(int grid[9][9], int row, int digit)
{
	for (int i = 0; i < 9; i++)
	{
		if (grid[row][i] == digit)
		{
			return 0;
		}
	}

	return 1;
}

/*
*	Function : is_valid_column
*	--------------------------
*	Check if the digit is already present in the column
*/
int is_valid_column(int grid[9][9], int col, int digit)
{
    for (int i = 0; i < 9; i++)
    {
        if (grid[i][col] == digit)
        {
            return 0;
        }
    }
    return 1;
}

/*
*	Function : is_valid_subgrid
*	---------------------------
*	Check if the digit is already present in the subgrid
*/
int is_valid_subgrid(int grid[9][9], int start_row, int start_col, int digit)
{
    for (int row = start_row; row < start_row + 3; row++)
    {
        for (int col = start_col; col < start_col + 3; col++)
        {
            if (grid[row][col] == digit)
            {
                return 0;
            }
        }
    }
    return 1;
}

/*
*	Function : is_valid
*	-------------------
*	Check if the digit is valid in the given position
*/
int is_valid(int grid[9][9], int row, int col, int digit)
{
	int val_row = is_valid_row(grid, row, digit);
	int val_col = is_valid_column(grid, col, digit);
	int val_sub = is_valid_subgrid(grid, row - row % 3, col - col % 3, digit);
	return (val_row && val_col && val_sub);
}

/*
*	Function : solver
*	-----------------
*	Solve the sudoku using backtracking
*/
int solver(int grid[9][9])
{
	int row = 0;
	int col = 0;

	if (!find_empty_cell(grid, &row, &col))
	{
		return 1;
	}

	for (int digit = 1; digit <= 9; digit++)
	{
		if (is_valid(grid, row, col, digit))
		{
			grid[row][col] = digit;
			
			if (solver(grid))
			{
				return 1;
			}

			grid[row][col] = 0;
		}
	}

	return 0;
}


//Return in missing digits the digits that were not in the original grid
int solverMissingDigits(int grid[9][9], int** missingDigits)
{
	int row = 0;
	int col = 0;
	
	if (!find_empty_cell(grid, &row, &col))
	{
		return 1;
	}
	
	for (int digit = 1; digit <= 9; digit++)
	{
		if (is_valid(grid, row, col, digit))
		{
			grid[row][col] = digit;
			missingDigits[row][col] = digit;
			
			if (solverMissingDigits(grid, missingDigits))
			{
				return 1;
			}
			
			grid[row][col] = 0;
		}
	}
	
	return 0;
}

/*
*	Function : load_grid
*	--------------------
*	Load the grid from the file
*/
void load_grid(const char *filename, int grid[9][9])
{
	FILE *file = fopen(filename, "r");
	if (!file)
	{
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			char ch;
			fscanf(file, " %c", &ch);
			grid[row][col] = (ch == '.') ? 0 : ch - '0';
		}
	}

	fclose(file);
}

/*
*	Function : print_grid
*	---------------------
*	Print the grid
*/
void print_grid(int grid[9][9])
{
	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			if (grid[row][col] == 0)
				printf(". ");
			else
				printf("%d ", grid[row][col]);
		}
		printf("\n");
	}
}

/*
*	Function : save_grid
*	--------------------
*	Save the grid to the file
*/
void save_grid(const char *filename, int grid[9][9])
{
	FILE *file = fopen(filename, "w");
	if (!file)
	{
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	for (int row = 0; row < 9; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			if (grid[row][col] == 0)
			{
				fprintf(file, ". ");
			}
			else if (col == 2 || col == 5)
			{
				fprintf(file, "%d ", grid[row][col]);
			}
			else
			{
				fprintf(file, "%d", grid[row][col]);
			}
		}
		if (row == 2 || row == 5)
		{
			fprintf(file, "\n\n");
		}
		else
		{
			fprintf(file, "\n");
		}
	}

	fclose(file);
}

/*
*	Function : solver_main
*	----------------------
*	Main function
*/
int solver_main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int grid[9][9];
	load_grid(argv[2], grid);

	if (solver(grid))
	{
		char output_filename[100];
		snprintf(output_filename, sizeof(output_filename), "%s.result", argv[2]);
		save_grid(output_filename, grid);
	}
	else
	{
		printf("No solution exists.\n");
	}

	return EXIT_SUCCESS;
}