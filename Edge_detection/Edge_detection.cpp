// Edge_detection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/core/eigen.hpp>
#include "Eigen/Dense"

using namespace cv;
using namespace std;
//using Eigen::Matrix;

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

void CannyThreshold(int, void*)
{
	/// Reduce noise with a kernel 3x3
	blur(src_gray, detected_edges, Size(3, 3));

	/// Canny detector
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);

	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	imshow(window_name, dst);
}

template<typename _Tp, int _rows, int _cols, int _options, int _maxRows, int _maxCols> static inline
void cv2eigen(const Mat& src, Eigen::Matrix<_Tp, _rows, _cols, _options, _maxRows, _maxCols>& dst){
	cout << src.rows << _rows << endl;
	CV_DbgAssert(src.rows == _rows && src.cols == _cols);
	if (!(dst.Flags & Eigen::RowMajorBit))
	{
		const Mat _dst(src.cols, src.rows, traits::Type<_Tp>::value,
			dst.data(), (size_t)(dst.outerStride() * sizeof(_Tp)));
		if (src.type() == _dst.type())
			transpose(src, _dst);
		else if (src.cols == src.rows)
		{
			src.convertTo(_dst, _dst.type());
			transpose(_dst, _dst);
		}
		else
			Mat(src.t()).convertTo(_dst, _dst.type());
	}
	else
	{
		const Mat _dst(src.rows, src.cols, traits::Type<_Tp>::value,
			dst.data(), (size_t)(dst.outerStride() * sizeof(_Tp)));
		src.convertTo(_dst, _dst.type());
	}
}

int main(int argc, char** argv)
{
	VideoCapture stream1(1);   //0 is the id of video device.0 if you have only one camera.

	while(true){
		/// Load an image
		stream1.read(src);
		//Eigen::Matrix<float, 480, 640>  image;
		//cv2eigen(src, image);

		//cout << image.block(0, 0, 3, 3) << endl;
		//cout << image.size() << "   " << src.size[1] << endl;
		cout << " " << endl;
		

		
		if (!src.data)
		{
			return -1;
		}

		/// Create a matrix of the same type and size as src (for dst)
		dst.create(src.size(), src.type());

		/// Convert the image to grayscale
		cvtColor(src, src_gray, CV_BGR2GRAY);

		/// Create a window
		namedWindow(window_name, CV_WINDOW_AUTOSIZE);

		/// Create a Trackbar for user to enter threshold
		createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

		/// Show the image
		CannyThreshold(0, 0);

		/// Wait until user exit program by pressing a key
		if (waitKey(30) >= 0)
			break;

	}
	return 0;
}

/*
int main() {
	VideoCapture stream1(1);   //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened()) { //check if video device has been initialisedc
		cout << "cannot open camera";
	}

	//unconditional loop
	while (true) {
		Mat cameraFrame;
		stream1.read(cameraFrame);
		imshow("cam", cameraFrame);
		if (waitKey(30) >= 0)
			break;
	}
	return 0;
}*/
