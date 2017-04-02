#pragma once
#include "Selection\Selection.h"
#include "MuPdfData.h"
#include <string>
#include <memory>

class SelectionMatcher
{
public:
	SelectionMatcher();
	~SelectionMatcher();
	std::unique_ptr<Selection> GetSelection(mu_point* pointList, int size, int page);
};
