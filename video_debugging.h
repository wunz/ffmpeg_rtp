#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/timestamp.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <libavutil/opt.h>
#include <string.h>
#include <inttypes.h>

    enum{
      LOG_DEBUG,
      LOG_ERROR,
      LOG_INFO
    };
    extern int g_loglevel_ww;
   #define log(loglevel,format,...) \
   do{\
     if(g_loglevel_ww <= loglevel)\
     {\
       if(loglevel == LOG_DEBUG)\
       printf("[DEBUG] [%s] [%s] [%d]" #format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
       if(loglevel == LOG_ERROR)\
       printf("[ERROR] [%s] [%s] [%d]" #format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
       if(loglevel == LOG_INFO)\
       printf("[INFO] [%s] [%s] [%d]" #format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
     }\
   }while(0)
void logging(const char *fmt, ...);
void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt);
void print_timing(char *name, AVFormatContext *avf, AVCodecContext *avc, AVStream *avs);
