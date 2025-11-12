// tcp_demux_mkv.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345

#define BUFFER_SIZE 4096

// Read callback for AVIOContext
static int socket_read(void *opaque, uint8_t *buf, int buf_size) {
    int sock = (int)(intptr_t)opaque;
    int n = recv(sock, buf, buf_size, 0);
    if (n < 0) return AVERROR(EIO);
    if (n == 0) return AVERROR_EOF;
    return n;
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;

    avformat_network_init();

    // Connect to server
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect failed");
        return 1;
    }
    printf("Connected to server\n");

    // Allocate buffer and AVIOContext
    unsigned char *buffer = av_malloc(BUFFER_SIZE);
    AVIOContext *avio_ctx = avio_alloc_context(
        buffer, BUFFER_SIZE, 0, (void *)(intptr_t)sock,
        socket_read, NULL, NULL
    );

    AVFormatContext *fmt_ctx = avformat_alloc_context();
    fmt_ctx->pb = avio_ctx;
    fmt_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;

    if (avformat_open_input(&fmt_ctx, NULL, NULL, NULL) < 0) {
        fprintf(stderr, "Failed to open input from socket\n");
        return 1;
    }

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Failed to find stream info\n");
        return 1;
    }

    // Find first video and audio streams
    int video_stream_idx = -1, audio_stream_idx = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_stream_idx < 0)
            video_stream_idx = i;
        else if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_stream_idx < 0)
            audio_stream_idx = i;
    }

    // Open output files
    FILE *video_out = fopen("recv_output.h265", "wb");
    FILE *audio_out = fopen("recv_audio.flac", "wb");

    AVPacket pkt;
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == video_stream_idx) {
            fwrite(pkt.data, 1, pkt.size, video_out);
        } else if (pkt.stream_index == audio_stream_idx) {
            fwrite(pkt.data, 1, pkt.size, audio_out);
        }
        av_packet_unref(&pkt);
    }

    printf("Demuxing complete\n");

    fclose(video_out);
    fclose(audio_out);
    avformat_close_input(&fmt_ctx);
    av_freep(&avio_ctx->buffer);
    avio_context_free(&avio_ctx);
    close(sock);

    return 0;
}

