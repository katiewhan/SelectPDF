#pragma once
#include "Selection.h"
class LineSelection :
	public Selection
{
public:
	LineSelection(int x0, int x1, int y, int page);
	~LineSelection();

	void Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) override;

private:
	int m_startx;
	int m_endx;
	int m_avgy;
};

