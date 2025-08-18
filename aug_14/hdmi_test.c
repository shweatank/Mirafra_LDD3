/*Build prerequisites
sudo apt-get install build-essential pkg-config libdrm-dev

Build
cc -O2 -Wall show_image_drm.c -o show_image_drm $(pkg-config --cflags --libs libdrm) -lm

Run
# Show image on the first connected HDMI connector:
sudo ./show_image_drm /path/to/image.png

# Force a specific connector type (HDMI-A, DSI, eDP, LVDS, DP) or id:
sudo ./show_image_drm /path/to/image.jpg --type HDMI-A
sudo ./show_image_drm /path/to/image.jpg --type DSI
sudo ./show_image_drm /path/to/image.jpg --id 42


The image is scaled to screen, preserving aspect (letterbox/pillarbox with black bars).

Program restores the previous CRTC config on exit (best effort).

If multiple outputs are connected, it prioritizes the requested type; otherwise uses the first connected.

show_image_drm.c
// show_image_drm.c
// Display an image full-screen over DRM/KMS (HDMI/DSI/etc.) using a dumb buffer + libdrm.
// Single-file, uses stb_image to load PNG/JPG/BMP/TGA/etc.
//
// Build: cc -O2 -Wall show_image_drm.c -o show_image_drm $(pkg-config --cflags --libs libdrm) -lm
// Run:   sudo ./show_image_drm /path/to/image.png [--type HDMI-A|DSI|DP|eDP|LVDS] [--id <connector_id>]
*/
#define _GNU_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <math.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_mode.h>

// ------------------------- stb_image (header-only) -------------------------
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_GIF
#define STBI_NO_PNM
#define STBI_MALLOC(sz)        malloc(sz)
#define STBI_REALLOC(p,newsz)  realloc(p,newsz)
#define STBI_FREE(p)           free(p)
#include "stb_image.h"
// --------------------------------------------------------------------------

// Helper: ioctl wrapper
static int xioctl(int fd, unsigned long req, void *arg) {
    int r;
    do { r = ioctl(fd, req, arg); } while (r == -1 && (errno == EINTR || errno == EAGAIN));
    return r;
}

typedef struct {
    uint32_t handle;
    uint32_t pitch;
    uint64_t size;
    void*    map;
    int      fb_id;
    int      width;
    int      height;
    uint32_t format; // DRM_FORMAT_XRGB8888
} DumbFB;

typedef struct {
    int fd;
    uint32_t crtc_id;
    uint32_t connector_id;
    drmModeModeInfo mode;
    drmModeCrtc *saved_crtc; // to restore on exit
    DumbFB fb;
} Display;

// Map DRM connector type strings to enum
static const struct { const char* name; uint32_t type; } kConnTypes[] = {
    {"VGA", DRM_MODE_CONNECTOR_VGA},
    {"DVI", DRM_MODE_CONNECTOR_DVID},
    {"HDMI-A", DRM_MODE_CONNECTOR_HDMIA},
    {"HDMI-B", DRM_MODE_CONNECTOR_HDMIB},
    {"DP", DRM_MODE_CONNECTOR_DisplayPort},
    {"eDP", DRM_MODE_CONNECTOR_eDP},
    {"LVDS", DRM_MODE_CONNECTOR_LVDS},
    {"DSI", DRM_MODE_CONNECTOR_DSI},
    {"TV", DRM_MODE_CONNECTOR_TV},
    {"Virtual", DRM_MODE_CONNECTOR_VIRTUAL},
    {"USB", DRM_MODE_CONNECTOR_USB},
};

static uint32_t parse_connector_type(const char* s) {
    if (!s) return 0;
    for (size_t i=0; i<sizeof(kConnTypes)/sizeof(kConnTypes[0]); ++i) {
        if (strcmp(kConnTypes[i].name, s) == 0) return kConnTypes[i].type;
    }
    return 0;
}

static const char* type_to_str(uint32_t t) {
    for (size_t i=0; i<sizeof(kConnTypes)/sizeof(kConnTypes[0]); ++i)
        if (kConnTypes[i].type == t) return kConnTypes[i].name;
    return "UNKNOWN";
}

static int open_card(void) {
    // Try card0..card3
    const char* paths[] = {"/dev/dri/card0","/dev/dri/card1","/dev/dri/card2","/dev/dri/card3"};
    for (size_t i=0;i<sizeof(paths)/sizeof(paths[0]);++i) {
        int fd = open(paths[i], O_RDWR | O_CLOEXEC);
        if (fd >= 0) return fd;
    }
    return -1;
}

static int pick_connector_and_crtc(int fd, uint32_t want_type, uint32_t want_id,
                                   uint32_t* out_conn, uint32_t* out_crtc, drmModeModeInfo* out_mode) {
    drmModeRes *res = drmModeGetResources(fd);
    if (!res) { perror("drmModeGetResources"); return -1; }

    drmModeConnector *best_conn = NULL;
    drmModeModeInfo   best_mode = {0};

    // First pass: prefer requested connector id if given
    for (int i=0;i<res->count_connectors;i++) {
        drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[i]);
        if (!conn) continue;
        if (conn->connection == DRM_MODE_CONNECTED) {
            if (want_id && conn->connector_id == want_id) {
                if (conn->count_modes > 0) {
                    best_conn = conn; best_mode = conn->modes[0];
                    break;
                }
            }
        }
        drmModeFreeConnector(conn);
    }

    // Second pass: prefer requested type
    if (!best_conn) {
        for (int i=0;i<res->count_connectors;i++) {
            drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[i]);
            if (!conn) continue;
            if (conn->connection == DRM_MODE_CONNECTED) {
                if (want_type && conn->connector_type != want_type) {
                    drmModeFreeConnector(conn);
                    continue;
                }
                if (conn->count_modes > 0) {
                    best_conn = conn; best_mode = conn->modes[0];
                    break;
                }
            }
            drmModeFreeConnector(conn);
        }
    }

    // Fallback: first connected anything
    if (!best_conn) {
        for (int i=0;i<res->count_connectors;i++) {
            drmModeConnector *conn = drmModeGetConnector(fd, res->connectors[i]);
            if (!conn) continue;
            if (conn->connection == DRM_MODE_CONNECTED && conn->count_modes > 0) {
                best_conn = conn; best_mode = conn->modes[0];
                break;
            }
            drmModeFreeConnector(conn);
        }
    }

    if (!best_conn) {
        fprintf(stderr, "No connected connector found.\n");
        drmModeFreeResources(res);
        return -1;
    }

    // Find a CRTC compatible with one of the connector's encoders
    uint32_t crtc_id = 0;
    for (int i = 0; i < best_conn->count_encoders && crtc_id==0; i++) {
        drmModeEncoder *enc = drmModeGetEncoder(fd, best_conn->encoders[i]);
        if (!enc) continue;
        if (enc->crtc_id) {
            crtc_id = enc->crtc_id;
        } else {
            // Search for a CRTC in the resource list with matching possible_crtcs
            for (int j=0; j<res->count_crtcs; j++) {
                if (enc->possible_crtcs & (1 << j)) {
                    crtc_id = res->crtcs[j];
                    break;
                }
            }
        }
        drmModeFreeEncoder(enc);
    }

    if (!crtc_id) {
        fprintf(stderr, "Failed to find CRTC for connector %u.\n", best_conn->connector_id);
        drmModeFreeConnector(best_conn);
        drmModeFreeResources(res);
        return -1;
    }

    *out_conn = best_conn->connector_id;
    *out_crtc = crtc_id;
    *out_mode = best_mode;

    fprintf(stderr, "Using connector %u (%s) %ux%u@%u\n",
            best_conn->connector_id, type_to_str(best_conn->connector_type),
            best_mode.hdisplay, best_mode.vdisplay, best_mode.vrefresh);

    drmModeFreeConnector(best_conn);
    drmModeFreeResources(res);
    return 0;
}

static int create_fb(int fd, int width, int height, DumbFB* out) {
    struct drm_mode_create_dumb creq = {0};
    creq.width  = width;
    creq.height = height;
    creq.bpp    = 32; // XRGB8888
    if (xioctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq) < 0) {
        perror("DRM_IOCTL_MODE_CREATE_DUMB");
        return -1;
    }

    uint32_t handle = creq.handle;
    uint32_t pitch  = creq.pitch;
    uint64_t size   = creq.size;

    uint32_t fb_id = 0;
    if (drmModeAddFB(fd, width, height, 24, 32, pitch, handle, &fb_id)) {
        perror("drmModeAddFB");
        // cleanup dumb
        struct drm_mode_destroy_dumb dreq = {.handle = handle};
        xioctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
        return -1;
    }

    struct drm_mode_map_dumb mreq = {.handle = handle};
    if (xioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq) < 0) {
        perror("DRM_IOCTL_MODE_MAP_DUMB");
        drmModeRmFB(fd, fb_id);
        struct drm_mode_destroy_dumb dreq = {.handle = handle};
        xioctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
        return -1;
    }

    void* map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
    if (map == MAP_FAILED) {
        perror("mmap dumb");
        drmModeRmFB(fd, fb_id);
        struct drm_mode_destroy_dumb dreq = {.handle = handle};
        xioctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
        return -1;
    }

    out->handle = handle;
    out->pitch  = pitch;
    out->size   = size;
    out->map    = map;
    out->fb_id  = fb_id;
    out->width  = width;
    out->height = height;
    out->format = DRM_FORMAT_XRGB8888;

    // Clear to black
    memset(out->map, 0, size);
    return 0;
}

static void destroy_fb(int fd, DumbFB* fb) {
    if (fb->map && fb->map != MAP_FAILED) munmap(fb->map, fb->size);
    if (fb->fb_id) drmModeRmFB(fd, fb->fb_id);
    if (fb->handle) {
        struct drm_mode_destroy_dumb dreq = {.handle = fb->handle};
        xioctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
    }
    memset(fb, 0, sizeof(*fb));
}

// Letterbox copy: scale src into dst keeping aspect, black borders
static void blit_letterbox(uint8_t* dst, int dst_w, int dst_h, int dst_pitch,
                           const uint8_t* src, int src_w, int src_h, int src_ch) {
    // Expect src_ch == 3 or 4
    double sx = (double)dst_w / src_w;
    double sy = (double)dst_h / src_h;
    double scale = (sx < sy) ? sx : sy; // fit
    int out_w = (int)round(src_w * scale);
    int out_h = (int)round(src_h * scale);
    int off_x = (dst_w - out_w) / 2;
    int off_y = (dst_h - out_h) / 2;

    // Simple nearest-neighbor scaler for brevity (fast, good enough for static image)
    for (int y = 0; y < out_h; ++y) {
        int syy = (int)((double)y / (double)out_h * src_h);
        uint8_t* row = dst + (y + off_y) * dst_pitch + off_x * 4;
        for (int x = 0; x < out_w; ++x) {
            int sxx = (int)((double)x / (double)out_w * src_w);
            const uint8_t* sp = src + (syy * src_w + sxx) * src_ch;
            uint8_t r, g, b;
            if (src_ch == 4) { r = sp[0]; g = sp[1]; b = sp[2]; }
            else             { r = sp[0]; g = sp[1]; b = sp[2]; }
            // XRGB8888: [B][G][R][X]
            row[x*4 + 0] = b;
            row[x*4 + 1] = g;
            row[x*4 + 2] = r;
            row[x*4 + 3] = 0xFF;
        }
    }
}

static void usage(const char* prog) {
    fprintf(stderr, "Usage: sudo %s <image.(png|jpg|bmp|tga)> [--type HDMI-A|DSI|DP|eDP|LVDS] [--id <connector_id>]\n", prog);
}

int main(int argc, char** argv) {
    const char* img_path = NULL;
    uint32_t want_type = 0;
    uint32_t want_id = 0;

    for (int i=1; i<argc; ++i) {
        if (!img_path) { img_path = argv[i]; continue; }
        if (strcmp(argv[i], "--type") == 0 && i+1 < argc) {
            want_type = parse_connector_type(argv[++i]);
            if (!want_type) { fprintf(stderr, "Unknown connector type.\n"); return 1; }
        } else if (strcmp(argv[i], "--id") == 0 && i+1 < argc) {
            want_id = (uint32_t)atoi(argv[++i]);
        } else {
            // extra arg? accept as image if not set
            if (!img_path) img_path = argv[i];
        }
    }
    if (!img_path) { usage(argv[0]); return 1; }

    int fd = open_card();
    if (fd < 0) { perror("open /dev/dri/cardX"); return 1; }

    Display disp = {.fd = fd, .saved_crtc = NULL};
    if (pick_connector_and_crtc(fd, want_type, want_id, &disp.connector_id, &disp.crtc_id, &disp.mode) < 0) {
        close(fd); return 1;
    }

    // Save current CRTC to restore later
    disp.saved_crtc = drmModeGetCrtc(fd, disp.crtc_id);

    // Create FB sized to mode
    if (create_fb(fd, disp.mode.hdisplay, disp.mode.vdisplay, &disp.fb) < 0) {
        if (disp.saved_crtc) drmModeFreeCrtc(disp.saved_crtc);
        close(fd); return 1;
    }

    // Set CRTC with our FB
    if (drmModeSetCrtc(fd, disp.crtc_id, disp.fb.fb_id, 0, 0, &disp.connector_id, 1, &disp.mode)) {
        perror("drmModeSetCrtc");
        destroy_fb(fd, &disp.fb);
        if (disp.saved_crtc) drmModeFreeCrtc(disp.saved_crtc);
        close(fd);
        return 1;
    }

    // Load image
    int iw, ih, ich;
    // Ask for 3 channels (RGB) from stb_image to simplify
    stbi_uc* pixels = stbi_load(img_path, &iw, &ih, &ich, 3);
    if (!pixels) {
        fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
        // Keep screen black but keep program alive briefly for visibility
        sleep(2);
        // Restore and exit
        if (disp.saved_crtc) {
            drmModeSetCrtc(fd, disp.saved_crtc->crtc_id, disp.saved_crtc->buffer_id,
                           disp.saved_crtc->x, disp.saved_crtc->y,
                           &disp.connector_id, 1, &disp.saved_crtc->mode);
            drmModeFreeCrtc(disp.saved_crtc);
        }
        destroy_fb(fd, &disp.fb);
        close(fd);
        return 1;
    }

    // Blit with letterboxing
    blit_letterbox((uint8_t*)disp.fb.map, disp.fb.width, disp.fb.height, disp.fb.pitch,
                   pixels, iw, ih, 3);

    stbi_image_free(pixels);

    // Keep displayed until user presses Enter/Ctrl-C
    fprintf(stderr, "Image shown on connector %u. Press Enter to exit...\n", disp.connector_id);
    getchar();

    // Restore prior CRTC if possible
    if (disp.saved_crtc) {
        drmModeSetCrtc(fd, disp.saved_crtc->crtc_id, disp.saved_crtc->buffer_id,
                       disp.saved_crtc->x, disp.saved_crtc->y,
                       &disp.connector_id, 1, &disp.saved_crtc->mode);
        drmModeFreeCrtc(disp.saved_crtc);
    }

    destroy_fb(fd, &disp.fb);
    close(fd);
    return 0;
}

Notes & tips

Raspberry Pi: Ensure you’re using the KMS driver (dtoverlay=vc4-kms-v3d in /boot/firmware/config.txt or /boot/config.txt depending on distro). Reboot after enabling.

MIPI-DSI: Use --type DSI to target a DSI panel; the program will pick the connected DSI connector if available.

Multiple outputs: If both HDMI and DSI are connected, use --type or --id to choose explicitly.

Pixel format: Uses XRGB8888. Most KMS devices support this. If your device prefers ARGB8888 or RGB565, we can tweak it.

Scaling quality: Nearest-neighbor for simplicity. If you want bilinear scaling and/or EXIF rotation, say the word and I’ll wire it in.
