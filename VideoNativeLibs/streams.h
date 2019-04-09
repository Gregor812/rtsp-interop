#pragma once

#ifdef __cplusplus
extern "C"
{ 
#endif

    #include <libavformat/avformat.h>

    typedef struct
    {
        AVFormatContext *InputFormatContext;
        int ErrorCode;
    } InputFormatContextResult;

    InputFormatContextResult OpenInputStream();

#ifdef __cplusplus
}
#endif
