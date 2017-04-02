#pragma once
#include "Selection.h"
class BracketSelection :
	public Selection
{
public:
	BracketSelection(int x, int y0, int y1, int page);
	~BracketSelection();

	void Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) override;

private:
	int m_avgx;
	int m_starty;
	int m_endy;
};

