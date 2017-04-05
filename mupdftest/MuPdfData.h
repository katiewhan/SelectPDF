#pragma once

struct mu_point {
	int x;
	int y;
};

struct mu_rect {
	mu_rect() {
		x0 = 0;
		y0 = 0;
		x1 = 0;
		y1 = 0;
	}
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