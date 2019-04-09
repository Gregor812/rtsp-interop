#include <cstdio>

#include "streams.h"

InputFormatContextResult OpenInputStream()
{
    const char *inputFile = "rtsp://184.72.239.149/vod/mp4:BigBuckBunny_115k.mov";
    InputFormatContextResult result;
    result.InputFormatContext = (AVFormatContext *)0;

    printf("Opening file %s...", inputFile);
    result.ErrorCode = avformat_open_input(&(result.InputFormatContext), inputFile, 0, 0);
    if (result.ErrorCode < 0)
    {
        fprintf(stderr, "Could not open input file '%s'", inputFile);
        return result;
    }

    printf("File %s successfully opened", inputFile);

    result.ErrorCode = avformat_find_stream_info(result.InputFormatContext, 0);
    if (result.ErrorCode < 0)
    {
        fprintf(stderr, "Failed to retrieve input stream information");
        return result;
    }

    av_dump_format(result.InputFormatContext, 0, inputFile, 0);
    return result;
}
