#pragma once

struct mu_point {
	int x;
	int y;
};

struct mu_rect {
	int x0, y0;
	int x1, y1;
};

enum
{
	TEXT_CONTENT = 0,
	IMAGE_CONTENT = 1
};

struct mu_selection {
	int type;
	int num_bytes;
	union
	{
		char* text;
		unsigned char* image;
	} content;
};