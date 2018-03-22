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

float** gaussianMask(int std) {	
	float** result = new float*[3];
	for (int i = -1; i < 2; i++) {
		result[i+1] = new float[3];
		for (int j = -1; j < 2; j++) {
			result[i+1][j+1] = exp(-(float(i*i) + float(j*j)) / (2.0f * float(std)));
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
	float **gaussianFilter;
	gaussianFilter = new float *[mask_rows];

	//Values of filter 
	float temp[mask_rows][mask_cols] = {{ 2., 4., 5., 4., 2. },
	{ 4., 9., 12., 9., 4. },
	{ 5., 12., 15., 12., 5. },
	{ 4., 9., 12., 9., 4. },
	{ 2., 4., 5., 4., 2. } };

	//Transfer values and normalize
	for (int i = 0; i < mask_rows; i++) {
		gaussianFilter[i] = new float[mask_cols];
		for (int j = 0; j < mask_cols; j++) {
			gaussianFilter[i][j] = temp[i][j]/159;
			
		}
	}


	//Edge detection:
	//Dimentions of filter mask
	const int edge_rows = 3;
	const int edge_cols = 3;
	float **edgeFilter;
	edgeFilter = new float *[edge_rows];

	//Values of filter 
	float edge_temp[edge_rows][edge_cols] = { { 2., -1., 2.},
	{ -1., -4., -1.},
	{ 2., -1., 2.} };

	//Transfer values and normalize
	for (int i = 0; i < edge_rows; i++) {
		edgeFilter[i] = new float[edge_cols];
		for (int j = 0; j < edge_cols; j++) {
			edgeFilter[i][j] = edge_temp[i][j] / 3;

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
		uint8_t **image;
		image = new uint8_t *[gray.rows];
		
		//Fill with actual pixel-values for image
		for (int i = 0; i < gray.rows; ++i) {
			image[i] = new uint8_t[gray.cols];
			for (int j = 0; j < gray.cols; ++j) {
				image[i][j] =gray.at<uint8_t>(i,j);
			}
			
		}

		
	
		//Outpit 2d-array
		uint8_t **blur_image;

		//Perform GaussianBlur
		blur_image = convolution(image, gray.rows, gray.cols, gaussianFilter, mask_rows, mask_cols);

		//Scale our output image
		int blur_rows = gray.rows - mask_rows + 1;
		int blur_cols = gray.cols - mask_cols + 1;

		//Outpit 2d-array
		uint8_t **out_image;

		//Perform Edge detection
		out_image = convolution(blur_image, blur_rows, blur_cols, edgeFilter, edge_rows, edge_cols);

		//Rescale image
		int out_rows = blur_rows - edge_rows + 1;
		int out_cols = blur_cols - edge_cols + 1;


		
		
		//output CV_matrix for display
		cv::Mat out_mat(out_rows , out_cols, CV_8UC1);

		//Add pixels to matrix
		for (int i = 0; i < out_rows; ++i) {
			for (int j = 0; j < out_cols; ++j) {
				out_mat.at<uint8_t>(i, j) = out_image[i][j];
				
			}
		}

		//Display matrix
		imshow("cam", out_mat);
		
		if (waitKey(30) >= 0)
			break;

		for (int i = 0; i < blur_rows; i++) {
			delete blur_image[i];
		}
		for (int i = 0; i < out_rows; i++) {
			delete out_image[i];
		}
		for (int i = 0; i < gray.rows; i++) {
			delete image[i];
		}
		delete blur_image;
		delete out_image;
		delete image;
		
	}

	delete gaussianFilter;
	delete edgeFilter;
}

