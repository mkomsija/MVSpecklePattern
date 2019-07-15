#include "Grid.h"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include "opencv2/imgproc.hpp"

// Calculate Euclidean distance between two points
double euclideanNorm(const Point2i& a, const Point2i& b)
{
	double distX = a.x - b.x;
	double distY = a.y - b.y;
	return sqrt(distX * distX + distY * distY);
}

void gridCreate(const Mat& image, vector<GridPoint>& Grid, const int& gridXsize, const int& gridYsize, const int& size_factor, Point2i OffsetTop)
{
	for (int i = 0; i < gridXsize; i++)
	{
		for (int j = 0; j < gridYsize; j++)
		{
			Grid.push_back(GridPoint(Point2i(i*size_factor + OffsetTop.x, j*size_factor + OffsetTop.y)));
		}
	}

	// Detect blobs
	SimpleBlobDetector::Params blobParams;

	blobParams.filterByArea = true;
	blobParams.minArea = 10;

	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(blobParams);

	// Draw all keypoints - for debug purposes 
	//vector<KeyPoint> keypoints;
	//Mat imageKeypoints;
	//detector->detect(image, keypoints);
	//drawKeypoints(image, keypoints, imageKeypoints, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//imshow("all keypoints", imageKeypoints);
	//waitKey(0);

	// Find triangles
	for (int o = 0; o < Grid.size(); o++)
	{
		GridPoint* gridPt = &Grid[o];
		vector<KeyPoint> keyPts;
		int rectHalfSize = 5;
		Mat neighborhood;
		double minDist = 10;
		Mat mask = Mat::zeros(image.size(), CV_8U);
		Mat imageCp = image.clone();

		// Expand neighborhood until there is at least 10 keypoints
		while (keyPts.size() < 10)
		{
			rectHalfSize += 5;
			if (rectHalfSize > size_factor)
			{
				std::cout << "Not enough keypoints found in neighborhood!" << endl;
				return;
			}
			Rect r(gridPt->Q.x - rectHalfSize, gridPt->Q.y - rectHalfSize, 2 * rectHalfSize + 1, 2 * rectHalfSize + 1);
			neighborhood = image(r);						// Cut the neighborhood from input image
			detector->detect(neighborhood, keyPts);			// Masks are too slow
		}

		// Map keypoints back to the original image
		for (int p = 0; p < keyPts.size(); p++)
		{
			keyPts[p].pt.x = round(keyPts[p].pt.x + (round((o / gridYsize)) * size_factor) - rectHalfSize) + OffsetTop.x;
			keyPts[p].pt.y = round(keyPts[p].pt.y + ((o % gridYsize)) * size_factor - rectHalfSize) + OffsetTop.y;
		}
		
		// Iterate all triangles and find the one whose centroid is the closest to the grid intersection
		for (int i = 0; i < keyPts.size() - 2; i++)
		{
			for (int j = i + 1; j < keyPts.size() - 1; j++)
			{
				int m = keyPts[i].pt.x + keyPts[j].pt.x;
				int n = keyPts[i].pt.y + keyPts[j].pt.y;
				for (int k = j + 1; k < keyPts.size(); k++)
				{

					int x = round((m + keyPts[k].pt.x) / 3);
					int y = round((n + keyPts[k].pt.y) / 3);

					Point2i centroid(x, y);

					if (euclideanNorm(gridPt->Q,centroid) < minDist)
					{
						gridPt->triangle[0] = keyPts[i];
						gridPt->triangle[1] = keyPts[j];
						gridPt->triangle[2] = keyPts[k];
						gridPt->kx = gridPt->Q.x - centroid.x;
						gridPt->ky = gridPt->Q.y - centroid.y;
						minDist = euclideanNorm(gridPt->Q, centroid);
					}
				}
			}
		}

		keyPts.clear();
	}
}

// Draw grid
void gridDraw(const Mat& inImg, Mat& outImg, vector<GridPoint> Grid, const int& gridXsize, const int& gridYsize)
{
	inImg.copyTo(outImg);
	for (int i = 0; i < gridXsize - 1; i++)
	{
		for (int j = 0; j < gridYsize - 1; j++)
		{
			line(outImg, Grid[i*gridYsize + j].Q, Grid[(i + 1)*gridYsize + j].Q, Scalar(0, 0, 0), 2);
			line(outImg, Grid[i*gridYsize + j].Q, Grid[i*gridYsize + j + 1].Q, Scalar(0, 0, 0), 2);
		}
		line(outImg, Grid[i*gridYsize + gridYsize - 1].Q, Grid[(i + 1)*gridYsize + gridYsize - 1].Q, Scalar(0, 0, 0), 2);
	}
	for (int j = 0; j < gridYsize - 1; j++)
	{
		line(outImg, Grid[(gridXsize - 1)*gridYsize + j].Q, Grid[(gridXsize - 1)*gridYsize + j + 1].Q, Scalar(0, 0, 0), 2);
	}
}

void findNewGrid(Mat image, vector<GridPoint> oldGrid, vector<GridPoint>& newGrid, const int& gridXsize, const int& gridYsize, const int& size_factor)
{
	// Detect blobs
	SimpleBlobDetector::Params blobParams;

	blobParams.filterByArea = true;
	blobParams.minArea = 10;

	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(blobParams);

	// Draw all keypoints - for debug purposes 
	//vector<KeyPoint> keypoints;
	//Mat imageKeypoints;
	//detector->detect(image, keypoints);
	//drawKeypoints(image, keypoints, imageKeypoints, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//imshow("all keypoints", imageKeypoints);
	//waitKey(0);

	// For every intersection in grid find the new location
	newGrid = oldGrid;
	for (int o = 0; o < oldGrid.size(); o++)
	{
		GridPoint gridPt = oldGrid[o];
		
		// Find new locations of every triangle vertex
		for (int k = 0; k < gridPt.triangle.size(); k++)
		{
			vector<KeyPoint> closestKeyPt;
			int rectHalfSize = 0;
			Mat mask = Mat::zeros(image.size(), CV_8U);
			Mat neighborhood;
			// Expand neighborhood until there is a keypoint present
			while (closestKeyPt.empty())
			{
				rectHalfSize += 1;
				if (rectHalfSize > 30)			// if the keypoint moves by more than 30px then there is a problem
				{
					std::cout << "Can't find any keypoints!" << endl;
					return;
				}
				Rect r(gridPt.triangle[k].pt.x - rectHalfSize, gridPt.triangle[k].pt.y - rectHalfSize, 2 * rectHalfSize +1, 2 * rectHalfSize+1);
				neighborhood = image(r);							// Cut the neighborhood from input image
				detector->detect(neighborhood, closestKeyPt);		// Masks are too slow
				
			}
			newGrid[o].triangle[k].pt.x += closestKeyPt[0].pt.x - rectHalfSize;				// Move the new vertex
			newGrid[o].triangle[k].pt.y += closestKeyPt[0].pt.y - rectHalfSize;				// Move the new vertex
		}
		int Qx = round((newGrid[o].triangle[0].pt.x + newGrid[o].triangle[1].pt.x + newGrid[o].triangle[2].pt.x) / 3) + newGrid[o].kx;		//Calculate new intersection location
		int Qy = round((newGrid[o].triangle[0].pt.y + newGrid[o].triangle[1].pt.y + newGrid[o].triangle[2].pt.y) / 3) + newGrid[o].ky;		//Calculate new intersection location

		newGrid[o].Q = Point2i(Qx, Qy);
	}

}