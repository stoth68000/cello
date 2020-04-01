#include "framebase.h"

framebase::framebase()
: framesProcessed(0)
{
}

framebase::~framebase()
{
}

void avframe_dump_console(AVFrame *frm)
{
	int planes = av_pix_fmt_count_planes((enum AVPixelFormat)frm->format);
	printf("width = %d height = %d pts = %ld format = %s planes = %d\n",
		frm->width,
		frm->height,
		frm->pts,
		av_get_pix_fmt_name((enum AVPixelFormat)frm->format),
		planes);
	printf("      data: ");
	for (int i = 0; i < planes; i++)
		printf("%p ", frm->data[i]);
	printf("\n");
	printf("  linesize: ");
	for (int i = 0; i < planes; i++)
		printf("%d ", frm->linesize[i]);
	printf("\n");

	if (frm->data[0]) {
		unsigned char *ptr = frm->data[0];
		printf("    pixels: ");
		for (int i = 0; i < 16; i++)
			printf("%02x ", ptr[i]);
		printf("\n");
	}
}

uint64_t framebase::getFramesProcessed()
{
        return framesProcessed;
}

void framebase::incrementFramesProcessed()
{
	framesProcessed++;
}
