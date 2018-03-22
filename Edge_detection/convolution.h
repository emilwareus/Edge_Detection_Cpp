#pragma once

int** convolution(int **image, int n_rows, int n_cols, int **mask, const int mask_rows, const int mask_cols);
int** convolution_norm(int **image, int n_rows, int n_cols, int **mask, const int mask_rows, const int mask_cols, int norm_factor);
