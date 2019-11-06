/**
@file videocapture_basic.cpp
@brief A very basic sample for using VideoCapture and VideoWriter
@author PkLab.net
@date Aug 24, 2016
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std;


int find_same_array(int arr[], int arr2[])
{
	for (int i = 0; i < 9; i++)
	{
		if (arr[i] != arr2[i])
		{
			return -1;
		}

	}
	return 0;
}



int dist_arr(int arr[])
{
	int pattern_land[9] = { 0,1,0,1,0,1,0,1,0 };
	int pattern_takeoff[9] = { 1,0,1,0,1,0,1,0,1 };

	if (find_same_array(arr, pattern_land) != -1)
	{
		printf("-----TAKEOFF MARKER DETECT-----\n");
		return 1;
	}
	if (find_same_array(arr, pattern_takeoff) != -1)
	{
		printf("-----LAND MARKER DETECT-----\n");
		return 2;
	}
}


int find_marker(Mat image)
{
	
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	//namedWindow("Original", WINDOW_AUTOSIZE);

	Mat input_gray_image;
	cvtColor(image, input_gray_image, COLOR_BGR2GRAY);

	Mat binary_image;
	//adaptiveThreshold(input_gray_image, binary_image,
	//255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 91, 7);
	threshold(input_gray_image, binary_image, 125, 255, THRESH_BINARY_INV | THRESH_OTSU);
	//imshow("binary", binary_image);

	Mat contour_image = binary_image.clone();
	vector<vector<Point> > contours;
	findContours(contour_image, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

	/// Draw contours 
	Mat drawing = Mat::zeros(contour_image.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(0, 255, 0);
		drawContours(drawing, contours, i, color, 2, 8, RETR_LIST, 0, Point2i());
	}

	//imshow("Original", drawing);

	//contour�� �ٻ�ȭ�Ѵ�.
	vector<vector<Point2f> > marker;
	vector<vector<Point2f> > select_marker;
	vector<Point2f> approx;

	// �˻�˰��� �߰����� �־����
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.05, true);

		if (
			approx.size() == 4 && //�簢���� 4���� vertex�� ������. 
			fabs(contourArea(Mat(approx))) > 5000 && //������ ����ũ�� �̻��̾�� �Ѵ�.
			fabs(contourArea(Mat(approx))) < 200000  //������ ����ũ�� ���Ͽ��� �Ѵ�. 
			//isContourConvex(Mat(approx)) //convex���� �˻��Ѵ�.
			)
		{
			//printf("is correct\n");
			drawContours(drawing, contours, i, Scalar(0, 0, 255), 1, LINE_AA);

			vector<cv::Point2f> points;
			for (int j = 0; j < 4; j++)
				points.push_back(cv::Point2f(approx[j].x, approx[j].y));

			//�ݽð� �������� ����
			cv::Point v1 = points[1] - points[0];
			cv::Point v2 = points[2] - points[0];

			double o = (v1.x * v2.y) - (v1.y * v2.x);
			if (o < 0.0)
				swap(points[1], points[3]);

			marker.push_back(points);

		}
	}




	vector<Point2f> approx_two;

	int area_one, area_two = 0;
	int k = 0;

	for (size_t i = 0; i < marker.size(); i++)
	{
		approx = marker[i];
		for (size_t j = 0; j < marker.size(); j++)
		{
			approx_two = marker[j];
			area_one = fabs(contourArea(Mat(approx)));
			area_two = fabs(contourArea(Mat(approx_two)));
			k = area_one;
			k = k * 16 / 25;
			

			if (area_one > area_two * 15 / 25 && area_one < area_two * 17 / 25)
			{
				//drawContours(drawing, approx_two, 0, Scalar(0, 0, 255), 1, LINE_AA);
				select_marker.push_back(approx_two);
				break;
			}
		}
	}

	//����

	vector<vector<Point2f> > detectedMarkers;
	vector<Mat> detectedMarkersImage;
	vector<Point2f> square_points;

	int marker_image_side_length = 100; //��Ŀ 10x10ũ���϶� ������ �׵θ� ���� ������ ũ��� 10*10
								//���� �ܰ迡�� �̹����� ���ڷ� ������ �� ���ϳ��� �ȼ��ʺ� 10���� �Ѵٸ�
								//��Ŀ �̹����� �Ѻ� ���̴� 100
	square_points.push_back(cv::Point2f(0, 0));
	square_points.push_back(cv::Point2f(marker_image_side_length - 1, 0));
	square_points.push_back(cv::Point2f(marker_image_side_length - 1, marker_image_side_length - 1));
	square_points.push_back(cv::Point2f(0, marker_image_side_length - 1));

	Mat marker_image;
	for (int i = 0; i < select_marker.size(); i++)
	{
		vector<Point2f> m = select_marker[i];

		Mat input_gray_image2 = input_gray_image.clone();
		Mat markerSubImage = input_gray_image2(cv::boundingRect(m));
		//imshow("marker sub", markerSubImage);

		//��Ŀ�� �簢�����·� �ٲ� perspective transformation matrix�� ���Ѵ�.
		Mat PerspectiveTransformMatrix = getPerspectiveTransform(m, square_points);
		//Mat PerspectiveTransformMatrix = getAffineTransform(m, square_points);

		//perspective transformation�� �����Ѵ�. 
		warpPerspective(input_gray_image, marker_image, PerspectiveTransformMatrix, Size(marker_image_side_length, marker_image_side_length));

		//otsu ������� ����ȭ�� �����Ѵ�. 
		threshold(marker_image, marker_image, 125, 255, THRESH_BINARY | THRESH_OTSU);



		//��Ŀ�� ũ��� 6, ������ �µθ��� ������ ũ��� 8
		//��Ŀ �̹��� �׵θ��� �˻��Ͽ� ���� ���������� Ȯ���Ѵ�. 
		int cellSize = marker_image.rows / 10;
		int white_cell_count = 0;
		for (int y = 0; y < 10; y++)
		{
			int inc = 9; // ù��° ���� ������ ���� �˻��ϱ� ���� ��

			if (y == 0 || y == 9) inc = 1; //ù��° �ٰ� ���������� ��� ���� �˻��Ѵ�. 


			for (int x = 0; x < 10; x += inc)
			{
				int cellX = x * cellSize;
				int cellY = y * cellSize;
				cv::Mat cell = marker_image(Rect(cellX, cellY, cellSize, cellSize));

				int total_cell_count = countNonZero(cell);
				//printf("%f \n", float(total_cell_count) / (cellSize*cellSize));
				if (total_cell_count > (cellSize*cellSize) / 2)
					white_cell_count++; //�µθ��� ��������� �ִٸ�, ������ �ȼ��� �����̻� ����̸� ����������� ���� 

			}
			//printf("%d", white_cell_count);
		}

		//������ �µθ��� �ѷ��׿� �ִ� �͸� �����Ѵ�.
		if (white_cell_count == 0) {
			detectedMarkers.push_back(m);
			Mat img = marker_image.clone();
			detectedMarkersImage.push_back(img);
		}
	}

	int pattern_arr[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int count = 0;

	vector<Mat> bitMatrixs;
	for (int i = 0; i < detectedMarkers.size(); i++)
	{
		Mat marker_image = detectedMarkersImage[i];

		//���� 6x6�� �ִ� ������ ��Ʈ�� �����ϱ� ���� ����
		Mat bitMatrix = Mat::zeros(3, 3, CV_8UC1);

		int cellSize = marker_image.rows / 5;
		for (int y = 0; y < 3; y++)
		{
			
			for (int x = 0; x < 3; x++)
			{
				int cellX = (x + 1)*cellSize;
				int cellY = (y + 1)*cellSize;
				Mat cell = marker_image(cv::Rect(cellX, cellY, cellSize, cellSize));
				int total_cell_count = countNonZero(cell);
				if (total_cell_count > (cellSize*cellSize) / 2) {
					printf("��");
					bitMatrix.at<uchar>(y, x) = 1;
					pattern_arr[count] = 0;
				}
				else
				{
					printf("��");
					pattern_arr[count] = 1;
				}
				count += 1;
			}
			printf("\n");
		}
		bitMatrixs.push_back(bitMatrix);
		//imshow("Find marker", marker_image);
		waitKey(0);

		return dist_arr(pattern_arr);
		
		//Mat marker_land = (Mat_<uchar>(3, 3) << 0, 1, 0, 1, 0, 1, 0, 1, 0);
		//Mat compareMatrix = Mat::zeros(3, 3, CV_8UC1);
		//compare(marker_land, bitMatrix,compareMatrix, answer);
		//printf("%d", answer);

	}



}

int check_pixel(Vec3b pixel)
{
	for (int i = 0; i < 3; i++)
	{
		if (pixel[i] < 10)
		{
			cout << "find point : " << pixel << endl;
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



int find_point(Mat image)
{

	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		return -1;
	}

	//namedWindow("Original", WINDOW_AUTOSIZE);

	Mat input_gray_image;
	cvtColor(image, input_gray_image, COLOR_BGR2GRAY);

	int len_row_img = input_gray_image.rows;
	int len_col_img = input_gray_image.cols;
	printf("row: %d col : %d \n",len_row_img,len_col_img);

	cout << image.at<Vec3b>(0, 294) << endl;

	//int * ans = (int*)malloc(sizeof(int)*2);
	vector<int> point;
	point = find_top_point(image, len_row_img, len_col_img);
	cout << point[0] << " "<< point[1] << endl;

	point = find_down_point(image, len_row_img, len_col_img);
	cout << point[0] << " " << point[1] << endl;

	point = find_left_point(image, len_row_img, len_col_img);
	cout << point[0] << " " << point[1] << endl;

	point = find_right_point(image, len_row_img, len_col_img);
	cout << point[0] << " " << point[1] << endl;

	return 0;

}


int main()
{
	Mat image;
	image = imread("img/circle1.jpg", IMREAD_COLOR);
	printf("%d",find_point(image));
	//system("pause");
	return 0;
}