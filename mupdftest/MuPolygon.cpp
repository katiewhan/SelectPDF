#include "MuPolygon.h"
#include <algorithm>
#include <math.h>

MuPolygon::MuPolygon(std::vector<mu_point> points)
{
	m_points = SampleStroke(points);
	int n = points.size();
}

MuPolygon::~MuPolygon()
{
}

bool MuPolygon::HasPoint(mu_point p) {
	if (m_points.size() < 3) return false;

	mu_point extreme;
	extreme.x = 1000000;
	extreme.y = p.y;

	int count = 0;
	int i = 0;

	do
	{
		int next = (i + 1) % m_points.size();

		// Check if the line segment from 'p' to 'extreme' intersects
		// with the line segment from 'polygon[i]' to 'polygon[next]'
		if (LinesIntersect(m_points[i], m_points[next], p, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next',
			// then check if it lies on segment. If it lies, return true,
			// otherwise false
			if (Orientation(m_points[i], p, m_points[next]) == 0)
				return OnSegment(m_points[i], p, m_points[next]);

			count++;
		}
		i = next;
	} while (i != 0);

	/*for (int i = 0; i < m_lines.size(); i++) {
		mu_line l1;
		l1.start = p;
		l1.end = extreme;
		if (LinesIntersect(m_lines[i], l1)) {
			if (Orientation(m_lines[i].start, p, m_lines[i].end) == 0) {
				return LineHasPoint(m_lines[i], p);
			}
			count++;
		}
	}*/

	return (count % 2 == 1);
}

bool MuPolygon::LinesIntersect(mu_point p1, mu_point q1, mu_point p2, mu_point q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = Orientation(p1, q1, p2);
	int o2 = Orientation(p1, q1, q2);
	int o3 = Orientation(p2, q2, p1);
	int o4 = Orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && OnSegment(p1, p2, q1)) return true;

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && OnSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && OnSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && OnSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}

bool MuPolygon::OnSegment(mu_point p, mu_point q, mu_point r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;
	return false;
}

int MuPolygon::Orientation(mu_point p, mu_point q, mu_point r) {
	int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;

	return (val > 0) ? 1 : 2;
}

bool MuPolygon::TooClose(mu_point p1, mu_point p2)
{
	return (abs(p1.x - p2.x) < 10 && abs(p1.y - p2.y) < 10);
}

double MuPolygon::Degree(mu_point p1, mu_point p2)
{
	return atan2(p2.y - p1.y, p2.x - p1.x) * 180 / (atan(1) * 4);
}

std::vector<mu_point> MuPolygon::SampleStroke(std::vector<mu_point> points)
{
	int len = points.size();
	int ypre;
	int xpre;
	int predg = -20;

	mu_point prept = points[0];
	int strokeHash = {};

	std::vector<mu_point> sampledStrokes;

	sampledStrokes.push_back(prept);

	for (int i = 1; i < len; i++) {

		mu_point pt = points[i];

		if ((abs(predg - Degree(pt, prept)) < 10 || TooClose(pt, prept)) && i < len - 1) {
			continue;
		}

		predg = Degree(pt, prept);
		sampledStrokes.push_back(points[i]);

		prept = pt;
	}

	return sampledStrokes;
}