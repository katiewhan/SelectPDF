#include "Selection.h"

Selection::Selection()
{
	m_rects = nullptr;
	fz_var(m_rects);

	m_num_rects = 0;
}

fz_rect* Selection::GetRects() {
	return m_rects;
}

int Selection::GetNumRects() {
	return m_num_rects;
}
