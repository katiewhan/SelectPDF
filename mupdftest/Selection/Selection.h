#pragma once
extern "C" {
#include "mupdf\fitz.h"
#include "mupdf\pdf.h"
}

#include <string>
#include <vector>
#include "MuPdfImageUtil.h"
#include "MuPdfData.h"

const int MAX_TEXT = 1024;
const int SELECTION_BUFFER = 40;


class Selection
{
public:
	Selection(int page);
	std::vector<fz_rect> GetRects();
	int GetNumRects();
	int GetNumSels();
	char* GetText();
	std::vector<mu_selection> GetContents();

	virtual void Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) = 0;

protected:
	int m_page_num;
	char m_text[MAX_TEXT];
	std::vector<fz_rect> m_rects;
	std::vector<mu_selection> m_contents;
};

