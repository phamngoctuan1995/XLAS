#ifndef DOAN_H
#define DOAN_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/ml/ml.hpp>
using namespace std;
using namespace cv;

#define PI 3.14159
#define DFT_MAX 2000
#define E	0.01

/* Ham lam tron bang toan tu Gaussian
- img: anh can lam tron
- dim: kich thuoc kernel
- sigma: do lech chuan
*/
Mat GaussianSmooth(const Mat& img, int dim, float sigma);

/* Ham lam tron bang toan tu Gaussian cua OpenCV
- img: anh can lam tron
*/
Mat GaussianSmooth_CV(const Mat& img, int dim, float sigma);

void LinhTinh(const Mat& img);

Mat IdealFilter(int hei, int wid, float D, bool isHigh = false);

Mat ButterworthFilter(int hei, int wid, float D, float n);

Mat GaussianFilter(int hei, int wid, float var, bool isHigh = false);

void Fourier_OpenCV(Mat src, Mat &des, bool isInv = false);
void FourierThuan(const Mat& img, Mat &fouR, Mat &fouI);
void FourierNguoc(const Mat &fouR, const Mat &fouI, Mat &resR, Mat &resI);
void FourierFilter(const Mat &fouR, const Mat &fouI, bool isHigh);

void ElementMultiply(const Mat &real, const Mat &imag, Mat &resR, Mat &resI, const Mat &h);
Mat PCA_RIP(Mat& img, int m, int type);
Mat PCA_RIP_FACE(Mat& img, int m, int type);
Mat PCA_CV(Mat& img, int m, int type);
Mat readImg(const string& link, int tsize);
void writeImg(const string& link, const Mat& img, int height, string a);

#endif