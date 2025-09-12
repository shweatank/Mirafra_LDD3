#include <stdio.h>      // Standard I/O for printf, fopen, fwrite, etc.
#include <stdlib.h>     // Standard library functions (malloc, free, exit)
#include <libavformat/avformat.h>  // FFmpeg: Demuxer and container format handling
#include <libavcodec/avcodec.h>    // FFmpeg: Decoder/encoder functions
#include <libswscale/swscale.h>    // FFmpeg: Scaling and pixel format conversion
#include <libavutil/imgutils.h>    // FFmpeg: Utility functions for images

int main(int argc, char *argv[]) {
    // -------------------- CHECK INPUT ARGUMENTS --------------------
    if (argc < 2) {
        // If no input video file is provided
        printf("Usage: %s <input.h265>\n", argv[0]);
        return -1;
    }

    const char *input_file = argv[1];   // Input H.265 video file path (from argv)
    const char *out_dir = "frames";     // Directory to store extracted raw frames

    // Declare FFmpeg structures
    AVFormatContext *fmt_ctx = NULL;    // Holds input file format and stream info
    AVCodecContext *codec_ctx = NULL;   // Holds decoder settings and state
    const AVCodec *codec = NULL;        // Points to the chosen decoder (HEVC/H.265)
    AVFrame *frame = NULL;              // Stores decoded raw frame (YUV)
    AVPacket *pkt = NULL;               // Stores compressed packet read from file
    int video_stream_index = -1;        // To hold the index of the video stream
    int frame_count = 0;                // Counter for extracted frames

    // -------------------- OPEN INPUT FILE --------------------
    // avformat_open_input(&fmt_ctx, input_file, NULL, NULL)
    // &fmt_ctx  -> Output: FFmpeg fills this with file context
    // input_file -> Input: filename provided by user
    // NULL       -> Input format (let FFmpeg auto-detect)
    // NULL       -> Options dictionary (use defaults)
    if (avformat_open_input(&fmt_ctx, input_file, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open input file %s\n", input_file);
        return -1;
    }

    // -------------------- RETRIEVE STREAM INFO --------------------
    // avformat_find_stream_info(fmt_ctx, NULL)
    // fmt_ctx -> Input: already opened file
    // NULL    -> Options dictionary (default)
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream info\n");
        return -1;
    }

    // -------------------- FIND VIDEO STREAM --------------------
    // Loop through all streams and find the first video stream
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;  // Found video stream
            break;
        }
    }
    if (video_stream_index == -1) {
        fprintf(stderr, "No video stream found\n");
        return -1;
    }

    // -------------------- FIND DECODER --------------------
    AVCodecParameters *codecpar = fmt_ctx->streams[video_stream_index]->codecpar;
    // avcodec_find_decoder(codecpar->codec_id)
    // codecpar->codec_id -> Input: codec ID (e.g., AV_CODEC_ID_HEVC)
    // Returns a decoder for H.265 if available
    codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        fprintf(stderr, "H.265 decoder not found\n");
        return -1;
    }

    // -------------------- ALLOCATE CODEC CONTEXT --------------------
    // avcodec_alloc_context3(codec)
    // codec -> Which decoder to prepare context for
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        fprintf(stderr, "Could not allocate codec context\n");
        return -1;
    }

    // -------------------- COPY STREAM PARAMETERS --------------------
    // avcodec_parameters_to_context(codec_ctx, codecpar)
    // codec_ctx -> Destination (decoder context)
    // codecpar  -> Source (stream parameters like width, height, format)
    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        fprintf(stderr, "Could not copy codec parameters\n");
        return -1;
    }

    // -------------------- OPEN DECODER --------------------
    // avcodec_open2(codec_ctx, codec, NULL)
    // codec_ctx -> Decoder context
    // codec     -> The decoder itself (H.265)
    // NULL      -> Options dictionary (default)
    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return -1;
    }

    // -------------------- PREPARE SCALING CONTEXT --------------------
    // sws_getContext(srcW, srcH, srcFormat, dstW, dstH, dstFormat, flags, NULL, NULL, NULL)
    // codec_ctx->width, codec_ctx->height -> Input dimensions
    // codec_ctx->pix_fmt                 -> Input pixel format (YUV420P usually)
    // 176, 220                           -> Output resolution (target display size)
    // AV_PIX_FMT_RGB565                  -> Output format (RGB565 for embedded display)
    // SWS_BILINEAR                       -> Scaling algorithm
    // NULL, NULL, NULL                   -> Custom filter params (not needed)
    struct SwsContext *sws_ctx = sws_getContext(
        codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
        176, 220, AV_PIX_FMT_RGB565,
        SWS_BILINEAR, NULL, NULL, NULL
    );
    if (!sws_ctx) {
        fprintf(stderr, "Could not initialize sws context\n");
        return -1;
    }

    // Allocate memory for reusable frame and packet
    frame = av_frame_alloc();     // For decoded YUV frames
    pkt = av_packet_alloc();      // For compressed packets

    // -------------------- READ AND DECODE FRAMES --------------------
    while (av_read_frame(fmt_ctx, pkt) >= 0 && frame_count < 100) {
        // Only process packets belonging to the video stream
        if (pkt->stream_index == video_stream_index) {
            // Send compressed packet to decoder
            if (avcodec_send_packet(codec_ctx, pkt) < 0) {
                av_packet_unref(pkt); // Drop packet on error
                continue;
            }

            // Receive decoded frames (may return >1 frame per packet)
            while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                // Allocate RGB565 frame
                AVFrame *rgb_frame = av_frame_alloc();

                // Get buffer size needed for RGB565 at 176x220
                int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB565, 176, 220, 1);

                // Allocate raw buffer
                uint8_t *buffer = (uint8_t *)av_malloc(num_bytes);

                // Map buffer to rgb_frame->data
                av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize,
                                     buffer, AV_PIX_FMT_RGB565, 176, 220, 1);

                // Convert YUV frame -> RGB565
                sws_scale(sws_ctx,
                          (const uint8_t * const *)frame->data,
                          frame->linesize, 0, codec_ctx->height,
                          rgb_frame->data, rgb_frame->linesize);

                // -------------------- SAVE FRAME TO FILE --------------------
                char filename[256];
                snprintf(filename, sizeof(filename), "%s/photo-%03d.rgb", out_dir, frame_count + 1);

                FILE *fout = fopen(filename, "wb");
                if (fout) {
                    // fwrite(buffer, element_size, count, file)
                    // buffer       -> Source data
                    // 1            -> Each element is 1 byte
                    // num_bytes    -> Total number of bytes
                    // fout         -> Output file
                    fwrite(buffer, 1, num_bytes, fout);
                    fclose(fout);
                }

                // Free memory for this frame
                av_free(buffer);
                av_frame_free(&rgb_frame);

                // Increment frame counter
                frame_count++;
                if (frame_count >= 100) break; // Limit to 100 frames
            }
        }
        // Free compressed packet after use
        av_packet_unref(pkt);
    }

    // -------------------- FLUSH DECODER --------------------
    // Send NULL packet to signal end of input
    avcodec_send_packet(codec_ctx, NULL);
    while (avcodec_receive_frame(codec_ctx, frame) == 0 && frame_count < 100) {
        // (You could repeat the same sws_scale + fwrite logic here if needed)
    }

    // -------------------- CLEANUP --------------------
    av_frame_free(&frame);         // Free decoded frame
    av_packet_free(&pkt);          // Free packet
    sws_freeContext(sws_ctx);      // Free scaling context
    avcodec_free_context(&codec_ctx); // Free codec context
    avformat_close_input(&fmt_ctx);   // Close input file

    printf("Extracted %d frames in RGB565 format.\n", frame_count);
    return 0;
}

