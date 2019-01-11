#ifndef biblioteca_hpp
#define biblioteca_hpp

#include <iostream>

using namespace std;
using namespace cv;

unsigned int getNBHD(Mat image, int x, int y, int n);

unsigned int getMedian(Mat image, int x, int y, int n);

unsigned getGaussianPixel(Mat image, int x, int y);

void getFreqs(Mat image, double freq[]);

void getProb(Mat image, double freq[], double prob[]);

void getCdf(Mat image, double prob[]);

void getMinMax(Mat image, unsigned int minMax[]);

Mat openImage(string path, bool modo);

Mat meanFilter(Mat image, int k_size);

Mat copyBorder(Mat image, int n);

Mat medianFilter(Mat image, int k_size);

Mat gaussianFilter(Mat image);

Mat histoEq(Mat image);

Mat sobel(Mat image);

Mat prewitt(Mat image);

Mat laplaciano(Mat image);

Mat laplacianGauss(Mat image);

Mat roberts(Mat image);

Mat conv(Mat image, int kx[3][3], int ky[3][3], int kvalue);

#endif
