// Edge_detection.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/core/eigen.hpp>
#include "Eigen/Dense"

#include "convolution.h"

using namespace cv;
using namespace std;

void printIntVec(std::vector<std::vector<uint8_t> > &vec) {
	std::vector< std::vector<uint8_t> >::const_iterator row; 
	std::vector<uint8_t>::const_iterator col; 
	for (row = vec.begin(); row != vec.end(); ++row)
	{ 
	 for (col = row->begin(); col != row->end(); ++col)
	 { 
			std::cout << *col; 
	 }
	}
}
void printFloatVec(std::vector<std::vector<float> > &vec) {
	std::vector< std::vector<float> >::const_iterator row; 
	std::vector<float>::const_iterator col; 

	for (row = vec.begin(); row != vec.end(); ++row)
	{ 
	 for (col = row->begin(); col != row->end(); ++col)
	 { 
			std::cout << *col << "   "; 
	 } 
	cout << endl;
	}
}
int main() {
	std::vector<uint8_t> a = {1, 1, 1, 1, 1};
	std::vector<uint8_t> b = {1, 2, 2, 2, 1};
	std::vector<uint8_t> c = {1, 2, 2, 2, 1};
	std::vector<uint8_t> d = {1, 2, 2, 2, 1};
	std::vector<uint8_t> e = {1, 1, 1, 1, 1};

	std::vector<std::vector<float> > result;
	std::vector<std::vector<uint8_t> > vecOfVecs;

	vecOfVecs.push_back(a);
	vecOfVecs.push_back(b);
	vecOfVecs.push_back(c);
	vecOfVecs.push_back(d);
	vecOfVecs.push_back(e);


	std::vector<uint8_t> m = {1, 1, 1};
	std::vector<uint8_t> n = {1, 1, 1};
	std::vector<uint8_t> o = {1, 1, 1};
	std::vector<std::vector<uint8_t> > mask;
	mask.push_back(m);
	mask.push_back(n);
	mask.push_back(o);
	
	printIntVec(mask);
	printIntVec(vecOfVecs);	
	result = convolution(vecOfVecs, mask); 
	cout << "CONVOLUTED" << endl;
	printFloatVec(result);
	return 0;
}
