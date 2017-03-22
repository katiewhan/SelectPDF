#pragma once

extern "C" {
	#include "mupdf\fitz.h"
	#include "mupdf\pdf.h"
}

namespace MuPdfApi
{
	// This class is exported from the MathLibrary.dll
	class PdfDocument
	{
	public:
		PdfDocument();
		void Dispose();
		// Returns a + b
		void Open(unsigned char* data, int bufferLen);
		int Render(int width, int height);
		unsigned char* GetBuffer();
		int GetTextBytes(unsigned char*);
		int GetPageWidth();
		int GetPageHeight();
		int GetNumComponents();
		int GetNumPages();
		bool GotoPage(int page);
		fz_buffer * png_from_pixmap(fz_context *ctx, fz_pixmap *pix, int drop);

		// Returns a * b
		//static MUPDFAPI_API void CloseMultiply(double b);

	private:
		fz_context* m_context;
		fz_document* m_document;
		fz_outline* m_outline;
		fz_page* m_current_page;
		fz_pixmap* m_pixmap;
		fz_rect m_rect;
		fz_buffer* m_buffer;
		
	};
}