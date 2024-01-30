#pragma once

int solver_main(int argc, char *argv[]);
int solver(int grid[9][9]);
void load_grid(const char *filename, int grid[9][9]);
void print_grid(int grid[9][9]);
int solverMissingDigits(int grid[9][9], int** missingDigits);