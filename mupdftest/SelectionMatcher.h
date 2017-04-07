#pragma once
#include "Selection\Selection.h"
#include "MuPdfData.h"
#include <string>
#include <memory>

const int STROKE_BUFFER = 40;

class SelectionMatcher
{
public:
	SelectionMatcher();
	~SelectionMatcher();
	std::unique_ptr<Selection> GetSelection(mu_point* pointList, int size, int page);

private:
	int GetStrokeMetrics(mu_point* pointList, int size);
};
