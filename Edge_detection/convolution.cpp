
#include <iostream>
#include <vector>


std::vector<std::vector<float>> convolution(std::vector<std::vector<uint8_t>>* image, std::vector<std::vector<uint8_t>>* mask) {

	int n_rows = image->size(); /* rows */
	int n_cols = image[0].size(); /* Collumns*/

	int mask_rows = mask->size(); /* Mask Rows */
	int mask_cols = mask[0].size(); /* Mask Collumns*/
	
	std::vector<std::vector<float>> result;

	for (int row = 1; row < n_rows - mask_rows / 2; row++) {
		std::vector<float> temp;
		for (int col = 1; col < n_cols - mask_cols / 2; col++) {
			float sum = 0;
			
			for (int m_row = 0; m_row < mask_rows; m_row++) {
				for (int m_col = 0; m_col < mask_cols; m_col++) {
					sum += ((*mask)[m_row][m_col])*((*image)[row][col]);

				}

			}
			temp.push_back(sum / (mask_cols*mask_cols));
		}
		result.push_back(temp);
	}

	return result;
}