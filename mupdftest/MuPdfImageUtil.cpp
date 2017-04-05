#include "MuPdfImageUtil.h"

MuPdfImageUtil::MuPdfImageUtil()
{
}

MuPdfImageUtil::~MuPdfImageUtil()
{
}

fz_buffer* MuPdfImageUtil::png_from_pixmap(fz_context *ctx, fz_pixmap *pix, int drop)
{
	fz_buffer *buf = NULL;
	fz_output *out;
	fz_pixmap *pix2 = NULL;

	fz_var(buf);
	fz_var(out);
	fz_var(pix2);

	if (pix->w == 0 || pix->h == 0)
		return NULL;

	fz_try(ctx)
	{
		if (pix->colorspace && pix->colorspace != fz_device_gray(ctx) && pix->colorspace != fz_device_rgb(ctx))
		{
			pix2 = fz_new_pixmap(ctx, fz_device_rgb(ctx), pix->w, pix->h, pix->alpha);
			fz_convert_pixmap(ctx, pix2, pix);
			if (drop)
				fz_drop_pixmap(ctx, pix);
			pix = pix2;
		}
		buf = fz_new_buffer(ctx, 1024);
		out = fz_new_output_with_buffer(ctx, buf);
		fz_write_pixmap_as_png(ctx, out, pix);
	}
	fz_always(ctx)
	{
		fz_drop_pixmap(ctx, drop ? pix : pix2);
		fz_drop_output(ctx, out);
	}
	fz_catch(ctx)
	{
		fz_drop_buffer(ctx, buf);
		fz_rethrow(ctx);
	}
	return buf;
}