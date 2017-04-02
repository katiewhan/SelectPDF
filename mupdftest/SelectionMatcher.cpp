#include "SelectionMatcher.h"
#include "Selection\LineSelection.h"
#include "Selection\BracketSelection.h"
#include <iostream>
#include <fstream>
#include <sstream>

SelectionMatcher::SelectionMatcher()
{
}

SelectionMatcher::~SelectionMatcher()
{
}

std::unique_ptr<Selection> SelectionMatcher::GetSelection(mu_point* pointList, int size, int page) {
	if (std::abs(pointList[size - 1].y - pointList[0].y) < 20) { // end y - start y
		int avg_y = 0;
		for (int i = 0; i < size; i++) {
			avg_y += pointList[i].y;
		}
		avg_y /= size;

		return std::make_unique<LineSelection>(pointList[0].x, pointList[size - 1].x, avg_y, page);
	}
	else if (std::abs(pointList[size - 1].x - pointList[0].x) < 20) { // end x - start x
		int avg_x = 0;
		for (int i = 0; i < size; i++) {
			avg_x += pointList[i].x;
		}
		avg_x /= size;

		return std::make_unique<BracketSelection>(avg_x, pointList[0].y, pointList[size - 1].y, page);
	}
}

