#include <stdio.h>
#include "video_debugging.h"
FILE *fp_open;
FILE *fp_write;
int g_loglevel_ww=LOG_DEBUG;
int fill_iobuffer(void *opaque, uint8_t *buf, int buf_size)
{

    if (!feof(fp_open))
    {
        int true_size = fread(buf, 1, buf_size, fp_open);
        return true_size;
    }
    else
    {
        return -1;
    }
}
//写文件的回调函数
int write_buffer(void *opaque, uint8_t *buf, int buf_size)
{
    if (!feof(fp_write))
    {
        int true_size = fwrite(buf, 1, buf_size, fp_write);
        return true_size;
    }
    else
    {
        return -1;
    }
}
int main(int argc,char *argv[])
{
    int ret;
    fp_open = fopen(argv[1], "rb+");
    AVFormatContext *ifmt_ctx  = NULL;
    ifmt_ctx = avformat_alloc_context();
    unsigned char *iobuffer = (unsigned char *)av_malloc(32768);
    AVIOContext *avio = avio_alloc_context(iobuffer, 32768, 0, NULL, fill_iobuffer, NULL, NULL);
    ic->pb = avio;
    int err = avformat_open_input(&ifmt_ctx, "nothing", NULL, NULL);
    //解码
    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0)
	{
		printf("Cannot find stream information\n");
		return ret;
	}
    return 0;
}