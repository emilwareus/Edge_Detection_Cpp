// Edge_detection.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "convolution.h"

using namespace cv;
using namespace std;

Mat src, src_gray;
Mat dst, detected_edges;


float PI = 3.14;

int** gaussianMask(int std) {	
	int** result = new int*[3];
	for (int i = -1; i < 2; i++) {
		result[i+1] = new int[3];
		for (int j = -1; j < 2; j++) {
			result[i+1][j+1] = int(exp(-(float(i*i) + float(j*j)) / (2.0f * float(std))));
		}
	}
	return result;
}


void print3x3Mask(float** mask){
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cout << mask[i][j] << "   ";
		}
		cout << endl;
	}
}
int main() {

	//Open Video stream
	VideoCapture stream1(1);   //0 is the id of video device.0 if you have only one camera.
	if (!stream1.isOpened()) { //check if video device has been initialisedc
		cout << "cannot open camera";
	}

	//Dimentions of filter mask
	const int mask_rows = 5;
	const int mask_cols = 5;
	int **gaussianFilter;
	gaussianFilter = new int *[mask_rows];

	//Values of filter 
	int temp[mask_rows][mask_cols] = {{ 2., 4., 5., 4., 2. },
	{ 4., 9., 12., 9., 4. },
	{ 5., 12., 15., 12., 5. },
	{ 4., 9., 12., 9., 4. },
	{ 2., 4., 5., 4., 2. } };

	int gaussian_normfactor = 0;
	//Transfer values and normalize
	for (int i = 0; i < mask_rows; i++) {
		gaussianFilter[i] = new int[mask_cols];
		for (int j = 0; j < mask_cols; j++) {
			gaussianFilter[i][j] = int(temp[i][j]);
			gaussian_normfactor = gaussian_normfactor + gaussianFilter[i][j];
		}
	}
	

	//Edge detection:
	//Dimentions of filter mask

	const int sobelmask_rows = 3;
	const int sobelmask_cols = 3;

	int **sobelMask_y;
	sobelMask_y = new int *[sobelmask_rows];


	int **sobelMask_x;
	sobelMask_x = new int *[sobelmask_rows];

	int tempx[sobelmask_rows][sobelmask_cols] = { { -1, -2, -1 },
	{ 0, 0, 0 },
	{ 1, 2, 1 } };

	//Transfer values and normalize

	for (int i = 0; i < sobelmask_rows; i++) {
		sobelMask_x[i] = new int[sobelmask_cols];
		for (int j = 0; j < sobelmask_cols; j++) {
			sobelMask_x[i][j] = tempx[i][j];
		}
	}

	int tempy[sobelmask_rows][sobelmask_cols] = { { -1, 0, 1 },
	{ -2, 0, 2 },
	{ -1, 0, 1 } };

	for (int i = 0; i < sobelmask_rows; i++) {
		sobelMask_y[i] = new int[sobelmask_cols];
		for (int j = 0; j < sobelmask_cols; j++) {
			sobelMask_y[i][j] = tempy[i][j];
		}
	}
	



	//unconditional loop
	while (true) {
		
		Mat gray, raw;

		//Grab image from camera
		stream1.read(raw);

		//Change to gray scale
		cvtColor(raw, gray, CV_RGB2GRAY);
		
		//Display image
		imshow("gray", gray);

		
		//2d-array of pixels
		int **image;
		image = new int *[gray.rows];
		
		//Fill with actual pixel-values for image
		for (int i = 0; i < gray.rows; ++i) {
			image[i] = new int[gray.cols];
			for (int j = 0; j < gray.cols; ++j) {
				image[i][j] =int(gray.at<uint8_t>(i,j));
				
			}
			
		}

		
	
		//Output 2d-array
		int **blur_image;

		//Perform GaussianBlur
		blur_image = convolution_norm(image, gray.rows, gray.cols, gaussianFilter, mask_rows, mask_cols, gaussian_normfactor);



		//Scale our output image
		int blur_rows = gray.rows - mask_rows + 1;
		int blur_cols = gray.cols - mask_cols + 1;

		
		int **xmask_image;
		int **ymask_image;

		//Perform Edge detection

		xmask_image = convolution(blur_image, blur_rows, blur_cols, sobelMask_x, sobelmask_rows, sobelmask_cols);
		ymask_image = convolution(blur_image, blur_rows, blur_cols, sobelMask_y, sobelmask_rows, sobelmask_cols);




		//Rescale image
		int out_rows = blur_rows - sobelmask_rows + 1;
		int out_cols = blur_cols - sobelmask_cols + 1;

		//Output 2d-array
		int **sobel_image;
		sobel_image = new int *[out_rows];

		int **edges_image;
		edges_image = new int *[out_rows];

		int max = 0;
		int min = 0;

		
		for (int i = 0; i<out_rows; i++) {
			sobel_image[i] = new int[out_cols];
			edges_image[i] = new int[out_cols];
			for (int j = 0; j<out_cols; j++) {
				sobel_image[i][j] = int(sqrt(xmask_image[i][j] * xmask_image[i][j] + ymask_image[i][j] * ymask_image[i][j]));
				
				
				if (sobel_image[i][j]>255) {
					sobel_image[i][j] = 255;
				}
				else if (sobel_image[i][j]<0) {
					sobel_image[i][j] = 0;
				}
		

				float temp_dir = atan2(ymask_image[i][j], xmask_image[i][j]) / PI * 180;
				if ((temp_dir>-22.5 && temp_dir <= 22.5) || (temp_dir>-157.5 && temp_dir <= 157.5)) {
					temp_dir = 0;
				}
				else if ((temp_dir>22.5 && temp_dir <= 67.5) || (temp_dir>112.5 && temp_dir <= -157.5)) {
					temp_dir = 45;
				}
				else if ((temp_dir>67.5 && temp_dir <= 112.5) || (temp_dir <= -67.5 && temp_dir >= -112.5)) {
					temp_dir = 90;
				}
				else if ((temp_dir>112.5 && temp_dir <= 157.5) || (temp_dir>-22.5 && temp_dir <= -67.5)) {
					temp_dir = 135;
				}
				edges_image[i][j] = temp_dir;
			}
		}

		//Non maximum suppression
		int ** thinned_edges_image;
		thinned_edges_image = new int *[out_rows];

		for (int i = 0; i<out_rows; i++) {
			thinned_edges_image[i] = new int[out_cols];
			for (int j = 0; j<out_cols; j++) {
				
				thinned_edges_image[i][j] = sobel_image[i][j];
				
			}
		}

		for (int i = 1; i<out_rows-1; i++) {
			for (int j = 1; j<out_cols-1; j++) {

				if (edges_image[i][j] == 0) {
					if ((sobel_image[i][j] < sobel_image[i][j + 1]) || (sobel_image[i][j] < sobel_image[i][j - 1])) {
						thinned_edges_image[i][j] = 0;
					}
				}
				else if (edges_image[i][j] == 90) {
					if ((sobel_image[i][j] < sobel_image[i + 1][j]) || (sobel_image[i][j] < sobel_image[i - 1][j])) {
						thinned_edges_image[i][j] = 0;
					}
				}
				else if (edges_image[i][j] == 135) {
					if ((sobel_image[i][j] < sobel_image[i - 1][j + 1]) || (sobel_image[i][j] < sobel_image[i + 1][j - 1])) {
						thinned_edges_image[i][j] = 0;
					}
				}
				else if (edges_image[i][j] == 45) {
					if ((sobel_image[i][j] < sobel_image[i + 1][j + 1]) || (sobel_image[i][j] < sobel_image[i - 1][j - 1])) {
						thinned_edges_image[i][j] = 0;
					}
				}


			}
		}



		
		//Thresholding
		int ** threshold_image;
		threshold_image = new int *[out_rows];

		for (int i = 0; i<out_rows; i++) {
			threshold_image[i] = new int[out_cols];
			for (int j = 0; j<out_cols; j++) {
				if (thinned_edges_image[i][j] < 70) {
					threshold_image[i][j] = 0;
				}
				else if (thinned_edges_image[i][j] > 150) {
					
					threshold_image[i][j] = 255;
				}
				else {
					threshold_image[i][j] = 127;
				}
			}
		}
		

	


		//output CV_matrix for display
		cv::Mat blur_mat(blur_rows, blur_cols, CV_8U);

		//Add pixels to matrix
		for (int i = 0; i < blur_rows; ++i) {
			for (int j = 0; j < blur_cols; ++j) {
				blur_mat.at<uint8_t>(i, j) = uint8_t(blur_image[i][j]);
				

			}
		}
		
		//Display matrix
		imshow("Blur_image", blur_mat);

		//___________________
	
		//output CV_matrix for display
		cv::Mat out_mat(out_rows, out_cols, CV_8UC1);

		//Add pixels to matrix
		for (int i = 0; i < out_rows; ++i) {
			for (int j = 0; j < out_cols; ++j) {
				out_mat.at<uint8_t>(i, j) = uint8_t(sobel_image[i][j]);
				
				
			}
		}


		//Display matrix
		imshow("Sobel_image", out_mat);

		//___________________

	

		//output CV_matrix for display
		cv::Mat thinned_edges_mat(out_rows, out_cols, CV_8UC1);

		//Add pixels to matrix
		for (int i = 0; i < out_rows; ++i) {
			for (int j = 0; j < out_cols; ++j) {
				thinned_edges_mat.at<uint8_t>(i, j) = uint8_t(thinned_edges_image[i][j]);
				
			}
		}

		cout << "max : " << max << endl;
		cout << "min : " << min << endl;
		//Display matrix
		imshow("Thinned_edges_image", thinned_edges_mat);


		//________________


		//output CV_matrix for display
		cv::Mat threshold_mat(out_rows, out_cols, CV_8UC1);

		//Add pixels to matrix
		for (int i = 0; i < out_rows; ++i) {
			for (int j = 0; j < out_cols; ++j) {
				threshold_mat.at<uint8_t>(i, j) = uint8_t(threshold_image[i][j]);

			}
		}
		//Display matrix
		imshow("Threshold_image", threshold_mat);


		//________________
		
		if (waitKey(30) >= 0)
			break;



		for (int i = 0; i < blur_rows; i++) {
			delete blur_image[i];
		}
		for (int i = 0; i < gray.rows; i++) {
			delete image[i];
		}
		
		for (int i = 0; i < out_rows; i++) {
			delete sobel_image[i];
			delete edges_image[i];
			delete threshold_image[i];
		}
		
		
		delete sobel_image;
		delete edges_image;
		delete threshold_image;
		delete blur_image;
		delete image;
		
	}


	delete gaussianFilter;
	delete sobelMask_x, sobelMask_y;

	return 0;

}

