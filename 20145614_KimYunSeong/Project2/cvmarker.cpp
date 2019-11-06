

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std;



int check_pixel(Vec3b pixel) // 겹칠때 약간 비껴가서 여러개가 될수도 있나? 주의
{
	for (int i = 0; i < 3; i++)
	{
		if (pixel[i] < 60)
		{
			//cout << "find point : " << pixel << endl;
			return 1;
		}
	}
	return 0;
}


vector<int> find_top_point(Mat image, int len_row, int len_col)
{
	int find = 0;
	vector<int> ans;
	for (int i = 0; i < len_row; i++)
	{
		//if (find != 0) break;
		for (int j = 0; j < len_col; j++)
		{
			//if (find != 0) break;
			if (check_pixel(image.at<Vec3b>(i, j)) == 1)
			{
				ans.push_back(i);
				ans.push_back(j);
				return ans;
			}
		}
	}
	return ans;
}

vector<int> find_down_point(Mat image, int len_row, int len_col)
{
	int find = 0;
	vector<int> ans;
	for (int i = len_row-1; i >= 0 ; i--)
	{
		//if (find != 0) break;
		for (int j = 0; j < len_col; j++)
		{
			//if (find != 0) break;
			if (check_pixel(image.at<Vec3b>(i, j)) == 1)
			{
				ans.push_back(i);
				ans.push_back(j);
				return ans;
			}
		}
	}
	return ans;
}

vector<int> find_left_point(Mat image, int len_row, int len_col)
{
	int find = 0;
	vector<int> ans;
	for (int j = 0; j < len_col; j++)
	{
		//if (find != 0) break;
		for (int i = 0; i < len_row; i++)
		{
			//if (find != 0) break;
			if (check_pixel(image.at<Vec3b>(i, j)) == 1)
			{
				ans.push_back(i);
				ans.push_back(j);
				return ans;
			}
		}
	}
	return ans;
}


vector<int> find_right_point(Mat image, int len_row, int len_col)
{
	int find = 0;
	vector<int> ans;
	for (int j = len_col-1; j >= 0; j--)
	{
		//if (find != 0) break;
		for (int i = 0; i < len_row; i++)
		{
			//if (find != 0) break;
			if (check_pixel(image.at<Vec3b>(i, j)) == 1)
			{
				ans.push_back(i);
				ans.push_back(j);
				return ans;
			}
		}
	}
	return ans;
}

vector<vector<int>> trim_list(vector<vector<int>> point_list) //겹치는 case 생각 주의
{
	vector<vector<int>> final_list;
	for (int i = 0; i < 4; i++)
	{
		if (find(final_list.begin(), final_list.end(), point_list[i]) == final_list.end())
		{
			final_list.push_back(point_list[i]);
		}
	}

	for (int i = 0; i < final_list.size(); i++)
		cout << final_list[i][0] << " " << final_list[i][1] << endl;

	return final_list;
}

vector<vector<int>> find_point(Mat image)
{

	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
	}

	//namedWindow("Original", WINDOW_AUTOSIZE);

	Mat input_gray_image;
	cvtColor(image, input_gray_image, COLOR_BGR2GRAY);

	int len_row_img = input_gray_image.rows;
	int len_col_img = input_gray_image.cols;
	printf("row: %d col : %d \n",len_row_img,len_col_img);


	//int * ans = (int*)malloc(sizeof(int)*2);
	vector<int> top_point,down_point,left_point,right_point;
	vector<vector<int>> point_list;
	vector<vector<int>> final_list;

	point_list.push_back(find_top_point(image, len_row_img, len_col_img));
	point_list.push_back(find_down_point(image, len_row_img, len_col_img));
	point_list.push_back(find_left_point(image, len_row_img, len_col_img));
	point_list.push_back(find_right_point(image, len_row_img, len_col_img));

	final_list = trim_list(point_list);

	return final_list;

}

int compare_image(Mat image,int ** new_img)
{
	int len_row_img = image.rows;
	int len_col_img = image.cols;
	int count = 0;

	for (int i = 0; i < len_row_img; i++)
	{
		for (int j = 0; j < len_col_img; j++)
		{
			if (new_img[i][j] == 1 && check_pixel(image.at<Vec3b>(i, j)) == 1)
				continue;
			else if (new_img[i][j] == 0 && check_pixel(image.at<Vec3b>(i, j)) == 0)
				continue;
			else
				count++;
		}
	}

	cout << count << " 픽셀 차이 발생" << endl;
	return 0;

}


void draw_triangle(int* pos, int sizeX, int sizeY, int** result) {
	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			int d1 = (pos[0] - x) * (pos[5] - y) - (pos[4] - x) * (pos[1] - y);
			int d2 = (pos[4] - x) * (pos[3] - y) - (pos[2] - x) * (pos[5] - y);
			int d3 = (pos[2] - x) * (pos[1] - y) - (pos[0] - x) * (pos[3] - y);
			int zero_count = 0;


			if (d1 == 0) {
				zero_count++;
			}
			if (d2 == 0) {
				zero_count++;
			}
			if (d3 == 0) {
				zero_count++;
			}

			if ((d1 > 0 && d2 > 0 && d3 > 0) || (d1 < 0 && d2 < 0 && d3 < 0)) {
				result[y][x] = 1;
			}
			else if (zero_count > 1) {
				result[y][x] = 1;
			}
			else {
				result[y][x] = 0;
			}
		}
	}
}

void draw_elipse(int* pos, int sizeX, int sizeY, int** result) {
	double a = abs(pos[2] - pos[0]) / 2;
	double b = abs(pos[7] - pos[5]) / 2;
	double center_x = pos[0] + a;
	double center_y = pos[5] + b;

	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			if (pow((x - center_x), 2) / pow(a, 2) + pow((y - center_y), 2) / pow(b, 2) <= 1) {
				result[sizeY - 1 - y][x] = 1;
			}
			else {
				result[sizeY - 1 - y][x] = 0;
			}
		}
	}

}


int main()
{
	Mat image;
	image = imread("img/circle1.jpg", IMREAD_COLOR);
	vector<vector<int>> point_list = find_point(image);

	int len_row_img = image.rows;
	int len_col_img = image.cols;
	int tri_point_arr[6], cir_point_arr[8];

	if (point_list.size() == 3)
	{
		cout << "answer is triangle\n" << endl;

		for (int i = 0; i < 6; i++)
		{
			tri_point_arr[i] = point_list[i/2][1];
			i++;
			tri_point_arr[i] = point_list[i/2][0];	
		}

		int sizeY = len_row_img;
		int sizeX = len_col_img;
		
		int** arr = new int*[sizeY];
		for (int i = 0; i < sizeY; ++i) {
			arr[i] = new int[sizeX];
		}
		int count = 0;
		draw_triangle(tri_point_arr, sizeX, sizeY, arr);


		//int ** new_img;
		//함수 실행
		compare_image(image, arr);

	}
	else if (point_list.size() == 4)
	{
		cout << "answer is circle\n" << endl;

		for (int i = 0; i < 8; i++)
		{
			cir_point_arr[i] = point_list[i / 2][1];
			i++;
			cir_point_arr[i] = point_list[i / 2][0];
		}

		int sizeY = len_row_img;
		int sizeX = len_col_img;

		int** arr = new int* [sizeY];
		for (int i = 0; i < sizeY; ++i) {
			arr[i] = new int[sizeX];
		}
		int count = 0;
		draw_elipse(cir_point_arr, sizeX, sizeY, arr);

		//int ** new_img;
		//함수 실행
		compare_image(image, arr);
	}





	system("pause");
	return 0;
}