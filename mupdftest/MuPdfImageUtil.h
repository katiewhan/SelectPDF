#pragma once
extern "C" {
#include "mupdf\fitz.h"
#include "mupdf\pdf.h"
}

class MuPdfImageUtil
{
public:
	MuPdfImageUtil();
	~MuPdfImageUtil();

	static fz_buffer * png_from_pixmap(fz_context *ctx, fz_pixmap *pix, int drop);
};

