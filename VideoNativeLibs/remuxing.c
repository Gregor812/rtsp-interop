#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include <stdio.h>

#include "remuxing.h"
#include "streams.h"
#include "utils.h"

static int create_new_part(AVFormatContext **ofmt_ctx, AVOutputFormat **ofmt, AVFormatContext *ifmt_ctx, int* streamMapping)
{
    static size_t file_part_number = 0;
    size_t stream_index = 0;
    char out_filename[32];

    ++file_part_number;
    sprintf(out_filename, "part%08ld.mkv", file_part_number);

    avformat_alloc_output_context2(ofmt_ctx, NULL, NULL, out_filename);
    if (!(*ofmt_ctx))
    {
        fprintf(stderr, "Could not create output context\n");
        return AVERROR_UNKNOWN;
    }

    *ofmt = (*ofmt_ctx)->oformat;

    for (int i = 0; i < ifmt_ctx->nb_streams; i++)
    {
        AVStream *out_stream;
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE)
        {
            streamMapping[i] = -1;
            continue;
        }

        streamMapping[i] = stream_index++;

        out_stream = avformat_new_stream(*ofmt_ctx, NULL);
        if (!out_stream)
        {
            fprintf(stderr, "Failed allocating output stream\n");
            return AVERROR_UNKNOWN;
        }

        if (avcodec_parameters_copy(out_stream->codecpar, in_codecpar) < 0)
        {
            fprintf(stderr, "Failed to copy codec parameters\n");
            return AVERROR_UNKNOWN;
        }
        out_stream->codecpar->codec_tag = 0;
    }

    av_dump_format(*ofmt_ctx, 0, out_filename, 1);

    if (!((*ofmt)->flags & AVFMT_NOFILE))
    {
        if (avio_open(&(*ofmt_ctx)->pb, out_filename, AVIO_FLAG_WRITE) < 0)
        {
            fprintf(stderr, "Could not open output file '%s'", out_filename);
            return AVERROR_UNKNOWN;
        }
    }

    if (avformat_write_header(*ofmt_ctx, NULL) < 0)
    {
        fprintf(stderr, "Error occurred when opening output file\n");
        return AVERROR_UNKNOWN;
    }

    return 0;
}

static void close_output_context(AVFormatContext* ofmt_ctx, AVOutputFormat* ofmt)
{
    av_write_trailer(ofmt_ctx);

    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
    {
        avio_closep(&ofmt_ctx->pb);
    }
    
    avformat_free_context(ofmt_ctx);
}

int remux(Callback callback)
{
    printf("We're in the native code now!\n");
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ofmt_ctx = NULL;
    AVPacket pkt;
    int ret;
    int *streamMapping = NULL;
    int streamMappingSize = 0;
    double part_time_threshold = 10.0;

    InputFormatContextResult inputFormatContextResult = OpenInputStream();
    if (inputFormatContextResult.ErrorCode != 0)
    {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(inputFormatContextResult.ErrorCode));
        return 1;
    }

    streamMappingSize = inputFormatContextResult.InputFormatContext->nb_streams;
    streamMapping = av_mallocz_array(streamMappingSize, sizeof(*streamMapping));
    if (!streamMapping)
    {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    int64_t *lastStreamDts = (int64_t *)malloc(streamMappingSize * sizeof(int64_t));

    for (int i = 0; i < streamMappingSize; ++i)
    {
        lastStreamDts[i] = 0;
    }

    while (1)
    {
        ret = create_new_part(&ofmt_ctx, &ofmt, inputFormatContextResult.InputFormatContext, streamMapping);
        if (ret != 0)
        {
            goto end;
        }

        while (1)
        {
            AVStream *in_stream, *out_stream;

            ret = av_read_frame(inputFormatContextResult.InputFormatContext, &pkt);
            if (ret < 0)
                break;

            in_stream = inputFormatContextResult.InputFormatContext->streams[pkt.stream_index];
            if (pkt.stream_index >= streamMappingSize ||
                streamMapping[pkt.stream_index] < 0)
            {
                av_packet_unref(&pkt);
                continue;
            }

            pkt.stream_index = streamMapping[pkt.stream_index];
            out_stream = ofmt_ctx->streams[pkt.stream_index];
            //LogPacket(inputFormatContextResult.InputFormatContext, &pkt, "in");

            AVRational time_base = inputFormatContextResult.InputFormatContext->streams[pkt.stream_index]->time_base;
            double current_packet_time = av_q2d(time_base) * pkt.pts;

            /* copy packet */
            pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
            pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
            if (pkt.dts < lastStreamDts[pkt.stream_index])
            {
                av_packet_unref(&pkt);
                continue;
            }
            lastStreamDts[pkt.stream_index] = pkt.dts;
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;
            //LogPacket(ofmt_ctx, &pkt, "out");

            ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
            if (ret < 0)
            {
                fprintf(stderr, "Error muxing packet\n");
                break;
            }

            av_packet_unref(&pkt);

            if (part_time_threshold <= current_packet_time)
            {
                part_time_threshold = current_packet_time + 10.0;
                break;
            }
        }

        close_output_context(ofmt_ctx, ofmt);

        if (ret < 0)
        {
            break;
        }
        callback();
    }
end:

    //close_output_context(ofmt_ctx, ofmt);
    avformat_close_input(&inputFormatContextResult.InputFormatContext);

    av_freep(&streamMapping);

    if (ret < 0 && ret != AVERROR_EOF)
    {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    printf("The whole file is splitted\n");

    return 0;
}
