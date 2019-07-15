// The goal of this project is to track defformations on a solid object by observing a speckle pattern that is sprayed on its surface.
// This can be done using many methods but the one tested and proposed in this project is a new one. Deformations are tracked using a
// grid of horizontal and vertical that is artificialy drawn on the sprayed object. The grid is undeformed before any force is applied
// and it will be deformed afterwards. Points of intersections in the grid are tracked from one frame to the next by observing three 
// points closest to the intersection. The intersection is in the 'centroid' (+ an offset) of the triangle formed by the three closest points. 
// Those points are tracked from one frame to the next by assuming that the changes caused by the defforming force will only move by 
// a small amount.

// Masks suck

// This program lacks any error checking and is mostly used as a proof of concept

#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include <iostream>
#include "Grid.h"
#include "opencv2/opencv.hpp"

#define GRID_SIZE_FACTOR			100					// Size of grid rectangle in pixels

// Global variable used to store position of mouse click
Point2i position;

// Funciton to extract position of mouse click
static void mouseCallback(int event, int x, int y, int flags, void* userdata)
{
	if (flags == EVENT_FLAG_LBUTTON) 
	{
		position = Point2i(x, y);
		cout << "Position of click: (" << position.x << ", " << position.y << ")" << endl;
	}
}

int main(int argc, char** argv)
{
	Mat img;											// Frame read from video input
	Mat imgGrid;										// Read frame with grid drawn over it
	Mat imgVChannel;									// V channel in HSV color system

	// Variables used for drawing grid
	int gridXsize = 0;
	int gridYsize = 0;
	int XRem = 0;
	int YRem = 0;

	Point2i TLC;										// ROI top left corner position
	Point2i BRC;										// ROI bottom right corner position

	vector<GridPoint> Grid0;							// Grid location in first frame
	vector<GridPoint> GridPrev;							// Grid in previous iteration
	vector<GridPoint> GridCurr;							// Grid in current iteration
	
	// Create a VideoCapture object and open the input file
	VideoCapture cap("test10/randomPattern.mp4");

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	// Read frame
	cap.read(img);

	// Define the codec and create VideoWriter object.
	VideoWriter video("test10/out.mp4", VideoWriter::fourcc('m', 'p', '4', 'v'), cap.get(CAP_PROP_FPS), Size(img.cols, img.rows));



	// Get TLC position
	namedWindow("Select top left corner of roi", WINDOW_AUTOSIZE);
	setMouseCallback("Select top left corner of roi", mouseCallback, NULL);
	imshow("Select top left corner of roi", img);
	waitKey(0);
	cout << "Selected position is (" << position.x << ", " << position.y << ")"<< endl;
	TLC = position;
	destroyAllWindows();

	// Get BRC position
	imgGrid = img.clone();
	circle(imgGrid, TLC, 2, Scalar(255, 255, 0), 3);	// Draw small circle at the location of first click
	namedWindow("Select bottom right corner of roi", WINDOW_AUTOSIZE);
	setMouseCallback("Select bottom right corner of roi", mouseCallback, NULL);
	imshow("Select bottom right corner of roi", imgGrid);
	waitKey(0);
	cout << "Selected position is (" << position.x << ", " << position.y << ")" << endl;
	BRC = position;
	destroyAllWindows();


	// Calculate grid coordinates
	gridXsize = round((BRC.x - TLC.x) / GRID_SIZE_FACTOR) + 1;
	gridYsize = round((BRC.y - TLC.y) / GRID_SIZE_FACTOR) + 1;
	XRem = (BRC.x - TLC.x) % GRID_SIZE_FACTOR;
	YRem = (BRC.y - TLC.y) % GRID_SIZE_FACTOR;

	// Separate V channel - use just the V channel since it holds all the necessary information 
	cvtColor(img, imgVChannel, COLOR_BGR2HSV);
	cv::extractChannel(imgVChannel, imgVChannel, 2);

	/* INSERT NECESSARY PROCESSING HERE - I didn't know what would be necessary */

	// Create grid
	gridCreate(imgVChannel, Grid0, gridXsize, gridYsize, GRID_SIZE_FACTOR, TLC + Point2i(XRem/2,YRem/2));

	// Draw grid for reference
	gridDraw(img, imgGrid, Grid0, gridXsize, gridYsize);

	video.write(imgGrid);
	
	GridPrev = Grid0;

	// Read and process frames in video
	int i = 1;
	while (cap.read(img))
	{
		Mat imageWGrid;
		cvtColor(img, imgVChannel, COLOR_BGR2HSV);
		cv::extractChannel(imgVChannel, imgVChannel, 2);

		/* INSERT NECESSARY PROCESSING HERE - I didn't know what would be necessary */

		cout << "Processing frame: " << i++ << endl;
		findNewGrid(imgVChannel, GridPrev, GridCurr, gridXsize, gridYsize, GRID_SIZE_FACTOR);
		gridDraw(img, imageWGrid, GridCurr, gridXsize, gridYsize);
		video.write(imageWGrid);
		GridPrev = GridCurr;
	}
	// When everything done, release the video capture and write object
	cap.release();
	video.release();
	

	return 0;
}

