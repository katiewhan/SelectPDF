#include "LassoSelection.h"

LassoSelection::LassoSelection(std::vector<mu_point> points, int page) :
	Selection(page),
	m_polygon(points)
{
}

LassoSelection::~LassoSelection()
{
}

void LassoSelection::Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) {
	char curr_text[MAX_TEXT];
	int curr_i = 0;
	bool need_newline = false;

	for (int block_num = 0; block_num < page->len; block_num++)
	{
		if (page->blocks[block_num].type == FZ_PAGE_BLOCK_TEXT) {
			// select text one char at a time
			fz_stext_line *line;
			fz_stext_block *block;
			fz_stext_span *span;

			block = page->blocks[block_num].u.text;

			for (line = block->lines; line < block->lines + block->len; line++)
			{
				bool saw_text = false;

				for (span = line->first_span; span; span = span->next)
				{
					for (int i = 0; i < span->len; i++)
					{
						fz_rect hitbox;

						fz_stext_char_bbox(ctx, &hitbox, span, i);
						fz_transform_rect(&hitbox, &ctm);
						int c = span->text[i].c;
						if (c < 32)
							c = '?';
						char curr = c; // for debugging

						mu_point p0 = { hitbox.x0, hitbox.y0 };
						mu_point p1 = { hitbox.x0, hitbox.y1 };
						mu_point p2 = { hitbox.x1, hitbox.y0 };
						mu_point p3 = { hitbox.x1, hitbox.y1 };
						if (m_polygon.HasPoint(p0) || m_polygon.HasPoint(p1) || m_polygon.HasPoint(p2) || m_polygon.HasPoint(p3))
						{
							saw_text = true;
							m_rects.push_back(hitbox); // add rect to highlight area

							if (need_newline)
							{
								if (curr_i < MAX_TEXT - 1) curr_text[curr_i++] = '\r\n';
								need_newline = false;
							}

							if (curr_i < MAX_TEXT - 1) curr_text[curr_i++] = c;
						}
					}
				}

				if (saw_text) need_newline = true;
			}
		}
		else {
			// if there is text being selected, end text and add to selection content list
			if (curr_i > 0) {
				if (curr_i < MAX_TEXT - 1) curr_text[curr_i++] = 0;

				mu_selection sel;
				sel.type = TEXT_CONTENT;
				sel.content.text = new char[curr_i];
				strncpy(sel.content.text, curr_text, curr_i);
				m_contents.push_back(sel);

				curr_i = 0;
			}

			// select image if within selection region
			fz_image_block* block = page->blocks[block_num].u.image;
			fz_rect hitbox = block->bbox;

			mu_point p0 = { hitbox.x0, hitbox.y0 };
			mu_point p1 = { hitbox.x0, hitbox.y1 };
			mu_point p2 = { hitbox.x1, hitbox.y0 };
			mu_point p3 = { hitbox.x1, hitbox.y1 };
			if (m_polygon.HasPoint(p0) || m_polygon.HasPoint(p1) || m_polygon.HasPoint(p2) || m_polygon.HasPoint(p3)) {
				m_rects.push_back(hitbox); // add rect to highlight area

				fz_pixmap* pix;
				fz_buffer* buf;
				int w = 0;
				int h = 0;

				fz_var(pix);
				fz_var(buf);

				fz_image* image = block->image;

				// add image to selection content list
				fz_try(ctx) {
					pix = fz_get_pixmap_from_image(ctx, image, NULL, NULL, &w, &h);
					buf = MuPdfImageUtil::png_from_pixmap(ctx, pix, 0);
					mu_selection sel;
					sel.type = IMAGE_CONTENT;
					sel.num_bytes = buf->len;
					sel.content.image = buf->data;
					m_contents.push_back(sel);
				}
				fz_always(ctx) {
					fz_drop_pixmap(ctx, pix);
				}
				fz_catch(ctx)
				{
					fz_drop_buffer(ctx, buf);
					fz_rethrow(ctx);
				}
			}
		}
	}

	// done traversing the page; add any remaining selected text to selection content list
	if (curr_i > 0) {
		if (curr_i < MAX_TEXT - 1) curr_text[curr_i++] = 0;

		mu_selection sel;
		sel.type = TEXT_CONTENT;
		sel.content.text = new char[curr_i];
		strncpy(sel.content.text, curr_text, curr_i);
		m_contents.push_back(sel);

		curr_i = 0;
	}
}