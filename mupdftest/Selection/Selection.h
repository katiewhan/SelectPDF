#pragma once
extern "C" {
#include "mupdf\fitz.h"
#include "mupdf\pdf.h"
}

#include <string>
#include <vector>

const int max_text = 1024;

class Selection
{
public:
	Selection(int page);
	std::vector<fz_rect> GetRects();
	int GetNumRects();
	char* GetText();

	virtual void Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) = 0;

protected:
	int m_page_num;
	char m_text[max_text];
	std::vector<fz_rect> m_rects;
};

