#ifndef GRID_POINT
#define GRID_POINT

#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

class GridPoint
{
public:
	Point2i Q;								// location of grid intersection
	vector<KeyPoint> triangle;				// three points of triangle in whose 'centroid' the intersection lies
	int kx;									// X axis offset from triangle centroid
	int ky;									// Y axis offset from triangle centroid


	GridPoint(Point2i O = Point2i(0, 0), KeyPoint A = KeyPoint(), KeyPoint B = KeyPoint(), KeyPoint C = KeyPoint(), int Kx = 1, int Ky = 1);
	GridPoint(const GridPoint&);
	~GridPoint();
};
#endif // !GRID_POINT

