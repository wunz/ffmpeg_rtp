#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include "video_debugging.h"
typedef unsigned char bool;
#define false 0
#define true 1
int DataHandle(unsigned char **cBufOut, unsigned char* cBufIn);
bool CreateDecoder(unsigned char **cBufOut, unsigned char* cBufIn);




//对socket接过来的数据进行处理
int DataHandle(unsigned char **cBufOut, unsigned char* cBufIn){

	return 1;
}

//对传入的视频进行解码
bool CreateDecoder(unsigned char **cBufOut, unsigned char* cBufIn)
{
	AVCodec *pVideoCodec;
	AVCodecContext *pCodecCtx;
	AVFrame *pFrameYUV;
	AVFrame *pFrame;
	AVPacket pktData;
	int nGopPicture;
	int iRetOpen;
	int iSize;
	int iRetDecode;
	int iHeight;
	int iWidth;
	struct SwsContext *SwrCtxVideo;
	
	//1.注册组件
	av_register_all();
	
	//2.确定解码的数据格式
	pVideoCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (pVideoCodec == NULL) {
		printf("Codec not found\n");
		return false;
	}

	//3.存储对应流的解码信息
	pCodecCtx = avcodec_alloc_context3(pVideoCodec);
	if (pCodecCtx == NULL) {
		printf("avcodec_alloc_context3  err\n");
		return false;
	}

	//4.初始化参数，下面的参数应该由具体的业务决定  
	pCodecCtx->time_base.num = 1;
	pCodecCtx->frame_number = 1; //每包一个视频帧  
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;//视频
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;//解码样式
	pCodecCtx->time_base.den = 228;//帧率--外部参数填充  
	pCodecCtx->width = 1280;//视频宽--外部参数填充  
	pCodecCtx->height = 720;//视频高--外部参数填充 

	//5.打开视频解码器
	iRetOpen=avcodec_open2(pCodecCtx, pVideoCodec, NULL);
	if (iRetOpen<0) {
		printf("Could not open codec\n");
		return false;
	}

	//6.设置视频参数
	//手动为 pFrameYUV分配数据缓存空间
	pFrameYUV = av_frame_alloc();
	uint8_t * VideoOutBuffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	// 将分配的数据缓存空间和AVFrame关联起来
	avpicture_fill((AVPicture *)pFrameYUV, VideoOutBuffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	SwrCtxVideo = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);

	//7.填充的数据
	pFrame = av_frame_alloc();
	//仅仅是把pkt的参数设为默认值，要求pkt的内存已经分配好了，如果为NULL，则此处会崩溃
	nGopPicture = 1;
	av_init_packet(&pktData);
	pktData.size = 49152;//数据长度--外部参数填充
	//pktData.dts = cBuf[1];//解码时间戳--外部参数填充
	pktData.data = cBufIn;//原始数据--外部参数填充

	//8.该函数的作用是实现压缩视频的解码。 
	iRetDecode = avcodec_decode_video2(pCodecCtx, pFrame, &nGopPicture, &pktData);
	if (iRetDecode < 0) {
		printf("Decode Error.\n");
		return -1;
	}
	//9.解码成功
	if (nGopPicture) {
		sws_scale(SwrCtxVideo, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			pFrameYUV->data, pFrameYUV->linesize);

		//外面申请内存  
		iHeight = pCodecCtx->height;
		iWidth = pCodecCtx->width;
		//写入数据  
		int iTemp = 0, i;
		for (i = 0; i<iHeight; i++)
		{
			memcpy(*cBufOut + iTemp, pFrameYUV->data[0] + i * pFrameYUV->linesize[0], iWidth);
			iTemp += iWidth;
		}
		for (i = 0; i<iHeight/2; i++)
		{
			memcpy(*cBufOut + iTemp, pFrameYUV->data[1] + i * pFrameYUV->linesize[1], iWidth /2);
			iTemp += iWidth/2;
		}
		for (i = 0; i<iHeight/2; i++)
		{
			memcpy(*cBufOut + iTemp, pFrameYUV->data[2] + i * pFrameYUV->linesize[2], iWidth /2);
			iTemp += iWidth/2;
		}

	}

	//10.释放资源
	av_free_packet(&pktData);
	av_free(VideoOutBuffer);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	return true;
}


    int g_loglevel_ww=LOG_DEBUG;
int main(int argc,char *argv[]) 
{
	unsigned  char *BufInt;
	BufInt = (unsigned  char *)malloc(5548016);

	int width = 1280;
	int height = 720;
	int picSize = width *height;
	int newSize = picSize * 1.5;
	unsigned char *BufOut = (unsigned  char *)malloc(newSize);

	//读数据
	FILE* fpH264 = fopen(argv[1],"rb+");
	fread(BufInt, 49152,1,fpH264);
	//写数据
	FILE* fpOut=fopen("out.yuv","wb+");
	CreateDecoder(&BufOut, BufInt);
	fwrite(BufOut, newSize,1, fpOut);
	
	fclose(fpOut);
	fclose(fpH264);


	system("pause");
	return 0;

}
