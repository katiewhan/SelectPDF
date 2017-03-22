#pragma once

extern "C" {
	#include "mupdf\fitz.h"
	#include "mupdf\pdf.h"
}

#include "SelectionMatcher.h"
#include "Selection\Selection.h"
#include <vector>

struct mu_point {
	int x;
	int y;
};

namespace MuPdfApi
{

	class PdfDocument
	{
	public:
		PdfDocument();
		void Dispose();
		void Open(unsigned char* data, int bufferLen);
		int Render(int width, int height);
		unsigned char* GetBuffer();
		int GetTextBytes(unsigned char*);
		int GetPageWidth();
		int GetPageHeight();
		int GetNumComponents();
		int GetNumPages();
		bool GotoPage(int page);
		int GetCurrentPageNum();
		bool GoToNextPage();
		bool GoToPrevPage();
		fz_buffer * png_from_pixmap(fz_context *ctx, fz_pixmap *pix, int drop);

		bool AddSelection(mu_point* pointList, int size);
		int GetHighlights(fz_rect* rectList[], int max);
		//int GetHTML();

	private:
		fz_context* m_context;
		fz_document* m_document;
		fz_outline* m_outline;
		fz_page* m_current_page;
		fz_pixmap* m_pixmap;
		fz_rect m_rect;
		fz_buffer* m_buffer;
		int m_current_page_num;

		SelectionMatcher^ m_selection;
		//std::vector<Selection> m_selection_list;

		// temp testing
		fz_rect* m_rects;
		int m_num_rects;
		
	};
}