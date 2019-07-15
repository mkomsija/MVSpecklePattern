#include "GridPoint.h"	

GridPoint::GridPoint(Point2i O, KeyPoint A, KeyPoint B, KeyPoint C, int Kx, int Ky)
{
	Q = O;
	triangle.push_back(A);
	triangle.push_back(B);
	triangle.push_back(C);
	kx = Kx;
	ky = ky;
}

GridPoint::GridPoint(const GridPoint& gp)
{
	this->Q = gp.Q;
	this->triangle = gp.triangle;
	this->kx = gp.kx;
	this->ky = gp.ky;
}


GridPoint::~GridPoint()
{
	// all other private variables aren't dynamically allocated
	triangle.clear();
}