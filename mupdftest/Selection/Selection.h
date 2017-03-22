#pragma once
extern "C" {
#include "mupdf\fitz.h"
#include "mupdf\pdf.h"
}

#include <string>

class Selection
{
public:
	Selection();
	fz_rect* GetRects();
	int GetNumRects();

	virtual void Select(fz_stext_page* page) {};

private:

	std::string m_content;
	fz_rect* m_rects;
	int m_num_rects;
};

