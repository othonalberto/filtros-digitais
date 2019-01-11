#ifndef biblioteca_cpp
#define biblioteca_cpp

#ifdef __APPLE__
    #include <opencv2/core/core.hpp>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <opencv2/highgui/highgui.hpp>    
#else
    #include <opencv2/core.hpp> 
#endif

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <tgmath.h> 

#include "biblioteca.hpp"

using namespace std;
using namespace cv;

unsigned int getNBHD(Mat image, int x, int y, int n) {
    int c,
        r,
        ac = 0; 

    for (r = y-n; r <= y+n; r++) {
        for (c = x-n; c <= x+n; c++) {
            ac += (unsigned int)image.at<uchar>(Point(c,r));
        }
    }

    return ac;
}

unsigned int getMedian(Mat image, int x, int y, int n) {
    int c,
        r;

    vector<unsigned int> values;

    for (r = y-n; r <= y+n; r++) {
        for (c = x-n; c <= x+n; c++) {
            values.push_back(
                (unsigned)image.at<uchar>(Point(c,r))
                );
        }
    }

    sort(values.begin(), values.end());

    return values[values.size() / 2];

}

unsigned getGaussianPixel(Mat image, int x, int y) {
    int c,
        r,
        n = 5 / 2,
        y1,
        x1,
        v2;

    double v1,
           sum = 0;

    int kernel[5][5] = {{1,  4,  7,  4, 1},
                        {4, 16, 26, 16, 4},
                        {7, 26, 41, 26, 7},
                        {4, 16, 26, 16, 4},
                        {1,  4,  7,  4, 1}};

    y1 = 0;
    
    for (r = y-n; r <= y+n; r++) {
        x1 = 0;
        for (c = x-n; c <= x+n; c++) {
            v1 = (unsigned int)image.at<uchar>(Point(c,r));
            v2 = kernel[x1][y1];
            sum += v1*v2;
            x1++;
        }

        y1++;
    }

    return sum;
}

void getFreqs(Mat image, double freq[]) {
    int x,
        y;


    for (y = 0; y < image.rows; y++) {
        for (x = 0; x < image.cols; x++) {
            freq[(unsigned int)image.at<uchar>(Point(x,y))]++;
        }
    }
}

void getProb(Mat image, double freq[], double prob[]) {
    int x;

    for (x = 0; x < 256; x++) 
        prob[x] = freq[x] / (image.rows * image.cols);
}

void getCdf(Mat image, double prob[]) {
    int x;

    for (x = 1; x < 256; x++) {
        prob[x] = prob[x] + prob[x-1];
    }
    
}

void getMinMax(Mat image, unsigned int minMax[]) {
    unsigned int min = 0,
                 max = 0,
                 value;

    int x,
        y;

    for (y = 0; y < image.rows; y++) {
        for (x = 0; x < image.cols; x++) {
            value = image.at<uchar>(Point(x,y));

            if (value < min) min = value;

            if (value > max) max = value;
        }
    }

    minMax[0] = min;
    minMax[1] = max;
}

Mat openImage(string path, bool modo) {

    Mat image;
    // 0 = grey
    // 1 = color
    image = imread(path, modo);
    
    if (image.data) return image;
    else cout << "Nao foi possível abrir a imagem";

    exit(0);

}

Mat meanFilter(Mat image, int k_size) {
    int x,
        y,
        h,
        w;
        
    unsigned int value;

    Mat processada,
        result;

    processada = copyBorder(image, k_size/2);
    
    image.copyTo(result);

    h = processada.rows;
    w = processada.cols;

    for (y = k_size/2; y < h; y++) {
        for (x = k_size/2; x < w; x++) {
            value = getNBHD(processada, x, y, k_size/2);
            processada.at<uchar>(Point(x,y)) = value / (k_size * k_size);
        }
    }

    for (y = 0; y < image.rows; y++) {
        for (x = 0; x < image.cols; x++) {
            result.at<uchar>(Point(x,y)) = processada.at<uchar>(Point(x+ (k_size/2), y + (k_size/2)));
        }
    }
    
    return result;

}

Mat copyBorder(Mat image, int n) {

    int w = image.cols,
        h = image.rows,
        x,
        y;

    Mat saida = Mat::zeros(h + 2*n, w+2*n, CV_16S);

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            saida.at<uchar>(Point(x+n,y+n)) = image.at<uchar>(Point(x,y));
        }
    }

    // copia linha cima
    for (y = 0; y < n; y++) {
        for (x = 0; x < w; x++) {
            saida.at<uchar>(Point(x,y)) = image.at<uchar>(Point(x,y));
        }
    }

    // copia coluna esquerda
    for (y = 0; y < h; y++) {
        for (x = 0; x < n; x++) {
            saida.at<uchar>(Point(x,y)) = image.at<uchar>(Point(x,y));
        }
    }

    // copia linha abaixo
    for (y = h-1; y < h; y++) {
        for (x = 0; x < w; x++) {
            saida.at<uchar>(Point(x,y)) = image.at<uchar>(Point(x,y));
        }
    }    

    // copia coluna direita
    for (y = w-1; y < w; y++) {
        for (x = 0; x < h; x++) {
            saida.at<uchar>(Point(x,y)) = image.at<uchar>(Point(x,y));
        }
    }

    return saida;
}

Mat medianFilter(Mat image, int k_size) {
    int x,
        y,
        h,
        w;

    unsigned int value;

    Mat processada,
        result;

    processada = copyBorder(image, k_size/2);

    image.copyTo(result);

    h = processada.rows;
    w = processada.cols;

    for (y = k_size/2; y < h; y++) {
        for (x = k_size/2; x < w; x++) {
            value = getMedian(processada, x, y, k_size/2);
            processada.at<uchar>(Point(x,y)) = value;
        }
    }


    for (y = 0; y < result.rows; y++) {
        for (x = 0; x < result.cols; x++) {
            result.at<uchar>(Point(x,y)) = processada.at<uchar>(Point(x+k_size/2, y+k_size/2));
        }
    }

    return result;

}

Mat gaussianFilter(Mat image) {

    int x,
        y;

    Mat result,
        processada = copyBorder(image, 5/2);

    int kernel[5][5] = {{1,  4,  7,  4, 1},
                        {4, 16, 26, 16, 4},
                        {7, 26, 41, 26, 7},
                        {4, 16, 26, 16, 4},
                        {1,  4,  7,  4, 1}};

    image.copyTo(result);

    for (y = 5/2; y < processada.cols; y++) {
        for (x = 5/2; x < processada.rows; x++) {
            processada.at<uchar>(Point(x,y)) = getGaussianPixel(processada, x, y) / 273;
        }
    }

    for (y = 0; y < result.rows; y++) {
        for (x = 0; x < result.cols; x++) {
            result.at<uchar>(Point(x,y)) = processada.at<uchar>(Point(x+2,y+2));
        }
    }

    return result;
}

Mat histoEq(Mat image) {
    int x,
        y,
        k2 = 255,
        k1 = 0,
        l1,
        l2;

    Mat result;
    image.copyTo(result);

    double freq[256] = {0};
    double prob[256] = {0};
    double p;
    unsigned int minMax[2] = {0};

    //cout << "Get Freqs vai começar" << endl;
    getFreqs(result, freq);
    //cout << "Get Freqs ok " << endl;
    getProb(result, freq, prob);
    //cout << "Get Probs" << endl;
    getCdf(result, prob);
    //cout << "Get CDF" << endl;
    getMinMax(result, minMax);
    //cout << "Get MinMax" << endl;
    l1 = minMax[0];
    l2 = minMax[1];

    
    for (x = 0; x < 256; x++) {
        prob[x] = floor(prob[x] * 255);
    }

    for (y = 0; y < result.rows; y++) {
        for (x = 0; x < result.cols; x++) {

            result.at<uchar>(Point(x,y)) = prob[result.at<uchar>(Point(x,y))];
        }
    }

    cout << "OPA" << endl;;

    return result;   
}

Mat sobel(Mat image) {
    int kernelX[3][3] = {{-1, 0, 1},
                         {-2, 0, 2},
                         {-1, 0, 1}};

    int kernelY[3][3] = {{1, 2, 1},
                         {0,  0,  0},
                         {-1,  -2,  -1}};

    Mat result = conv(image, kernelX, kernelY, 2);

    return result;

    
}

Mat prewitt(Mat image) {
    int kx[3][3] = {{-1, 0, 1},
                    {-1, 0, 1},
                    {-1, 0, 1}};

    int ky[3][3] = {{1, 1, 1},
                    {0, 0, 0},
                    {-1, -1, -1}};

    Mat result = conv(image, kx, ky, 1);

    return result;
}

Mat laplaciano(Mat image) {
     Mat result,
        buff,
        processada;

    int kernel[3][3] = {{0, -1, 0},
                        {-1, 4, -1},
                        {0, -1, 0}};

    processada = copyBorder(image, 3/2);

    image.copyTo(result);

    int y,
        x;
    

    int value = 0,
        gx = 0,
        gy = 0;

    for (y = 1; y < processada.cols; y++) {
        for (x = 1; x < processada.rows; x++) {
            
            for (int k = y - 1; k <= y+1; k++) {
                for(int l = x - 1; l <= x+1; l++) {
					value += ((int)image.at<uchar>(l,k) * kernel[gx][gy]);
					gx++;
				}
                
                gx = 0;
                
                gy++;
            }

            gy = 0;
            
            processada.at<uchar>(x,y) = abs(value);

            value = 0;
        }
    }

    for (y = 0; y < image.rows; y++) {
        for (x = 0; x < image.cols; x++) {
            result.at<uchar>(Point(x,y)) = processada.at<uchar>(Point(x+1, y+1));
        }
    }

    return result;

    
}

Mat laplacianGauss(Mat image) {
    Mat result,
        buff,
        processada;

    int kernel[5][5] = {{0, 0,   1, 0, 0},
                        {0, 1,   2, 1, 0},
                        {1, 2, -16, 2, 1},
                        {0, 1,   2, 1, 0},
                        {0, 0,   1, 0, 0}};

    processada = copyBorder(image, 5/2);

    image.copyTo(result);

    int y,
        x;
    

    int value = 0,
        gx = 0,
        gy = 0;

    for (y = 5/2; y < processada.cols; y++) {
        for (x = 5/2; x < processada.rows; x++) {
            
            for (int k = y - 5/2; k <= y+5/2; k++) {
                for(int l = x - 5/2; l <= x+5/2; l++) {
					value += ((int)image.at<uchar>(l,k) * kernel[gx][gy]);
					gx++;
				}
                
                gx = 0;
                
                gy++;
            }

            gy = 0;
            
            processada.at<uchar>(x,y) = abs(value);

            value = 0;
        }
    }

    for (y = 0; y < image.rows; y++) {
        for (x = 0; x < image.cols; x++) {
            result.at<uchar>(Point(x,y)) = processada.at<uchar>(Point(x+5/2, y+5/2));
        }
    }

    return result;

}

Mat roberts(Mat image) {
    int kx[2][2] = {{1, 0},
                    {0, -1}};

    int ky[2][2] = {{0, 1},
                    {-1, 0}};
    Mat result,
        buff,
        processada;

    processada = copyBorder(image, 2/2);
    buff = copyBorder(image, 2/2);

    image.copyTo(result);

    int y,
        x;
    
    int px = 0,
        py = 0,
        value = 0,
        gx = 0,
        gy = 0;

    for (y = 1; y < processada.cols; y++) {
        for (x = 1; x < processada.rows; x++) {
            
            for (int k = y - 1; k <= y; k++) {
                for(int l = x - 1; l <= x; l++) {
					px += ((int)image.at<uchar>(l,k) * kx[gx][gy]);
					py += ((int)image.at<uchar>(l,k) * ky[gx][gy]);
					gx++;
				}
                
                gx = 0;
                
                gy++;
            }

            gy = 0;
            
            processada.at<uchar>(x,y) = sqrt(px*px + py*py);

            px = py = 0;
        }
    }

    for (y = 0; y < image.rows; y++) {
        for (x = 0; x < image.cols; x++) {
            result.at<uchar>(Point(x,y)) = processada.at<uchar>(Point(x+1, y+1));
        }
    }

    return result;
}

Mat conv(Mat image, int kx[3][3], int ky[3][3], int kvalue) {
    Mat result,
        buff,
        processada;

    processada = copyBorder(image, 3/2);
    buff = copyBorder(image, 3/2);

    image.copyTo(result);

    int y,
        x;
    
    int px = 0,
        py = 0,
        value = 0,
        gx = 0,
        gy = 0;

    for (y = 1; y < processada.cols; y++) {
        for (x = 1; x < processada.rows; x++) {
            
            for (int k = y - 1; k <= y+1; k++) {
                for(int l = x - 1; l <= x+1; l++) {
					px += ((unsigned int)buff.at<uchar>(l,k) * kx[gx][gy]) / (2+kvalue);
					py += ((unsigned int)buff.at<uchar>(l,k) * ky[gx][gy]) / (2+kvalue);
					gx++;
				}
                
                gx = 0;
                
                gy++;
            }

            gy = 0;
            
            processada.at<uchar>(x,y) = sqrt(px*px + py*py);

            px = py = 0;
        }
    }

    for (y = 0; y < result.rows; y++) {
        for (x = 0; x < result.cols; x++) {
            result.at<uchar>(Point(x,y)) = processada.at<uchar>(Point(x+1, y+1));
        }
    }

    cout << "X: " << x << "Y: " << y << endl;

    //imshow("teste", result);
    //waitKey(0);
    return result;
}

#endif