#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;



int check_pixel(Vec3b pixel) // ��ĥ�� �ణ �񲸰��� �������� �ɼ��� �ֳ�? ����
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
	for (int i = len_row - 1; i >= 0; i--)
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
	for (int j = len_col - 1; j >= 0; j--)
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

vector<vector<int>> trim_list(vector<vector<int>> point_list) //��ġ�� case ���� ����
{
	vector<vector<int>> final_list;
	for (int i = 0; i < 4; i++)
	{
		if (find(final_list.begin(), final_list.end(), point_list[i]) == final_list.end())
		{
			final_list.push_back(point_list[i]);
		}
	}
	/*
	for (int i = 0; i < final_list.size(); i++)
		cout << final_list[i][0] << " " << final_list[i][1] << endl;
	*/
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
	/*
	printf("row: %d col : %d \n", len_row_img, len_col_img);
	*/

	//int * ans = (int*)malloc(sizeof(int)*2);
	vector<int> top_point, down_point, left_point, right_point;
	vector<vector<int>> point_list;
	vector<vector<int>> final_list;

	point_list.push_back(find_top_point(image, len_row_img, len_col_img));
	point_list.push_back(find_down_point(image, len_row_img, len_col_img));
	point_list.push_back(find_left_point(image, len_row_img, len_col_img));
	point_list.push_back(find_right_point(image, len_row_img, len_col_img));

	final_list = trim_list(point_list);

	return final_list;

}

int compare_image(Mat image, int** new_img)
{
	int len_row_img = image.rows;
	int len_col_img = image.cols;
	int count = 0;
	int count_original_color = 0;

	for (int i = 0; i < len_row_img; i++)
	{
		for (int j = 0; j < len_col_img; j++)
		{
			if (check_pixel(image.at<Vec3b>(i, j)) == 1) {
				count_original_color++;
			}
			if (new_img[i][j] == 1 && check_pixel(image.at<Vec3b>(i, j)) == 1)
				continue;
			else if (new_img[i][j] == 0 && check_pixel(image.at<Vec3b>(i, j)) == 0)
				continue;
			else
				count++;
		}
	}
	double accuracy = (1.0 - (double)count / (double)count_original_color) * 100;
	cout.precision(4);
	cout << "diff : " << count << " total : " << count_original_color << endl;
	cout << "accurcay : " <<  accuracy << " % " << endl << endl;
	return 0;

}

//�ﰢ���� 2���� �迭�� �׷��ִ� �Լ�
void draw_triangle(int* pos, int sizeX, int sizeY, int** result) {
	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			int d1 = (pos[0] - x) * (pos[5] - y) - (pos[4] - x) * (pos[1] - y);
			int d2 = (pos[4] - x) * (pos[3] - y) - (pos[2] - x) * (pos[5] - y);
			int d3 = (pos[2] - x) * (pos[1] - y) - (pos[0] - x) * (pos[3] - y);
			int zero_count = 0;


			// d1~d3 ���� 0�� �� ������ ���� �κ�
			if (d1 == 0) {
				zero_count++;
			}
			if (d2 == 0) {
				zero_count++;
			}
			if (d3 == 0) {
				zero_count++;
			}

			// d1~d3 �� ���� ����̰ų� �����̸� �ﰢ�� ����
			if ((d1 > 0 && d2 > 0 && d3 > 0) || (d1 < 0 && d2 < 0 && d3 < 0)) {
				result[y][x] = 1;
			}
			else if (zero_count > 1) { // 0�� 2�� �̻��̿��� �ﰢ�� ����
				result[y][x] = 1;
			}
			else if (zero_count == 1 && (d1 >= 0 && d2 >= 0 && d3 >= 0)) { // 0�� �Ѱ��̸鼭 2�� �̻��� 0���� ũ�� �ﰢ���� �� ��
				result[y][x] = 1;
			}
			else { // �� �ܿ��� �ܺ�
				result[y][x] = 0;
			}
		}
	}
}

//Ÿ���� 2���� �迭�� �׷��ִ� �Լ�
void draw_elipse(int* pos, int sizeX, int sizeY, int** result) {
	double a = (pos[3] - pos[1])/2.0; // ���� a�� ���Ѵ�.
	double b = (pos[6] - pos[4])/2.0; // ���� b�� ���Ѵ�.
	double center_x = pos[1] + a;     // x �߾� ��ǥ�� ���Ѵ�.
	double center_y = pos[4] + b;     // y �߾� ��ǥ�� ���Ѵ�.
	for (int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			if ((pow((x - center_y), 2) / pow(b, 2)) + (pow((y - center_x), 2) / pow(a, 2)) <= 1.0) { // x,y��ǥ�� Ÿ���� �������� �����ϴ��� �׽�Ʈ �Ѵ�.
				result[y][x] = 1; // �����ϸ� 1�� �����Ѵ�.
			}
			else {
				result[y][x] = 0; // �Ҹ����ϸ� 0���� �����Ѵ�.
			}
		}
	}
}


int main()
{
	Mat image;
	

	for (int i = 1; i <= 4; i++) {
		string path = "C:/input/" + to_string(i) + ".jpg";
		image = imread(path, IMREAD_COLOR);

		vector<vector<int>> point_list = find_point(image);
		int tri_point_arr[6], cir_point_arr[8];

		int HEIGHT = image.rows;
		int WIDTH = image.cols;

		int** arr = new int* [HEIGHT];
		for (int i = 0; i < HEIGHT; ++i) {
			arr[i] = new int[WIDTH];
		}

		if (point_list.size() == 3)
		{
			cout << path << " is triangle" << endl << endl;

			for (int i = 0; i < 6; i++)
			{
				tri_point_arr[i] = point_list[i / 2][1];
				i++;
				tri_point_arr[i] = point_list[i / 2][0];
			}

			draw_triangle(tri_point_arr, WIDTH, HEIGHT, arr);


			//int ** new_img;
			//�Լ� ����
			compare_image(image, arr);


		}
		else if (point_list.size() == 4)
		{
			cout << path << " is circle" << endl << endl;

			for (int i = 0; i < 8; i++)
			{
				cir_point_arr[i] = point_list[i / 2][1];
				i++;
				cir_point_arr[i] = point_list[i / 2][0];
			}


			draw_elipse(cir_point_arr, WIDTH, HEIGHT, arr);

			//int ** new_img;
			//�Լ� ����
			compare_image(image, arr);


		}
		// �޸� ����
		for (int i = 0; i < HEIGHT; ++i) {
			delete arr[i] ;
		}
		delete arr;
	}
	
	

	

	system("pause");
	return 0;
}