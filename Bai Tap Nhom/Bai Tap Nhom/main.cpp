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

void run5(Mat& img);
void run6(Mat& img);
void run7(Mat& img);
void run8(Mat& img);
void run (Mat& img);

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


void run5(Mat& img) {
	int choice;
	bool isFirst = true;
	Mat fouR, fouI, resR, resI;
	
	while (true)
	{
		imshow("Hinh goc", img);
		cout << "\n5.1. Phep bien doi Fourier thuan va nghich (su dung thuat toan Cooley-Tukey)." << endl;
		cout << "5.2. Tinh chat phep bien doi Fourier." << endl;
		cout << "5.3. Loc thong tin tan so thap dua vao phep bien doi Fourier va dinh ly Convolution." << endl;
		cout << "5.4. Loc thong tin tan so cao dua vao phep bien doi Fourier va dinh ly Convolution.\n" << endl;

		cout << "Chon tac vu (0 de thoat): ";
		cin >> choice;

		if (!choice)
			return;

		switch (choice) {
		case 1:
			if (isFirst)
				FourierThuan(img, fouR, fouI);
			imshow("real", fouR * fouR.rows * fouR.cols);
			imshow("imag", fouI * fouI.rows * fouI.cols);
			FourierNguoc(fouR, fouI, resR, resI);
			imshow("Fourier Nguoc", resR);
			if (waitKey(0) == 27){
				destroyAllWindows();
				return;
			}
			destroyAllWindows();
			break;

		case 2:
		{
			if (isFirst)
				FourierThuan(img, fouR, fouI);
			
			int fHei = fouR.rows, fWid = fouR.cols;
			float DCcomponent = fouR.at<float>(fHei/2, fWid/2)*255,
				RealMean = (float)mean(img)[0];
			
			bool flag = false;
			for (int i = 1; i <= fHei/2; ++i)
			{
				for (int j = 1; j < fWid; ++j)
					if (flag = (fouR.at<float>(i,j) != fouR.at<float>(fHei-i,fWid-j)))
						break;
				if (flag)
					break;
			}
			if (!flag)
				cout << "Tinh chat ... thoa" << endl;
			
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
				FourierThuan(img, fouR, fouI);
			FourierFilter(fouR, fouI, false);
			if (waitKey(0) == 27){
				destroyAllWindows();
				return;
			}
			destroyAllWindows();
			break;

		case 4: // Lọc tần số cao
			if (isFirst)
				FourierThuan(img, fouR, fouI);
			FourierFilter(fouR, fouI, true);
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

void run6(Mat& img) {
	cout << "\n6.1.Phan tich thanh phan chinh." << endl;
	cout << "6.2.Rut trich dac trung cho tap anh mat nguoi dua vao phan tich thanh phan chinh.\n" << endl;

	int choice;
	cout << "Chon tac vu: ";
	cin >> choice;

	Mat img1, img2;

	switch (choice) {
	case 1:

		if (waitKey(0) == 27){
			destroyAllWindows();
			return;
		}
		destroyAllWindows();
		break;

	case 2:
	
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

void run7(Mat& img) {
	cout << "\n7.1. Phuong phap Region Growing." << endl;
	cout << "7.2.Phuong phap K-Means.\n" << endl;

	int choice;
	cout << "Chon tac vu: ";
	cin >> choice;

	Mat img1, img2;

	switch (choice) {
	case 1:

		if (waitKey(0) == 27){
			destroyAllWindows();
			return;
		}
		destroyAllWindows();
		break;

	case 2:
	
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

void run8(Mat& img) {
	cout << "\n8.1. Toan tu hinh thai hoc tren anh nhi phan." << endl;
	cout << "8.2.Toan tu hinh thai hoc trn anh grayscale.\n" << endl;

	int choice;
	cout << "Chon tac vu: ";
	cin >> choice;

	Mat img1, img2;

	switch (choice) {
	case 1:

		if (waitKey(0) == 27){
			destroyAllWindows();
			return;
		}
		destroyAllWindows();
		break;

	case 2:
	
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

void run(Mat& img) {
	int choice;
	
	while (true)
	{
		imshow("Hinh goc", img);
		cout << "\nNhom tac vu 05 - Phep bien doi Fourier." << endl;
		cout << "Nhom tac vu 06 - Phep bien doi Karhunen-Loeve." << endl;
		cout << "Nhom tac vu 07 - Phan doan anh." << endl;
		cout << "Nhom tac vu 08 - Toan tu hinh thai hoc.\n" << endl;

		cout << "Chon nhom tac vu (0 de thoat): ";
		cin >> choice;

		if (!choice)
			return;

		switch (choice){
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