#pragma once
#include "Selection.h"
#include "MuPolygon.h"

class LassoSelection :
	public Selection
{
public:
	LassoSelection(std::vector<mu_point> points, int page);
	~LassoSelection();

	void Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) override;

private:
	MuPolygon m_polygon;

};

