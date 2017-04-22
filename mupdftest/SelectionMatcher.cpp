#include "SelectionMatcher.h"
#include "Selection\LineSelection.h"
#include "Selection\BracketSelection.h"
#include "Selection\NullSelection.h"
#include "Selection\MarqueeSelection.h"
#include "Selection\LassoSelection.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <cmath>

SelectionMatcher::SelectionMatcher()
{
}

SelectionMatcher::~SelectionMatcher()
{
}

std::unique_ptr<Selection> SelectionMatcher::GetSelection(mu_point* pointList, int size, int page) {
	int y_delta = std::abs(pointList[size - 1].y - pointList[0].y);
	int x_delta = std::abs(pointList[size - 1].x - pointList[0].x);

	if (GetStrokeMetrics(pointList, size) > (STROKE_BUFFER * 2)) {
		std::vector<mu_point> points(pointList, pointList + size);
		return std::make_unique<LassoSelection>(points, page);
	}
	else if (y_delta < (STROKE_BUFFER / 4) && x_delta < (STROKE_BUFFER / 4)) {
		return std::make_unique<NullSelection>();
	}
	else if (y_delta < STROKE_BUFFER) { // end y - start y
		int avg_y = 0;
		for (int i = 0; i < size; i++) {
			avg_y += pointList[i].y;
		}
		avg_y /= size;

		return std::make_unique<LineSelection>(pointList[0].x, pointList[size - 1].x, avg_y, page);
	}
	else if (x_delta < STROKE_BUFFER) { // end x - start x
		int avg_x = 0;
		for (int i = 0; i < size; i++) {
			avg_x += pointList[i].x;
		}
		avg_x /= size;

		return std::make_unique<BracketSelection>(avg_x, pointList[0].y, pointList[size - 1].y, page);
	}
	else if (y_delta > STROKE_BUFFER && x_delta > STROKE_BUFFER) {
		return std::make_unique<MarqueeSelection>(pointList[0].x, pointList[0].y, pointList[size - 1].x, pointList[size - 1].y, page);
	}
	else {
		return std::make_unique<NullSelection>();
	}
}

int SelectionMatcher::GetStrokeMetrics(mu_point * pointList, int size)
{
	mu_point start = pointList[0];
	mu_point end = pointList[size - 1];
	int v_x = end.x - start.x;
	int v_y = end.y - start.y;

	double v_len = sqrt((v_x * v_x) + (v_y * v_y));
	double x_norm = v_x / v_len;
	double y_norm = v_y / v_len;

	std::vector<double> errors;

	for (int i = 0; i < size; i++) {
		int a_x = pointList[i].x - start.x;
		int a_y = pointList[i].y - start.y;

		double dot = (a_x * x_norm) + (a_y * y_norm);
		double b_x = x_norm * dot;
		double b_y = y_norm * dot;

		double c_x = a_x - b_x;
		double c_y = a_y - b_y;
		double c_len = sqrt((c_x * c_x) + (c_y * c_y));

		errors.push_back(abs(c_len));
	}

	double median;

	std::sort(errors.begin(), errors.end());

	if (size % 2 == 0)
	{
		median = (errors[size / 2 - 1] + errors[size / 2]) / 2;
	}
	else
	{
		median = errors[size / 2];
	}

	return median;
}
