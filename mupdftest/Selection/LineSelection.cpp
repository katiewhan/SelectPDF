#include "LineSelection.h"

LineSelection::LineSelection(int x0, int x1, int y)
{

}

LineSelection::~LineSelection()
{
}

void LineSelection::Select(fz_stext_page* page) {

	for (int block_num = 0; block_num < page->len; block_num++)
	{
		fz_stext_line *line;
		fz_stext_block *block;
		fz_stext_span *span;

		if (page->blocks[block_num].type != FZ_PAGE_BLOCK_TEXT)
			continue;
		block = page->blocks[block_num].u.text;

		/*for (line = block->lines; line < block->lines + block->len; line++)
		{
			int saw_text = 0;

			for (span = line->first_span; span; span = span->next)
			{
				for (int i = 0; i < span->len; i++)
				{
					fz_stext_char_bbox(app->ctx, &hitbox, span, i);
					fz_transform_rect(&hitbox, &ctm);
					c = span->text[i].c;
					if (c < 32)
						c = '?';
					if (hitbox.x1 >= x0 && hitbox.x0 <= x1 && hitbox.y1 >= y0 && hitbox.y0 <= y1)
					{
						saw_text = 1;

						if (need_newline)
						{
							if (p < ucslen - 1)
								ucsbuf[p++] = '\r\n';
							need_newline = 0;
						}

						if (p < ucslen - 1)
							ucsbuf[p++] = c;
					}
				}
			}

			if (saw_text)
				need_newline = 1;
		}*/
	}
}