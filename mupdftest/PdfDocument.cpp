
#include "PdfDocument.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm> 

#include "Selection\NullSelection.h"
#include "MuPdfImageUtil.h"

namespace MuPdfApi
{
	PdfDocument::PdfDocument() {
		m_buffer = nullptr;
		m_pixmap = nullptr;
		m_current_page = nullptr;
		m_outline = nullptr;
		m_document = nullptr;
		m_context = nullptr;
		m_current_page_num = -1;
		m_current_sel_id = -1;

		m_sel_matcher = std::make_unique<SelectionMatcher>();

		fz_var(m_buffer);
		fz_var(m_pixmap);
		fz_var(m_current_page);
		fz_var(m_outline);
		fz_var(m_document);
		fz_var(m_context);
	}

	void PdfDocument::Dispose() {

		fz_empty_store(m_context);

		if (m_buffer != nullptr)
			fz_drop_buffer(m_context, m_buffer);
		if (m_pixmap != nullptr)
			fz_drop_pixmap(m_context, m_pixmap);
		if (m_current_page != nullptr)
			fz_drop_page(m_context, m_current_page);
		if (m_outline != nullptr)
			fz_drop_outline(m_context, m_outline);
		if (m_document != nullptr)
			fz_drop_document(m_context, m_document);
		if (m_context != nullptr)
			fz_drop_context(m_context);

		m_current_page_num = -1;
	}

	void PdfDocument::Open(unsigned char* buffer, int bufferLen)
	{
		m_context = fz_new_context(nullptr, nullptr, FZ_STORE_DEFAULT);
		fz_stream *stream = nullptr;
		stream = fz_open_memory(m_context, buffer, bufferLen);
		fz_register_document_handlers(m_context);
		m_document = fz_open_document_with_stream(m_context, "application/pdf", stream);
		m_outline = fz_load_outline(m_context, m_document);
		m_current_page = fz_load_page(m_context, m_document, 0);
		m_current_page_num = 0;
		fz_bound_page(m_context, m_current_page, &m_rect);

	}

	bool PdfDocument::GotoPage(int page) {
		if (page < 0 || page > fz_count_pages(m_context, m_document) - 1)
			return false;

		fz_try(m_context) {
			if (m_current_page != nullptr)
				fz_drop_page(m_context, m_current_page);

			m_current_page = fz_load_page(m_context, m_document, page);
			m_current_page_num = page;
			fz_bound_page(m_context, m_current_page, &m_rect);
		}
		fz_catch(m_context)
		{
		}

		return true;
	}

	bool PdfDocument::GoToNextPage() {
		bool res = GotoPage(m_current_page_num + 1);
		return res;
	}

	bool PdfDocument::GoToPrevPage() {
		bool res = GotoPage(m_current_page_num - 1);
		return res;
	}

	int PdfDocument::GetCurrentPageNum() {
		return m_current_page_num;
	}

	int PdfDocument::GetPageWidth() {
		return m_rect.x1 - m_rect.x0;
	}

	int PdfDocument::GetPageHeight() {
		return m_rect.y1 - m_rect.y0;
	}

	int PdfDocument::GetNumComponents() {
		return fz_pixmap_components(m_context, m_pixmap);
	}

	int PdfDocument::GetNumPages() {
		return fz_count_pages(m_context, m_document);
	}

	int PdfDocument::GetTextBytes(unsigned char* result) {
		auto sheet = fz_new_stext_sheet(m_context);
		auto text_page = fz_new_stext_page_from_page(m_context, m_current_page, sheet, 0);
		auto buf = fz_new_buffer_from_stext_page(m_context, text_page, &fz_infinite_rect, 1);
		
		std::string p(buf->data, buf->data + buf->len);
		memcpy(result, buf->data, buf->len);
		int len = buf->len;
		fz_drop_stext_sheet(m_context, sheet);
		fz_drop_stext_page(m_context, text_page);
		fz_drop_buffer(m_context, buf);

		return len;
	}

	int PdfDocument::Render(int width, int height) {
		
		fz_device* dev;
		fz_device* idev;
		fz_colorspace *colorspace;
		fz_display_list* pagelist;
		fz_var(dev);
		fz_var(idev);
		fz_var(colorspace);
		fz_var(pagelist);

		fz_try(m_context) {
			fz_drop_buffer(m_context, m_buffer);
			if (m_pixmap != nullptr) {
				fz_drop_pixmap(m_context, m_pixmap);
			}

			/*fz_rect pagebounds;
			fz_bound_page(m_context, m_current_page, &pagebounds);*/
			float xscale = (float)width / (float)(m_rect.x1 - m_rect.x0);
			float yscale = (float)height / (float)(m_rect.y1 - m_rect.y0);
			m_ctm = fz_identity;
			fz_scale(&m_ctm, xscale, yscale);

			fz_irect ibounds;
			fz_round_rect(&ibounds, fz_transform_rect(&m_rect, &m_ctm));
			fz_rect bounds;
			fz_rect_from_irect(&bounds, &ibounds);

			colorspace = fz_device_rgb(m_context);

			pagelist = fz_new_display_list(m_context, nullptr);
			dev = fz_new_list_device(m_context, pagelist);
			//fz_enable_device_hints(m_context, dev, FZ_NO_CACHE);
			fz_run_page(m_context, m_current_page, dev, &fz_identity, NULL);

			m_pixmap = fz_new_pixmap_with_bbox(m_context, colorspace, &ibounds, 1);
			fz_clear_pixmap_with_value(m_context, m_pixmap, 0xff);
			idev = fz_new_draw_device(m_context, nullptr, m_pixmap);

			fz_run_display_list(m_context, pagelist, idev, &m_ctm, &bounds, nullptr);

			// test
			m_buffer = MuPdfImageUtil::png_from_pixmap(m_context, m_pixmap, 0);
			fz_close_device(m_context, dev);
		}
		fz_always(m_context)
		{
			fz_drop_device(m_context, dev);
			fz_drop_device(m_context, idev);
			//fz_drop_pixmap(m_context, m_pixmap);
			fz_drop_colorspace(m_context, colorspace);
			//fz_drop_page(m_context, m_current_page);
			fz_drop_display_list(m_context, pagelist);
			//fz_empty_store(m_context);
		}
		fz_catch(m_context)
		{
			fz_rethrow(m_context);
		}


		return m_buffer->len;
	}

	unsigned char* PdfDocument::GetBuffer() {
		return m_buffer->data;
	}

	int PdfDocument::AddSelection(mu_point* pointList, int size) {
		if (size <= 0) {
			return false;
		}
		//fz_print_stext_page_html(fz_context *ctx, fz_output *out, fz_stext_page *page)
		
		fz_device* dev;
		fz_var(dev);
		fz_stext_sheet* sheet = fz_new_stext_sheet(m_context);
		fz_stext_page* page = fz_new_stext_page_from_page(m_context, m_current_page, sheet, 0);
		dev = fz_new_stext_device(m_context, sheet, page, 0);

		fz_disable_device_hints(m_context, dev, FZ_IGNORE_IMAGE);
		fz_matrix ctm = fz_identity;

		fz_run_page(m_context, m_current_page, dev, &m_ctm, NULL);

		m_selection_map[++m_current_sel_id] = m_sel_matcher->GetSelection(pointList, size, m_current_page_num);
		if (dynamic_cast<NullSelection*>(m_selection_map[m_current_sel_id].get()) != nullptr) {
			m_selection_map.erase(m_current_sel_id);
			return -1;
		}
		else {
			m_selection_map[m_current_sel_id].get()->Select(m_context, page, m_ctm);
			return m_current_sel_id;
		}
		
		//fz_try(m_context) {
		/*auto sheet = fz_new_stext_sheet(m_context);
		auto x = nelem(hits);
		auto pg = fz_new_stext_page_from_page(m_context, m_current_page, sheet, 0);
		n = fz_highlight_selection(m_context, pg, rect2, hits, 1);*/

		
		/*fz_buffer* buf = fz_new_buffer(m_context, 2048);
		fz_print_stext_page_html(m_context, fz_new_output_with_buffer(m_context, buf), page);
		unsigned char * result = buf->data;*/

		/*} 
		fz_catch(m_context) {
			fz_rethrow(m_context);
		}*/

		//m_selection_list.push_back(LineSelection())
		
		/*for (int i = 0; i < n; i++) {
			rectangles[i * 4] = hits[i].x0;
			rectangles[i * 4 + 1] = hits[i].y0;
			rectangles[i * 4 + 2] = hits[i].x1;
			rectangles[i * 4 + 3] = hits[i].y1;
		}

		return rectangles;*/

		/*char* word = new char[100];
		snprintf(word, sizeof(word), "end %d", pointList[size - 1].y);
		return word;*/

	}

	int PdfDocument::GetHighlights(int id, mu_rect* rectList[], int max) {
		int num_rects = m_selection_map[id].get()->GetNumRects();
		num_rects = std::min(num_rects, max);

		std::vector<fz_rect> rects = m_selection_map[id].get()->GetRects();

		for (int i = 0; i < num_rects; i++) {
			(*rectList)[i].x0 = rects[i].x0;
			(*rectList)[i].y0 = rects[i].y0;
			(*rectList)[i].x1 = rects[i].x1;
			(*rectList)[i].y1 = rects[i].y1;
 		}

		return num_rects;
	}

	int PdfDocument::GetSelectionContents(int id, mu_selection* contentList[], int max) {
		int num_contents = m_selection_map[id].get()->GetNumSels();
		num_contents = std::min(num_contents, max);

		std::vector<mu_selection> contents = m_selection_map[id].get()->GetContents();

		for (int i = 0; i < num_contents; i++) {
			(*contentList)[i].type = contents[i].type;
			(*contentList)[i].num_bytes = contents[i].num_bytes;
			(*contentList)[i].content = contents[i].content;
		}

		return num_contents;
	}

	int PdfDocument::GetNumSelections() {
		return m_selection_map.size();
	}

	char* PdfDocument::GetSelectionContent(int id) {
		return m_selection_map[id].get()->GetText();
	}
}