// This cpp file needs to be used instead of main function - in this case, remove the SpeckePatternTriangle.cpp file and replace with this one

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

Mat translateImg(Mat &img, int offsetx, int offsety) {
	Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	warpAffine(img, img, trans_mat, img.size());
	return img;
}

// small perspective warps
void perspective(Mat& img, int pxNum)
{
	// Input Quadilateral or Image plane coordinates
	Point2f inputQuad[4];
	// Output Quadilateral or World plane coordinates
	Point2f outputQuad[4];

	// Lambda Matrix
	Mat lambda(2, 4, CV_32FC1);


	// The 4 points that select quadilateral on the input , from top-left in clockwise order
	// These four pts are the sides of the rect box used as input 
	inputQuad[0] = Point2f(0, 0);
	inputQuad[1] = Point2f(img.cols, 0);
	inputQuad[2] = Point2f(img.cols, img.rows);
	inputQuad[3] = Point2f(0, img.rows);
	// The 4 points where the mapping is to be done , from top-left in clockwise order
	outputQuad[0] = Point2f(0, 0);
	outputQuad[1] = Point2f(img.cols, pxNum);
	outputQuad[2] = Point2f(img.cols, img.rows - pxNum);
	outputQuad[3] = Point2f(0, img.rows);

	// Get the Perspective Transform Matrix i.e. lambda 
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
	// Apply the Perspective Transform just found to the src image
	warpPerspective(img, img, lambda, img.size());
}

int main(int argc, char** argv)
{
	Mat startFrame = imread("././test3/0.jpg");

	//imshow("1st frame", startFrame);
	//waitKey(0);

	VideoWriter video("test7/randomPattern.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 8, Size(startFrame.cols, startFrame.rows));
	video.write(startFrame);

	Mat newFrame = startFrame.clone();

	for (int i = 1; i < 200; i++)
	{
		newFrame = startFrame.clone();
		//newFrame = translateImg(newFrame, i, i);										// translate
		perspective(newFrame, i*2);
		//imshow("1st frame", newFrame);
		//waitKey(0);
		video.write(newFrame);
	}

	video.release();

	return 0;
}