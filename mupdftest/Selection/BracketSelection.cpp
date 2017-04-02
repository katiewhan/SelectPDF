#include "BracketSelection.h"

BracketSelection::BracketSelection(int x, int y0, int y1, int page) :
	Selection(page)
{
	m_avgx = x;
	m_starty = y0;
	m_endy = y1;
}


BracketSelection::~BracketSelection()
{
}

void BracketSelection::Select(fz_context* ctx, fz_stext_page* page, fz_matrix ctm) {
	int curr_text = 0;
	bool need_newline = false;

	for (int block_num = 0; block_num < page->len; block_num++)
	{
		fz_stext_line *line;
		fz_stext_block *block;
		fz_stext_span *span;
		fz_rect hitbox;

		if (page->blocks[block_num].type != FZ_PAGE_BLOCK_TEXT)
			continue;
		block = page->blocks[block_num].u.text;

		for (line = block->lines; line < block->lines + block->len; line++) {
			bool saw_text = false;

			fz_stext_char_bbox(ctx, &hitbox, line->first_span, 0);
			fz_transform_rect(&hitbox, &ctm);

			if (hitbox.y1 >= m_starty && hitbox.y0 <= m_endy) {
				bool selecting = false;

				for (span = line->first_span; span; span = span->next)
				{
					for (int i = 0; i < span->len; i++)
					{
						fz_stext_char_bbox(ctx, &hitbox, span, i);
						fz_transform_rect(&hitbox, &ctm);
						int c = span->text[i].c;
						if (c < 32)
							c = '?';
						char curr = c;

						if (hitbox.x0 <= m_avgx && hitbox.x1 >= m_avgx) {
							selecting = true;
						}

						if (selecting) {
							saw_text = true;
							m_rects.push_back(hitbox);

							if (need_newline)
							{
								if (curr_text < max_text - 1) m_text[curr_text++] = '\r\n';
								need_newline = false;
							}

							if (curr_text < max_text - 1) m_text[curr_text++] = c;
						}
					}
				}

				if (saw_text) need_newline = true;
			}
		}
	}
}