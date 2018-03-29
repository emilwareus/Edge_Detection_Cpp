// Edge_detection.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <iostream>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include "convolution.h"
#include <stdio.h>
#include <fstream>
#include <bitset>

//using namespace cv;
using namespace std;

//Mat src, src_gray;
//Mat dst, detected_edges;
int cols;
int rows;
unsigned int bitNum;



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

void save_image(string outputFileName_str, int **image, int n_rows, int n_cols) {
	const char* outputFileName = outputFileName_str.c_str();

	//Output image in a new file
	int size = n_rows * n_cols;
	char* buffer = new char[size -1];
	for (int i = 0; i<n_rows; i++) {
		for (int j = 0; j<n_cols; j++) {
			buffer[i*n_cols + j] = (int)image[i][j];
		}
	}
	ofstream myFile(outputFileName, ios::out | ios::binary);
	myFile.write(buffer, size);
	delete buffer;
}


int ** read_image(const char* image_name) {
	char buff[5];
	FILE * imageFile;
	imageFile = fopen(image_name, "rb");

	/*get Header  -> rowNum, colNum and BitperPixel*/
	fscanf(imageFile, "%c", &buff[0]);
	fscanf(imageFile, "%c", &buff[1]);
	cols = ((unsigned int)buff[1] << 8) + (unsigned int)(buff[0] & 0xff);

	fscanf(imageFile, "%c", &buff[2]);
	fscanf(imageFile, "%c", &buff[3]);
	rows = ((unsigned int)buff[3] << 8) + (unsigned int)(buff[2] & 0xff);
	fscanf(imageFile, "%c", &buff[4]);
	bitNum = (unsigned int)(buff[4] & 0x08);
	cout << "Header: -> rows: " << rows << " cols: " << cols << " bitNum: " << bitNum << endl;

	int **image_temp;
	image_temp = new int *[rows];
	
	for (int i = 0; i<rows; i++) {
		image_temp[i] = new int[cols];
		for (int j = 0; j<cols; j++) {
			char pixbuff;
			fscanf(imageFile, "%c", &pixbuff);
			int val_temp = (int)pixbuff;
			image_temp[i][j] = val_temp;
		}
	}

	int **image;
	image = new int *[rows];

	//Fill with actual pixel-values for image
	for (int i = 0; i < rows; ++i) {
		image[i] = new int[cols];
		for (int j = 0; j < cols; ++j) {
			image[i][j] = image_temp[i][j];
		}
	}
	return image;
}

void print3x3Mask(float** mask){
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cout << mask[i][j] << "   ";
		}
		cout << endl;
	}
}



void edge_detection(int **image, int image_rows, int image_cols, int **gaussianFilter, const int mask_rows, const int mask_cols, int **sobelMask_x, int **sobelMask_y, int sobelmask_rows, int sobelmask_cols, int gaussian_normfactor, bool save) {
	



	//Output 2d-array
	int **blur_image;

	//Perform GaussianBlur
	blur_image = convolution_norm(image, image_rows, image_cols, gaussianFilter, mask_rows, mask_cols, gaussian_normfactor);



	//Scale our output image
	int blur_rows = image_rows - mask_rows + 1;
	int blur_cols = image_cols - mask_cols + 1;


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

	//edge magnitude and direction
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

	for (int i = 1; i<out_rows - 1; i++) {
		for (int j = 1; j<out_cols - 1; j++) {
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
			if (thinned_edges_image[i][j] < 60) {
				threshold_image[i][j] = 0;
			}
			else if (thinned_edges_image[i][j] > 100) {

				threshold_image[i][j] = 255;
			}
			else {
				threshold_image[i][j] = 127;
			}
		}
	}


	//Hysteresis
	int ** hysteresis_image;
	hysteresis_image = new int *[out_rows];


	for (int i = 1; i<out_rows; i++) {
		hysteresis_image[i] = new int[out_cols];
		for (int j = 1; j<out_cols; j++) {
			hysteresis_image[i][j] = threshold_image[i][j];
		}
	}
	int image_has_changed = 1;
	while (image_has_changed == 1) {
		image_has_changed = 0;
		for (int i = 1; i<out_rows - 1; i++) {
			for (int j = 1; j<out_cols - 1; j++) {
				if (hysteresis_image[i][j] == 127) {
					if (hysteresis_image[i - 1][j - 1] == 255 || hysteresis_image[i - 1][j] == 255 || hysteresis_image[i - 1][j + 1] == 255 || hysteresis_image[i][j - 1] == 255 || hysteresis_image[i][j + 1] == 255 || hysteresis_image[i + 1][j - 1] == 255 || hysteresis_image[i + 1][j] == 255 || hysteresis_image[i + 1][j + 1] == 255) {
						hysteresis_image[i][j] = 255;
						image_has_changed = 1;
					}
				}
			}
		}
	}
	for (int i = 1; i<out_rows; i++) {
		for (int j = 1; j<out_cols; j++) {
			if (hysteresis_image[i][j] == 127) {
				hysteresis_image[i][j] = 0;
			}
		}
	}



	/*
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

	//output CV_matrix for display
	cv::Mat hysteresis_mat(out_rows, out_cols, CV_8UC1);

	//Add pixels to matrix
	for (int i = 1; i < out_rows - 1; ++i) {
		for (int j = 1; j < out_cols - 1; ++j) {
			hysteresis_mat.at<uint8_t>(i, j) = uint8_t(hysteresis_image[i][j]);

		}
	}
	//Display matrix
	imshow("Hysteresis_image", hysteresis_mat);


	//________________

	*/
	
	//Saves the images
	if(save == true){

		save_image("modified_images/original_image.raw", image, image_rows, image_cols);
		save_image("modified_images/sobel_image.raw", sobel_image, out_rows, out_cols);
		save_image("modified_images/thinned_edges_image.raw", thinned_edges_image, out_rows, out_cols);
		save_image("modified_images/threshold_image.raw", threshold_image, out_rows, out_cols);
		save_image("modified_images/hysteresis_image.raw", hysteresis_image, out_rows, out_cols);

	}


	for (int i = 0; i < blur_rows; i++) {
		delete blur_image[i];
	}
	for (int i = 0; i < image_rows; i++) {
		delete image[i];
	}

	for (int i = 0; i < out_rows; i++) {
		delete sobel_image[i];
		delete edges_image[i];
		delete threshold_image[i];
		delete thinned_edges_image[i];
	}

	for (int i = 1; i < out_rows - 1; i++) {
		delete hysteresis_image[i];
	}

	delete thinned_edges_image;
	delete hysteresis_image;
	delete sobel_image;
	delete edges_image;
	delete threshold_image;
	delete blur_image;
	delete image;





}


int main() {

	//Open Video stream
	/*
	VideoCapture stream1(1);   //0 is the id of video device.0 if you have only one camera.
	if (!stream1.isOpened()) { //check if video device has been initialisedc
		cout << "cannot open camera";
	}
	*/

	/* To open .raw images
	int **image;
	image = read_image("images/leaf.raw");

	save_image("modified_images/original_image.raw", image, rows, cols);

	*/

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
	

	//Edgedetection 





	//unconditional loop
	/*
	while (true) {
		
		Mat raw;

		//Grab image from camera
		stream1.read(raw);

		Mat gray;
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
				image[i][j] = int(gray.at<uint8_t>(i, j));

			}

		}

		edge_detection(image, gray.rows, gray.cols, gaussianFilter, mask_rows, mask_cols, sobelMask_x, sobelMask_y, sobelmask_rows, sobelmask_cols, gaussian_normfactor, false);


		if (waitKey(30) >= 0)
			break;


	}
	*/

	delete gaussianFilter;
	delete sobelMask_x, sobelMask_y;

	return 0;

}

