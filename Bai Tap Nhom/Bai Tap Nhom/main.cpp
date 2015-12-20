#include "Header.h"

/*5. Nhóm tác vụ 05- Phép biến đổi Fourier.
5.1. Phép biến đổi Fourier thuận và nghịch (sử dụng FFT).
5.2. Tính chất phép biến đổi Fourier.
5.3. Lọc thông tần số thấp dựa vào phép biến đổi Fourier và định lý Convolution.
5.4. Lọc thông tần số cao dựa vào phép biến đổi Fourier và định lý Convolution.
6. Nhóm tác vụ 06 - Phép biến đổi Karhunen-Loeve.
6.1. Phân tích thành phần chính.
6.2. Rút trích đặc trưng cho tập ảnh mặt người dựa vàp phân tích thành phần chính.
7. Nhóm tác vụ 07 - Phân đoạn ảnh.
7.1. Phương pháp Region Growing.
7.2. Phương pháp K-Means.
8. Nhóm tác vụ 08 - Toán tử hình thái học.
8.1. Toán tử hình thái học trên ảnh nhị phân.
8.2. Toán tử hình thái học trên ảnh độ xám.*/

Mat Scale_CV(const Mat &img, float ratio) {
	Mat res;
	Mat mat_scal = getRotationMatrix2D(Point(0, 0), 0, ratio);
	warpAffine(img, res, mat_scal, img.size());
	return res;
}

void run5(const Mat &img);
void run6(const Mat &img);
void run7(const Mat &img);
void run8(const Mat &img);
void run(const Mat &img);

int main() {
	// Phan chung
	string s;
	cout << "Input the link: ";
	getline(cin, s);

	Mat img = imread(s, CV_LOAD_IMAGE_GRAYSCALE);
	if (img.data == NULL)
		return 1;

	run(img);

	destroyAllWindows();
	return 0;
}


void run5(const Mat& img) {
	char choice;
	bool isFirst = true;
	Mat fouR, fouI, resR, resI, resO, fouRO, fouIO, temp[2];

	while (true)
	{
		imshow("Hinh goc", img);
		waitKey(1);
		cout << "\n5.1. Phep bien doi Fourier thuan va nghich (su dung thuat toan Cooley-Tukey)." << endl;
		cout << "5.2. Tinh chat phep bien doi Fourier." << endl;
		cout << "5.3. Loc thong tin tan so thap dua vao phep bien doi Fourier va dinh ly Convolution." << endl;
		cout << "5.4. Loc thong tin tan so cao dua vao phep bien doi Fourier va dinh ly Convolution.\n" << endl;

		cout << "Chon tac vu (0 de thoat): ";
		cin >> choice;

		if (choice - '0' <= 0)
			return;

		switch (choice - '0') {
		case 1:
			if (isFirst)
			{
				FourierThuan(img, fouR, fouI);
				Fourier_OpenCV(img, resO);
				split(resO, temp);
				fouRO = temp[0];
				fouIO = temp[1];
			}
			imshow("real", fouR * fouR.rows * fouR.cols);
			imshow("imag", fouI * fouI.rows * fouI.cols);
			imshow("real - OpenCV", fouRO);
			imshow("imag - OpenCV", fouIO);

			FourierNguoc(fouR, fouI, resR, resI);
			imshow("Fourier Nguoc", resR);

			Fourier_OpenCV(resO, resI, true);
			imshow("Fourier Nguoc - OpenCV", resI);

			if (waitKey(0) == 27){
				destroyAllWindows();
				return;
			}
			destroyAllWindows();
			break;

		case 2:
		{
				  if (isFirst)
				  {
					  FourierThuan(img, fouR, fouI);
					  Fourier_OpenCV(img, resO);
					  split(resO, temp);
					  fouRO = temp[0];
					  fouIO = temp[1];
				  }

				  int fHei = fouR.rows, fWid = fouR.cols;
				  float DCcomponent = fouR.at<float>(fHei / 2, fWid / 2) * 255,
					  RealMean = (float)mean(img)[0];

				  if (abs(DCcomponent - RealMean) <= E)
				  {
					  cout << "Thoa tinh chat DC component xap xi gia tri trung binh thuc su: " << endl;
					  cout << "DC component: " << DCcomponent << "; Mean: " << RealMean << endl;
				  }

				  bool flag = false;
				  for (int i = fHei / 2; i >= 1; --i)
				  {
					  for (int j = fWid - 1; j >= 1; --j)
					  {
						  if (flag = ((abs(fouR.at<float>(i, j) - fouR.at<float>(fHei - i, fWid - j)) > 0.0001) ||
							  (abs(fouI.at<float>(i, j) + fouI.at<float>(fHei - i, fWid - j)) > 0.0001)))
							  break;
					  }
					  if (flag)
						  break;
				  }
				  if (!flag)
					  cout << "Thoa tinh chat doi xung" << endl;

				  cout << "Dinh ly Convolution the hien o Gaussian Filter" << endl;

				  if (waitKey(0) == 27){
					  destroyAllWindows();
					  return;
				  }
				  destroyAllWindows();
				  break;
		}
		case 3: // Lọc tần số thấp
			if (isFirst)
			{
				FourierThuan(img, fouR, fouI);
				Fourier_OpenCV(img, resO);
				split(resO, temp);
				fouRO = temp[0];
				fouIO = temp[1];
			}

			FourierFilter(fouR, fouI, resO, false);
			if (waitKey(0) == 27){
				destroyAllWindows();
				return;
			}
			destroyAllWindows();
			break;

		case 4: // Lọc tần số cao
			if (isFirst)
			{
				FourierThuan(img, fouR, fouI);
				Fourier_OpenCV(img, resO);
				split(resO, temp);
				fouRO = temp[0];
				fouIO = temp[1];
			}
			FourierFilter(fouR, fouI, resO, true);
			if (waitKey(0) == 27){
				destroyAllWindows();
				return;
			}
			destroyAllWindows();
			break;

		default:
			cout << "Tac vu khong ton tai" << endl;
		}

		isFirst = false;
		system("cls");
	}
}

void run6(const Mat& img) {
	char choice;
	while (true)
	{
		imshow("Hinh goc", img);
		waitKey(1);
		cout << "\n6.1. Phan tich thanh phan chinh." << endl;
		cout << "6.2. Rut trich dac trung cho tap anh mat nguoi dua vao phan tich thanh phan chinh." << endl;

		cout << "Chon tac vu (0 de thoat): ";
		cin >> choice;

		if (choice - '0' <= 0)
			break;

		unsigned int m, height, width;
		string link;
		Mat img1, img2;

		switch (choice - '0') {
		case 1:
			cout << "Nhap so chieu ket qua: "; cin >> m;
			if (m >= img.cols || m >= img.rows)
			{
				cout << "So chieu ket qua phai nho hon so mau" << endl;
				break;
			}
			img.convertTo(img1, CV_32FC1, 1 / 255.0);
			imshow("PCA_RIP", PCA_RIP(PCA_RIP(img1, m, 0), 0, 2));
			imshow("PCA_CV", PCA_CV(PCA_CV(img1, m, 0), 0, 2));
			if (waitKey(0) == 27){
				destroyAllWindows();
				return;
			}
			destroyAllWindows();
			break;

		case 2:
			cout << "Tap tin chua duong dan cac anh mat nguoi: "; cin >> link;
			cout << "Kich thuoc anh (height x width): "; cin >> height >> width;
			cout << "Nhap so chieu ket qua: "; cin >> m;
			img.convertTo(img1, CV_32FC1, 1 / 255.0);
			img2 = readImg(link, height * width);
			if (m >= img.cols || m >= img.rows)
			{
				cout << "So chieu ket qua phai nho hon so mau" << endl;
				break;
			}

			cout << "Cac anh _RIP.jpg la ket qua rut trich dac trung RIP" << endl;
			writeImg(link, PCA_RIP_FACE(PCA_RIP_FACE(img2, m, 0), m, 2), height, "_RIP");
			cout << "Cac anh _CV.jpg la ket qua rut trich dac trung CV" << endl;
			writeImg(link, PCA_CV(PCA_CV(img2, m, 0), m, 2), height, "_CV");
			if (waitKey(0) == 27){
				destroyAllWindows();
				return;
			}
			destroyAllWindows();
			break;

		default:
			cout << "Tac vu khong ton tai" << endl;
		}
	}
}

void run7(const Mat &img) {
	char choice;

	while (true)
	{
		imshow("Hinh goc", img);
		waitKey(1);
		cout << "\n7.1. Phuong phap Region Growing." << endl;
		cout << "7.2. Phuong phap K-Means.\n" << endl;

		cout << "Chon tac vu (0 de thoat): ";
		cin >> choice;

		if (choice - '0' <= 0)
			break;

		Mat img1;
		switch (choice - '0') {
		case 1:
		{
				  float thres = 50;
				  cout << "Nhap nguong: "; cin >> thres;
				  img1 = RegionGrowing(img, abs(thres));
				  imshow("Region growing", img1);
				  if (waitKey(0) == 27){
					  destroyAllWindows();
					  return;
				  }
				  destroyAllWindows();
				  break;
		}
		case 2:
		{
				  unsigned int k = 2;
				  vector<Point3f> meanReg;
				  cout << "Nhap k: "; cin >> k;
				  img1 = KMeans_RIP(img, k, meanReg);
				  imshow("K Means", img1);
				  if (waitKey(0) == 27){
					  destroyAllWindows();
					  return;
				  }
				  destroyAllWindows();
				  break;
		}
		default:
			cout << "Tac vu khong ton tai" << endl;
		}
		system("cls");
	}
}

void run8(const Mat &img) {
	char choice;

	while (true)
	{
		imshow("Hinh goc", img);
		waitKey(1);

		cout << "\n8.1. Toan tu hinh thai hoc tren anh nhi phan." << endl;
		cout << "8.2. Toan tu hinh thai hoc tren anh grayscale.\n" << endl;

		cout << "Chon tac vu (0 de thoat): ";
		cin >> choice;

		if (choice - '0' <= 0)
			break;
		Mat des, element, temp = img.clone();
		Size size;
		cout << "Nhap kich thuoc Element structure (h-w)"; cin >> size.height >> size.width;
		if (size.width <= 0 || size.height <= 0)
			continue;

		element = getStructuringElement(MORPH_RECT, size);
		switch (choice - '0') {
		case 1:
			//adaptiveThreshold(temp, temp, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 5);
			threshold(img, temp, 127, 255, THRESH_BINARY_INV | THRESH_OTSU);
			imshow("Hinh goc", temp);
			waitKey(1);
		case 2:
			Morphology(temp, des, element, MORPH_DILATE);
			imshow("Dilate", des);
			dilate(temp, des, element);
			imshow("Dilate - OpenCV", des);
			waitKey(0);

			Morphology(temp, des, element, MORPH_ERODE);
			imshow("Erode", des);
			erode(temp, des, element);
			imshow("Erode - OpenCV", des);
			waitKey(0);

			Morphology(temp, des, element, MORPH_OPEN);
			imshow("Open", des);
			morphologyEx(temp, des, MORPH_OPEN, element);
			imshow("Open - OpenCV", des);
			waitKey(0);

			Morphology(temp, des, element, MORPH_CLOSE);
			imshow("Close", des);
			morphologyEx(temp, des, MORPH_CLOSE, element);
			imshow("Close - OpenCV", des);


			if (choice - '0' == 2)
			{
				waitKey(0);
				Morphology(temp, des, element, MORPH_GRADIENT);
				imshow("Gradient", des);
				morphologyEx(temp, des, MORPH_GRADIENT, element);
				imshow("Gradient - OpenCV", des);
				waitKey(0);

				Morphology(temp, des, element, MORPH_TOPHAT);
				imshow("Top Hat", des);
				morphologyEx(temp, des, MORPH_TOPHAT, element);
				imshow("Top Hat - OpenCV", des);
				waitKey(0);

				Morphology(temp, des, element, MORPH_BLACKHAT);
				imshow("Black Hat", des);
				morphologyEx(temp, des, MORPH_BLACKHAT, element);
				imshow("Black Hat - OpenCV", des);
				waitKey(0);

				Morphology(temp, des, element, MORPH_SMOOTH);
				imshow("Smooth operator", des);
				SmoothOperatorOpenCV(temp, des, element);
				imshow("Smooth oparator - OpenCV", des);
				waitKey(0);

				Morphology(temp, des, element, MORPH_TEXTUAL_SEGMENTATION);
				imshow("Textual Segmentation", des);
				TextualSegmentationOpenCV(temp, des, element);
				imshow("Textual Segmentation - OpenCV", des);
			}
			if (waitKey(0) == 27){
				destroyAllWindows();
				return;
			}
			destroyAllWindows();
			break;

		default:
			cout << "Tac vu khong ton tai" << endl;
		}
		system("cls");
	}
}

void run(const Mat& img) {
	char choice;

	while (true)
	{
		imshow("Hinh goc", img);
		waitKey(1);
		cout << "\nNhom tac vu 05 - Phep bien doi Fourier." << endl;
		cout << "Nhom tac vu 06 - Phep bien doi Karhunen-Loeve." << endl;
		cout << "Nhom tac vu 07 - Phan doan anh." << endl;
		cout << "Nhom tac vu 08 - Toan tu hinh thai hoc.\n" << endl;

		cout << "Chon nhom tac vu (0 de thoat): ";
		cin >> choice;

		if (choice - '0' <= 0)
			return;

		switch (choice - '0'){
		case 5:
			run5(img);
			break;
		case 6:
			run6(img);
			break;
		case 7:
			run7(img);
			break;
		case 8:
			run8(img);
			break;
		default:
			cout << "Nhom tac vu khong ton tai" << endl;
		}
		system("cls");
	}
}