#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/tracking.hpp"

using namespace cv;
using namespace std;

int height,width;

void convertGrayscale(Mat& frame)
{
	Mat& ret = frame;
	for(int i = 0;i<height;i++)
	{
		for(int j = 0;j<width;j++)
		{
			Vec3b pix = frame.at<Vec3b>(i,j)[0];
			int comb = 2*(pix[0]+pix[1]+pix[2])/3;
			pix[0] = pix[1] = pix[2] = comb;
			ret.at<Vec3b>(i,j) = pix; 
		}
	}
	frame = ret;
}

Mat getSubmatrix(int x,int y,int h,int w,Mat& m)
{
	return m.clone()(Range(x,x+w),Range(y,y+h));
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


int winsize = 200;

int main() {

	cv::Mat frame;
	frame = imread("dog-stock.webp");
	cvtColor(frame, frame, CV_BGR2GRAY);
	height = frame.size().width,width = frame.size().height;
	Mat sub = getSubmatrix(200,200,winsize,winsize,frame);
	imshow("sub",sub);
	cv::imshow("original",frame);
	Mat err = Mat::zeros(Size(width-winsize,height-winsize),CV_8UC1);
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
	for(int i = 0;i<width-winsize;i++)
	{
		for(int j = 0;j<height-winsize;j++)
		{
			err.at<uchar>(j,i) = buf[j][i];
		}
	}
	imshow("error",err);
	waitKey(0);
	return 0;

}