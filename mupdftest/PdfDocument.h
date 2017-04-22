#pragma once

extern "C" {
	#include "mupdf\fitz.h"
	#include "mupdf\pdf.h"
}

#include "MuPdfData.h"
#include "Selection\Selection.h"
#include "SelectionMatcher.h"
#include <map>
#include <memory>

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

		int AddSelection(mu_point* pointList, int size);
		int GetHighlights(int id, mu_rect* rectList[], int max);
		int GetNumSelections();
		char* GetSelectionContent(int id); 
		int GetSelectionContents(int id, mu_selection* contentList[], int max);

	private:
		fz_context* m_context;
		fz_document* m_document;
		fz_outline* m_outline;
		fz_page* m_current_page;
		fz_pixmap* m_pixmap;
		fz_rect m_rect;
		fz_buffer* m_buffer;
		int m_current_page_num;
		fz_matrix m_ctm;

		std::unique_ptr<SelectionMatcher> m_sel_matcher;
		std::map<int, std::unique_ptr<Selection>> m_selection_map;
		int m_current_sel_id;
		
	};
}