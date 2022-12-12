#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/tracking.hpp"

using namespace cv;
using namespace std;

int height,width;

Mat getSubmatrix(int x,int y,int h,int w,Mat& m)
{
	return m.clone()(Range(x,x+w),Range(y,y+h));
}

void addNoise(Mat m)
{
	int w = m.size().width,h = m.size().height;
	int width = 20;
	for(int i = 0;i<h;i++)
	{
		for(int j = 0;j<w;j++)
		{
			m.at<uchar>(i,j) += ((width*rand())/RAND_MAX)-(width/2);
		}
	}
}
long long superimpose(Mat orig,Mat sub,int x,int y,int h,int w)
{
	long long mse = 0;
	for(int i = x;i<w+x;i++)
	{
		for(int j = y;j<h+y;j++)
		{
			long long  ne1 = orig.at<uchar>(i,j);
			long long ne2 = sub.at<uchar>(i-x,j-y);
			mse += (ne1-ne2)*(ne1-ne2);
		}
	}
	return mse;
}

void getMatrix(Mat m)
{
	int w = m.size().width,h = m.size().height;
	cout<<'{';
	for(int i = 0;i<h;i++)
	{
		cout<<'{';
		for(int j = 0;j<w;j++)
		{
			cout<<(int)m.at<uchar>(i,j);
			if(j!=w-1)
			cout<<',';
		}
		cout<<"},\n";
	}
	cout<<"}\n";
}


int winsize;
int main() {
	winsize = 35;
	int startX = 20,startY = 120;
	
	cv::Mat frame;
	frame = imread("dog-stock.webp");
	cvtColor(frame, frame, CV_BGR2GRAY);
	frame = getSubmatrix(180,200,200,200,frame);
	height = frame.size().width,width = frame.size().height;
	Mat err = Mat::zeros(Size(width-winsize,height-winsize),CV_8UC1);

	Mat sub = getSubmatrix(startX,startY,winsize,winsize,frame);
	addNoise(sub);
	imshow("sub",sub);
	vector<vector<long long>> buf;
	long long ma = 0;
	for(int i = 0;i<width-winsize;i++)
	{
		vector<long long> temp;
		for(int j = 0;j<height-winsize;j++)
		{
			long long a = superimpose(frame,sub,i,j,winsize,winsize);
			 ma = max(ma,a);
			 temp.push_back(a);
		}
		buf.push_back(temp);
	}
	for(auto &x:buf)
	{
		for(auto &y:x)
		{
			y = (y*255)/ma;
		}
	}
	int minX,minY;
	int minVal = INT_MAX;
	for(int i = 0;i<width-winsize;i++)
	{
		for(int j = 0;j<height-winsize;j++)
		{
			err.at<uchar>(j,i) = buf[j][i];
			if(minVal>buf[j][i])
			{
				minVal = buf[j][i];
				minX = i,minY = j;
			}
		}
	}
	cout<<minX<<" "<<minY<<endl;
	Point p1 = Point(minX,minY);
	Point p2 = Point(minX+winsize,minY+winsize);
	cvtColor(frame,frame,COLOR_GRAY2BGR);
	cvtColor(err,err,COLOR_GRAY2BGR);
	err.at<Vec3b>(minY,minX) = {0,255,0};
	cv::rectangle(frame, p1,p2, cv::Scalar(0, 255, 0), 2, 8);
	cv::imshow("original",frame);
	imshow("error",err);
	waitKey(0);
	//getMatrix(frame);
	
	return 0;

}
