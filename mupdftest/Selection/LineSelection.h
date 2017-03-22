#pragma once
#include "Selection.h"
class LineSelection :
	public Selection
{
public:
	LineSelection(int x0, int x1, int y);
	~LineSelection();

	void Select(fz_stext_page* page);

private:
	
};

