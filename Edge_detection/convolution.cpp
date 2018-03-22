#include <iostream>
#include "convolution.h"
#include <cmath> 

int** convolution(int **image, int n_rows, int n_cols, float **mask, const int mask_rows, const int mask_cols) {


	
	//Output 2d-array
	int **result;

	//Size becomes smaller due to no padding
	result = new int *[n_rows - mask_rows + 1];


	

	for (int row = mask_rows / 2; row < n_rows - mask_rows / 2; row++) {

		//Temp array
		int *temp;
		temp = new int[n_cols - mask_cols + 1];
		for (int col = mask_cols / 2; col < n_cols - mask_cols / 2; col++) {
			
			//Mattrx multiplication 
			float sum = 0;
			for (int m_row = 0; m_row < mask_rows; m_row++) {
				for (int m_col = 0; m_col < mask_cols; m_col++) {
					sum += mask[m_row][m_col] * image[row - (mask_rows / 2) + m_row][col - (mask_cols / 2) + m_col];
					
				}
			}
			
			//No need to deviede by scale since filter is normalized (filter sum = 1.0)
			
			temp[col - mask_cols / 2] = int(sum);
		}

		result[row - mask_rows / 2] = temp;
		
	}

	return result;
}




