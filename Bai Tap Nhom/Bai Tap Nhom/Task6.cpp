#include "Header.h"

Mat PCA_RIP(Mat& img, int m, int type){
	static Mat pca, img1, mean;

	if (type == 0){
		img1 = img.clone();
		reduce(img1, mean, 1, CV_REDUCE_AVG, CV_32FC1);
		for (int i = 0; i < img1.cols; ++i)
			img1.col(i) -= mean;
		Mat cov = img1 * img1.t(), al, ec;
		eigen(cov, al, ec);
		pca = ec.rowRange(0, m).t();
		return pca.t() * img1;
	}

	if (type == 1)
		return pca.t() * img1;

	if (type == 2){
		Mat z = pca * img;
		for (int i = 0; i < z.cols; ++i)
			z.col(i) += mean;
		return z;
	}

	return Mat();
}

Mat PCA_RIP_FACE(Mat& img, int m, int type){
	static Mat pca, img1, mean;

	if (type == 0){
		img1 = img.clone();
		reduce(img1, mean, 1, CV_REDUCE_AVG, CV_32FC1);
		for (int i = 0; i < img1.cols; ++i)
			img1.col(i) -= mean;
		Mat cov = img1.t() * img1, al, ec;
		eigen(cov, al, ec);
		pca = img1 * ec.rowRange(0, m).t();
		for (int i = 0; i < m; ++i)
			normalize(pca.col(i), pca.col(i));
		return pca.t() * img1;
	}

	if (type == 1)
		return pca.t() * img1;

	if (type == 2){
		Mat z = pca * img;
		for (int i = 0; i < z.cols; ++i)
			z.col(i) += mean;
		return z;
	}

	return Mat();
}

Mat PCA_CV(Mat& img, int m, int type){
	static PCA pca;

	if (type == 0){
		pca = PCA(img, Mat(), CV_PCA_DATA_AS_COL, m);
		return pca.project(img);
	}

	if (type == 1)
		return pca.project(img);

	if (type == 2)
		return pca.backProject(img);

	return Mat();
}

Mat readImg(const string& link, int tsize){
	Mat res;
	ifstream f(link);
	while (!f.eof()){
		string s;
		getline(f, s);
		Mat img = imread(s, CV_LOAD_IMAGE_GRAYSCALE);
		Mat x = Mat::zeros(1, tsize, CV_32FC1);
		int xpos = 0;
		for (int i = 0; i < img.rows; ++i)
		for (int j = 0; j < img.cols; ++j){
			x.at<float>(0, xpos) = img.at<uchar>(i, j) / 255.0;
			++xpos;
		}
		res.push_back(x);
	}
	f.close();
	return res.t();
}

void writeImg(const string& link, const Mat& img, int height, string a){
	ifstream f(link);
	for (int i = 0; i < img.cols; ++i){
		string s;
		getline(f, s);
		Mat y = img.col(i);
		int width = y.rows / height;
		Mat x = Mat::zeros(height, width, CV_32FC1);
		int ypos = 0;
		for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j){
			x.at<float>(i, j) = y.at<float>(ypos, 0) * 255.0;
			++ypos;
		}
		x.convertTo(x, CV_8UC1);
		int pos = s.find_last_of('.');
		if (pos != string::npos)
			s.erase(pos);
		s += "_re" + a + ".jpg";
		imwrite(s, x);
	}
	f.close();
}