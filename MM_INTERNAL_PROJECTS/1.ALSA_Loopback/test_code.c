/*
 * alsa_loopback.c
 *
 * Simple ALSA loopback tester:
 *  - Plays a sine wave + occasional impulse (pulse) on playback device
 *  - Captures from capture device and writes raw PCM to an output file
 *  - Detects impulse in captured stream to estimate round-trip latency
 *
 * Compile:
 *   gcc -O2 -o alsa_loopback alsa_loopback.c -lasound -lm
 *
 * Run (example):
 *   ./alsa_loopback hw:Loopback,0,0 hw:Loopback,1,0 out.raw
 *
 * If you use the default ALSA loopback module, typical device strings:
 *   Playback (loopback playback):  hw:Loopback,0,0   (or "default")
 *   Capture  (loopback capture):   hw:Loopback,1,0
 *
 * Notes:
 *  - Output file is raw PCM signed 16-bit little-endian, interleaved.
 *  - You can convert to WAV using sox/ffmpeg:
 *       ffmpeg -f s16le -ar 48000 -ac 2 -i out.raw out.wav
 *
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <alsa/asoundlib.h>

#define DEFAULT_RATE      48000
#define DEFAULT_CHANNELS  2
#define DEFAULT_FORMAT    SND_PCM_FORMAT_S16_LE
#define DEFAULT_PERIOD_FRAMES 1024
#define DEFAULT_PERIODS   4

/* Pulse settings (for latency detection) */
#define PULSE_INTERVAL_SEC 1           /* every 1 second insert a pulse */
#define PULSE_WIDTH_FRAMES 64          /* width of the pulse in frames */
#define PULSE_AMPLITUDE     16000      /* amplitude of the pulse (int16) */
#define PULSE_DETECT_THRESHOLD 10000   /* threshold to detect pulse in capture */

static void print_usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s <playback_dev> <capture_dev> <out_raw_file> [seconds]\n"
        "Example: %s hw:Loopback,0,0 hw:Loopback,1,0 out.raw 10\n",
        prog, prog);
}

/* open and configure PCM (playback or capture) */
static int open_pcm(snd_pcm_t **handle, const char *devname, snd_pcm_stream_t stream,
                    unsigned int rate, int channels, snd_pcm_format_t format,
                    snd_pcm_uframes_t period_size, unsigned int periods)
{
    snd_pcm_hw_params_t *hw;
    snd_pcm_sw_params_t *sw;
    int err;
    snd_pcm_t *pcm;
    unsigned int rrate = rate;
    snd_pcm_uframes_t buffer_size;

    if ((err = snd_pcm_open(&pcm, devname, stream, 0)) < 0) {
        fprintf(stderr, "Error opening PCM device %s (%s)\n", devname, snd_strerror(err));
        return err;
    }

    snd_pcm_hw_params_alloca(&hw);
    snd_pcm_hw_params_any(pcm, hw);
    snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, hw, format);
    snd_pcm_hw_params_set_rate_near(pcm, hw, &rrate, 0);
    snd_pcm_hw_params_set_channels(pcm, hw, channels);

    snd_pcm_hw_params_set_period_size_near(pcm, hw, &period_size, 0);
    buffer_size = period_size * periods;
    snd_pcm_hw_params_set_buffer_size_near(pcm, hw, &buffer_size);

    if ((err = snd_pcm_hw_params(pcm, hw)) < 0) {
        fprintf(stderr, "Error setting HW params (%s)\n", snd_strerror(err));
        snd_pcm_close(pcm);
        return err;
    }

    /* SW params: make device start when enough data available (playback) */
    snd_pcm_sw_params_alloca(&sw);
    snd_pcm_sw_params_current(pcm, sw);
    if (stream == SND_PCM_STREAM_PLAYBACK) {
        snd_pcm_sw_params_set_start_threshold(pcm, sw, period_size);
    } else {
        /* capture: set minimum avail to period */
        snd_pcm_sw_params_set_start_threshold(pcm, sw, 1);
    }
    snd_pcm_sw_params_set_avail_min(pcm, sw, period_size);
    snd_pcm_sw_params(pcm, sw);

    *handle = pcm;
    return 0;
}

/* write frames to playback (interleaved s16 samples) */
static int write_frames(snd_pcm_t *play, int16_t *buf, snd_pcm_uframes_t frames)
{
    int err;
    snd_pcm_uframes_t written = 0;
    while (written < frames) {
        err = snd_pcm_writei(play, buf + (written * DEFAULT_CHANNELS), frames - written);
        if (err == -EAGAIN) continue;
        if (err < 0) {
            if ((err = snd_pcm_recover(play, err, 1)) < 0) {
                fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror(err));
                return err;
            }
            continue;
        }
        written += err;
    }
    return 0;
}

/* read frames from capture */
static int read_frames(snd_pcm_t *cap, int16_t *buf, snd_pcm_uframes_t frames)
{
    int err;
    snd_pcm_uframes_t read = 0;
    while (read < frames) {
        err = snd_pcm_readi(cap, buf + (read * DEFAULT_CHANNELS), frames - read);
        if (err == -EAGAIN) continue;
        if (err < 0) {
            if ((err = snd_pcm_recover(cap, err, 1)) < 0) {
                fprintf(stderr, "snd_pcm_readi failed: %s\n", snd_strerror(err));
                return err;
            }
            continue;
        }
        read += err;
    }
    return 0;
}

/* helper: get monotonic time in ms */
static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

/* Insert pulse in buffer at specified frame offset (interleaved stereo) */
static void insert_pulse(int16_t *buf, snd_pcm_uframes_t frame_offset, snd_pcm_uframes_t pulse_width) {
    for (snd_pcm_uframes_t i = 0; i < pulse_width; ++i) {
        /* left channel pulse, right channel zero (you can change) */
        buf[(frame_offset + i) * DEFAULT_CHANNELS + 0] = PULSE_AMPLITUDE;
        buf[(frame_offset + i) * DEFAULT_CHANNELS + 1] = 0;
    }
}

/* detect pulse in captured buffer, return frame index of detection or -1 */
static long detect_pulse(int16_t *buf, snd_pcm_uframes_t frames) {
    for (snd_pcm_uframes_t i = 0; i < frames; ++i) {
        int16_t left = buf[i * DEFAULT_CHANNELS + 0];
        if (abs(left) > PULSE_DETECT_THRESHOLD) {
            return (long)i;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    const char *play_dev = argv[1];
    const char *cap_dev  = argv[2];
    const char *outfile  = argv[3];
    int run_seconds = 10;
    if (argc >= 5) run_seconds = atoi(argv[4]);

    unsigned int rate = DEFAULT_RATE;
    int channels = DEFAULT_CHANNELS;
    snd_pcm_format_t format = DEFAULT_FORMAT;
    snd_pcm_uframes_t period_size = DEFAULT_PERIOD_FRAMES;

    snd_pcm_t *play = NULL, *cap = NULL;
    int rc;

    printf("Opening playback device %s and capture device %s\n", play_dev, cap_dev);
    rc = open_pcm(&play, play_dev, SND_PCM_STREAM_PLAYBACK, rate, channels, format, period_size, DEFAULT_PERIODS);
    if (rc < 0) return 1;
    rc = open_pcm(&cap, cap_dev, SND_PCM_STREAM_CAPTURE, rate, channels, format, period_size, DEFAULT_PERIODS);
    if (rc < 0) { snd_pcm_close(play); return 1; }

    /* buffers */
    snd_pcm_uframes_t frames_per_buf = period_size;
    int16_t *playbuf = malloc(sizeof(int16_t) * frames_per_buf * channels);
    int16_t *capbuf  = malloc(sizeof(int16_t) * frames_per_buf * channels);
    if (!playbuf || !capbuf) { fprintf(stderr, "Out of memory\n"); return 1; }

    FILE *out = fopen(outfile, "wb");
    if (!out) { perror("fopen"); return 1; }

    /* prepare sine wave + pulse generator */
    double phase = 0.0;
    double freq = 1000.0; /* 1kHz tone */
    double phase_step = 2.0 * M_PI * freq / (double)rate;
    unsigned int frames_between_pulses = rate * PULSE_INTERVAL_SEC;
    unsigned int frames_since_last_pulse = 0;
    unsigned long total_frames_written = 0;
    unsigned long total_frames_read    = 0;

    double t0 = now_ms();
    double capture_detect_time = 0.0;
    double play_pulse_time = 0.0;
    int detected = 0;

    printf("Running loopback test for %d seconds... (pulse every %d sec)\n", run_seconds, PULSE_INTERVAL_SEC);

    /* start with preparing devices */
    snd_pcm_prepare(play);
    snd_pcm_prepare(cap);

    /* main loop: generate playback frames and capture frames */
    unsigned int loops = (run_seconds * rate) / frames_per_buf;
    for (unsigned int loop = 0; loop < loops; ++loop) {

        /* generate one period of frames */
        for (snd_pcm_uframes_t f = 0; f < frames_per_buf; ++f) {
            int16_t sample = (int16_t)(sin(phase) * 8000.0); /* modest amplitude */
            /* interleaved stereo */
            playbuf[f * channels + 0] = sample;
            playbuf[f * channels + 1] = sample;
            phase += phase_step;
            if (phase > 2.0 * M_PI) phase -= 2.0 * M_PI;
        }

        /* insert pulse when appropriate (one pulse per interval) */
        if (frames_since_last_pulse + frames_per_buf >= frames_between_pulses) {
            /* pulse starts in this buffer at offset */
            snd_pcm_uframes_t offset = frames_between_pulses - frames_since_last_pulse;
            if (offset < frames_per_buf) {
                insert_pulse(playbuf, offset, (PULSE_WIDTH_FRAMES < (frames_per_buf - offset) ? PULSE_WIDTH_FRAMES : (frames_per_buf - offset)));
                play_pulse_time = now_ms();
                /* we only mark the last pulse timestamp; detection occurs in capture */
            }
            frames_since_last_pulse = (frames_since_last_pulse + frames_per_buf) - frames_between_pulses;
        } else {
            frames_since_last_pulse += frames_per_buf;
        }

        /* write playback frames */
        rc = write_frames(play, playbuf, frames_per_buf);
        if (rc < 0) break;
        total_frames_written += frames_per_buf;

        /* read capture frames */
        rc = read_frames(cap, capbuf, frames_per_buf);
        if (rc < 0) break;
        total_frames_read += frames_per_buf;

        /* write captured raw PCM to file */
        fwrite(capbuf, sizeof(int16_t), frames_per_buf * channels, out);

        /* try detect pulse in captured buffer if not yet detected */
        if (!detected) {
            long idx = detect_pulse(capbuf, frames_per_buf);
            if (idx >= 0) {
                capture_detect_time = now_ms();
                /* estimate latency in ms = (capture_detect_time - play_pulse_time) */
                double latency_ms = capture_detect_time - play_pulse_time;
                /* If play_pulse_time was zero (pulse happened earlier), we can approximate via frames difference:
                 * But we record play_pulse_time when we inserted the pulse in the playback buffer above.
                 */
                printf("Pulse detected in capture at frame %ld (loop %u). Estimated round-trip latency: %.2f ms\n", idx, loop, latency_ms);
                detected = 1;
            }
        }

        /* optional: print progress */
        if ((loop & 0x7) == 0) {
            double elapsed = now_ms() - t0;
            printf("Elapsed: %.1fs Written frames: %lu Read frames: %lu\n", elapsed/1000.0, total_frames_written, total_frames_read);
        }
    }

    double t1 = now_ms();
    printf("Test complete. Duration: %.2fs\n", (t1 - t0) / 1000.0);

    fclose(out);
    free(playbuf);
    free(capbuf);
    snd_pcm_close(play);
    snd_pcm_close(cap);

    printf("Captured output saved to: %s (raw s16le @ %u Hz, %d channels)\n", outfile, rate, channels);
    if (!detected) printf("Pulse was not detected in capture. Increase pulse amplitude or check loopback routing.\n");

    return 0;
}
