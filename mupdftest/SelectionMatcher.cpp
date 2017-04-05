#include "SelectionMatcher.h"
#include "Selection\LineSelection.h"
#include "Selection\BracketSelection.h"
#include "Selection\NullSelection.h"
#include "Selection\MarqueeSelection.h"
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
	int y_delta = std::abs(pointList[size - 1].y - pointList[0].y);
	int x_delta = std::abs(pointList[size - 1].x - pointList[0].x);

	if (y_delta < (STROKE_BUFFER / 4) && x_delta < (STROKE_BUFFER / 4)) {
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

