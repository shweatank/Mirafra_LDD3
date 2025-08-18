#include <stdio.h>
#include <string.h>

void detect_file_format(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    unsigned char buffer[16] = {0};
    fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    // JPEG
    if (buffer[0] == 0xFF && buffer[1] == 0xD8)
        printf("File format: JPEG (image/jpeg)\n");

    // PNG
    else if (buffer[0] == 0x89 && !memcmp(&buffer[1], "PNG", 3))
        printf("File format: PNG (image/png)\n");

    // GIF
    else if (!memcmp(buffer, "GIF89a", 6) || !memcmp(buffer, "GIF87a", 6))
        printf("File format: GIF (image/gif)\n");

    // BMP
    else if (buffer[0] == 'B' && buffer[1] == 'M')
        printf("File format: BMP (image/bmp)\n");

    // PDF
    else if (!memcmp(buffer, "%PDF", 4))
        printf("File format: PDF (application/pdf)\n");

    // MP3 (ID3 tag or MPEG header)
    else if (!memcmp(buffer, "ID3", 3) || (buffer[0] == 0xFF && (buffer[1] & 0xE0) == 0xE0))
        printf("File format: MP3 (audio/mpeg)\n");

    // WAV
    else if (!memcmp(buffer, "RIFF", 4) && !memcmp(&buffer[8], "WAVE", 4))
        printf("File format: WAV (audio/wav)\n");

    // AVI
    else if (!memcmp(buffer, "RIFF", 4) && !memcmp(&buffer[8], "AVI ", 4))
        printf("File format: AVI (video/x-msvideo)\n");

    // MP4 (ftyp in bytes 4–8)
    else if (!memcmp(&buffer[4], "ftyp", 4))
        printf("File format: MP4 or AVIF (check major brand)\n");

    // ZIP
    else if (buffer[0] == 'P' && buffer[1] == 'K')
        printf("File format: ZIP or DOCX or JAR (application/zip)\n");

    // ELF
    else if (!memcmp(buffer, "\x7F""ELF", 4))
        printf("File format: ELF Executable (Linux binary)\n");

    // Default
    else
        printf("File format: Unknown\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    detect_file_format(argv[1]);
    return 0;
}

