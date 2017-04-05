#pragma once
#include "Selection.h"
class MarqueeSelection :
	public Selection
{
public:
	MarqueeSelection(int x0, int y0, int x1, int y1, int page);
	~MarqueeSelection();

	void Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) override;

private:
	int m_startx;
	int m_starty;
	int m_endx;
	int m_endy;
};

