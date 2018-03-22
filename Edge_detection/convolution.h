#pragma once

uint8_t** convolution(uint8_t **image, int n_rows, int n_cols, float **mask, const int mask_rows, const int mask_cols);

uint8_t ** non_maximum_supression(uint8_t **image, int n_rows, int n_cols);