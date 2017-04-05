#pragma once
#include "Selection.h" 
class NullSelection :
	public Selection
{
public:
	NullSelection();
	~NullSelection();

	void Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) override {}
};

