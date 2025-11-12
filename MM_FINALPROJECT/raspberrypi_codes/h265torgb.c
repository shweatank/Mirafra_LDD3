#include <stdio.h>
#include <stdlib.h>
#include <libavformat/avformat.h>  // Demuxing (container) functions
#include <libavcodec/avcodec.h>    // Codec functions (decode/encode)
#include <libswscale/swscale.h>    // Pixel format conversion
#include <libavutil/imgutils.h>    // Utilities for image/frame buffer

int main(int argc, char *argv[]) {
    // Check command line argument (input H.265 file)
    if (argc < 2) {
        printf("Usage: %s <input.h265>\n", argv[0]);
        return -1;
    }

    const char *input_file = argv[1];  // Input H.265 file
    const char *out_dir = "frames";    // Directory to save extracted RGB frames

    AVFormatContext *fmt_ctx = NULL;   // Format context (container)
    AVCodecContext *codec_ctx = NULL;  // Codec context (decoder)
    const AVCodec *codec = NULL;       // Decoder
    AVFrame *frame = NULL;             // Decoded video frame
    AVPacket *pkt = NULL;              // Encoded packet
    int video_stream_index = -1;       // Index of the video stream
    int frame_count = 0;               // Count of frames extracted

    // Open input file (container)
    if (avformat_open_input(&fmt_ctx, input_file, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open input file %s\n", input_file);
        return -1;
    }

    // Retrieve stream information (codec, resolution, etc.)
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream info\n");
        return -1;
    }

    // Find the first video stream
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        fprintf(stderr, "No video stream found\n");
        return -1;
    }

    // Get codec parameters from stream
    AVCodecParameters *codecpar = fmt_ctx->streams[video_stream_index]->codecpar;
    codec = avcodec_find_decoder(codecpar->codec_id);  // Find decoder
    if (!codec) {
        fprintf(stderr, "H.265 decoder not found\n");
        return -1;
    }

    // Allocate codec context for decoding
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        fprintf(stderr, "Could not allocate codec context\n");
        return -1;
    }

    // Copy codec parameters from stream to codec context
    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        fprintf(stderr, "Could not copy codec parameters\n");
        return -1;
    }

    // Open codec
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return -1;
    }

    // Prepare scaling/conversion context (original -> RGB565, 176x220)
    struct SwsContext *sws_ctx = sws_getContext(
        codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt, // source
        176, 220, AV_PIX_FMT_RGB565,                              // target
        SWS_BILINEAR, NULL, NULL, NULL                            // scaling options
    );
    if (!sws_ctx) {
        fprintf(stderr, "Could not initialize sws context\n");
        return -1;
    }

    frame = av_frame_alloc();   // Allocate AVFrame for decoded video
    pkt = av_packet_alloc();    // Allocate AVPacket for encoded packet

    // Read packets from file
    while (av_read_frame(fmt_ctx, pkt) >= 0 && frame_count < 100) {
        if (pkt->stream_index == video_stream_index) {           // Only process video stream
            if (avcodec_send_packet(codec_ctx, pkt) < 0) {      // Send packet to decoder
                av_packet_unref(pkt);
                continue;
            }

            // Receive all available frames from decoder
            while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                // Allocate output frame (RGB565)
                AVFrame *rgb_frame = av_frame_alloc();
                int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB565, 176, 220, 1);
                uint8_t *buffer = (uint8_t *)av_malloc(num_bytes);
                av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize,
                                     buffer, AV_PIX_FMT_RGB565, 176, 220, 1);

                // Convert decoded frame to RGB565 using sws_scale
                sws_scale(sws_ctx,
                          (const uint8_t * const *)frame->data,
                          frame->linesize, 0, codec_ctx->height,
                          rgb_frame->data, rgb_frame->linesize);

                // Save RGB frame to file
                char filename[256];
                snprintf(filename, sizeof(filename), "%s/photo-%03d.rgb", out_dir, frame_count + 1);
                FILE *fout = fopen(filename, "wb");
                if (fout) {
                    fwrite(buffer, 1, num_bytes, fout);
                    fclose(fout);
                }

                av_free(buffer);          // Free allocated RGB buffer
                av_frame_free(&rgb_frame); // Free RGB frame

                frame_count++;
                if (frame_count >= 100) break;
            }
        }
        av_packet_unref(pkt); // Free packet memory
    }

    // Flush decoder to get delayed frames (B-frames)
    avcodec_send_packet(codec_ctx, NULL); // Send NULL to signal EOF
    while (avcodec_receive_frame(codec_ctx, frame) == 0 && frame_count < 100) {
        // Same sws_scale + save as above if needed
    }

    // Cleanup
    av_frame_free(&frame);
    av_packet_free(&pkt);
    sws_freeContext(sws_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    printf("Extracted %d frames in RGB565 format.\n", frame_count);
    return 0;
}

