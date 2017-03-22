#pragma once

#include <string>

ref class SelectionMatcher sealed
{
internal:
	SelectionMatcher();
	char* GetSelection(double* pointList, int size);
};
