#ifndef GRID
#define GRID

#include "GridPoint.h"

// This function calculates coordinates of grid intersections, finds best fitting triangles to describe those intersections and calculates kx and ky coeffs
void gridCreate(const Mat& image, vector<GridPoint>& Grid, const int& gridXsize, const int& gridYsize, const int& size_factor, Point2i OffsetTop = Point2i(0, 0));

// Takes a video frame as input and outputs a frame with a grid draw on it
void gridDraw(const Mat& inImg, Mat& outImg, vector<GridPoint> Grid, const int& gridXsize, const int& gridYsize);

// Takes new frame of image as input and old Grid and returns new Grid
void findNewGrid(Mat image, vector<GridPoint> oldGrid, vector<GridPoint>& newGrid, const int& gridXsize, const int& gridYsize, const int& size_factor);
#endif
