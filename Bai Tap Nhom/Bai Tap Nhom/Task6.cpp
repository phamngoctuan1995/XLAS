#include "Header.h"
#include <iostream>
Mat PCA_RIP(Mat& img, Mat& mean, int m){

	reduce(img, mean, 1, CV_REDUCE_AVG, CV_32FC1);
	for (int i = 0; i < img.cols; ++i)
		img.col(i) -= mean;
	Mat cov = img * img.t();
	Mat al, ec;
	eigen(cov, al, ec);
	return ec.rowRange(0, m).t();
}

Mat PCA_CV(Mat& img, Mat& mean, int m){

	PCA pca(img, Mat(), CV_PCA_DATA_AS_COL, m);
	mean = pca.mean;
	for (int i = 0; i < img.cols; ++i)
		img.col(i) -= mean;
	return pca.eigenvectors.t();
}

Mat readImg(const string& link, int tsize){
	Mat res;
	ifstream f(link);
	while (!f.eof()){
		string s;
		getline(f, s);
		Mat img = imread(s, CV_LOAD_IMAGE_GRAYSCALE), x;
		//imshow("grgsd", img);	
		x = img.reshape(1, tsize);
		x = x.t();
		x.convertTo(x, CV_32FC1, 1 / 255.0);
		cout << img.rows << " " << img.cols;
		cout << x.rows << " " << x.cols;
		res.push_back(x);
	}
	f.close();
	return res.t();
}

void writeImg(const string& link, const Mat& img, int width, int height){
	ifstream f(link);
	for (int i = 0; i < img.cols; ++i){
		string s;
		getline(f, s);
		Mat x = img.col(i).reshape(width, height);
		cout << x.rows << " " << x.cols;
		x *= 255.0;
		x.convertTo(x, CV_8UC1);
		int pos = s.find_last_of('.');
		if (pos != string::npos)
			s.erase(pos);
		s += "_re.jpg";
		imwrite(s, x);
	}
	f.close();
}