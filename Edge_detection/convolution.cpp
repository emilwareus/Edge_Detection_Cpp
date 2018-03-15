#include <iostream>
#include <vector>


std::vector<std::vector<float> > convolution(std::vector<std::vector<uint8_t> > &image, std::vector<std::vector<uint8_t> > &mask) {
	int n_rows = image.size(); /* rows */
	int n_cols = image[0].size(); /* Collumns*/

	int mask_rows = mask.size(); /* Mask Rows */
	int mask_cols = mask[0].size(); /* Mask Collumns*/
	int half_mask_rows = mask_rows / 2;	
	int half_mask_cols = mask_cols / 2;	
		
	std::vector<std::vector<float> > result;
	for (int row = 1; row < n_rows - mask_rows / 2; row++) {
		std::vector<float> temp;
		for (int col = 1; col < n_cols - mask_cols / 2; col++) {
			float sum = 0;
			for (int m_row = -half_mask_rows; m_row <= half_mask_rows; m_row++) {
				for (int m_col = -half_mask_cols; m_col <= half_mask_cols ; m_col++) {
					sum += mask[half_mask_rows + m_row][half_mask_cols + m_col]*image[row+ m_row][col + m_col];
				}
			}
			temp.push_back(sum / (mask_cols*mask_cols));
		}
		result.push_back(temp);
	}
	return result;
}

