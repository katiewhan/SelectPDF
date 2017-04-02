#include "Selection.h"

Selection::Selection(int page)
{
	m_page_num = page;
}

std::vector<fz_rect> Selection::GetRects() {
	return m_rects;
}

int Selection::GetNumRects() {
	return m_rects.size();
}

char* Selection::GetText() {
	return m_text;
}
