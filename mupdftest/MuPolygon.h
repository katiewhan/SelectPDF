#pragma once
#include "MuPdfData.h"
#include <vector>

#define INF 100000

class MuPolygon
{
public:
	MuPolygon(std::vector<mu_point> points);
	~MuPolygon();

	bool HasPoint(mu_point p);

private:
	static bool LinesIntersect(mu_point p1, mu_point q1, mu_point p2, mu_point q2);
	static bool OnSegment(mu_point p, mu_point q, mu_point r);
	static int Orientation(mu_point p, mu_point q, mu_point r);

	bool TooClose(mu_point p1, mu_point p2);
	double Degree(mu_point p1, mu_point p2);
	std::vector<mu_point> SampleStroke(std::vector<mu_point> points);

	std::vector<mu_point> m_points;
};

