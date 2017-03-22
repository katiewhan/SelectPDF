#include "SelectionMatcher.h"
#include <iostream>
#include <fstream>
#include <sstream>

SelectionMatcher::SelectionMatcher()
{
}

char* SelectionMatcher::GetSelection(double* pointList, int size) {
	if (std::abs(pointList[size-1] - pointList[1]) < 20) { // end y - start y
		return "line";
	}
}

