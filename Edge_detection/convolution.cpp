#include <iostream>
#include "convolution.h"


uint8_t** convolution(uint8_t **image, int n_rows, int n_cols, float **mask, const int mask_rows, const int mask_cols) {


	
	//Output 2d-array
	uint8_t **result;

	//Size becomes smaller due to no padding
	result = new uint8_t *[n_rows - mask_rows + 1];


	for (int row = mask_rows / 2; row < n_rows - mask_rows / 2; row++) {

		//Temp array
		uint8_t *temp;
		temp = new uint8_t[n_cols - mask_cols + 1];
		for (int col = mask_cols / 2; col < n_cols - mask_cols / 2; col++) {
			
			//Mattrx multiplication 
			float sum = 0;
			for (int m_row = 0; m_row < mask_rows; m_row++) {
				for (int m_col = 0; m_col < mask_cols; m_col++) {
					sum += mask[m_row][m_col] * image[row - (mask_rows / 2) + m_row][col - (mask_cols / 2) + m_col];
				

				}
			}
			
			//No need to deviede by scale since filter is normalized (filter sum = 1.0)
			temp[col - mask_cols / 2] = sum;
		}

		result[row - mask_rows / 2] = temp;
		
	}
	
	return result;
}





//Does not work
uint8_t** non_maximum_supression(uint8_t **image, int n_rows, int n_cols) {
	float theta; // Calculate intensity gradient vector theta=atan2(Gy,Gx);
	double grad, phi, alpha, beta;

	//Output 2d-array
	double **gradient, **gradient_mag;
	//Size becomes smaller due to no padding
	gradient = new double *[n_rows - 2];
	gradient_mag = new double *[n_rows - 2];
	
	for (int row = 1; row < n_rows - 1 ; row++) {
		//Temp array
		double *temp;
		temp = new double[n_cols - 2];
		
		//Temp array
		double *temp_mag;
		temp_mag = new double[n_cols - 2];

		for (int col =1; col < n_cols - 1; col++) {
			phi = image[row][col];
			alpha = image[row-1][col] - phi;
			beta = image[row][col-1] - phi;
			temp[col-1] = atan2((beta), (alpha)) * 180 / 3.14;
			temp_mag[col-1] = sqrt((beta)*(beta ) + (alpha)*(alpha));

			std::cout << "Dir : " <<temp[col-1] << "  Mag : "<< temp_mag[col-1] << std::endl;
		}
		gradient[row - 1] = temp;
		gradient_mag[row - 1] = temp_mag;
	}

	//Non max Sup
	uint8_t  **non_max;
	//Size becomes smaller due to no padding
	non_max = new uint8_t  *[n_rows - 4];

	for (int j = 2; j < n_rows - 2; j++)
	{
		uint8_t  *temp_max;
		temp_max = new uint8_t[n_cols - 4];

		for (int i = 2; i < n_cols - 2; i++)
		{
			double theta = *gradient[i - 1, j - 1];
			temp_max[j - 2] = uint8_t(*gradient_mag[i - 1, j - 1]);

			// For horizontal edge
			if (((-22.5 < theta) && (theta <= 22.5)) || ((157.5 < theta) && (theta <= -157.5)))
			{
				if ((gradient_mag[i - 1, j - 1] < gradient_mag[i - 1, j]) || (gradient_mag[i - 1, j - 1] < gradient_mag[i - 1, j - 2]))
					temp_max[i - 2] = 0;
			}

			//For vertical edge
			else if (((-112.5 < theta) && (theta <= -67.5)) || ((67.5 < theta) && (theta <= 112.5)))
			{
				if ((gradient_mag[i - 1, j - 1] < gradient_mag[i, j - 1]) || (gradient_mag[i - 1, j - 1] < gradient_mag[i - 2, j - 1]))
					temp_max[i - 2] = 0;
			}

			// For 135 degree or -45 degree edge
			else if (((-67.5 < theta) && (theta <= -22.5)) || ((112.5 < theta) && (theta <= 157.5)))
			{
				if ((gradient_mag[i - 1, j - 1] < gradient_mag[i - 2, j]) || (gradient_mag[i - 1, j - 1] < gradient_mag[i, j - 2]))
					temp_max[i - 2] = 0;
			}

			// For 45 Degree Edge
			else if (((-157.5 < theta) && (theta <= -112.5)) || ((22.5 < theta) && (theta <= 67.5)))
			{
				if ((gradient_mag[i - 1, j - 1] < gradient_mag[i, j]) || (gradient_mag[i - 1, j - 1] < gradient_mag[i - 2, j - 2]))
					temp_max[i - 2] = 0;
			}

		}

		non_max[j - 2] = temp_max;
	}

	return non_max;

}



