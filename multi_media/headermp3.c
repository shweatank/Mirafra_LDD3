#include \FF\FB@\C0<stdio.h>
#include <stdint.h>
#include <stdlib.h>

const char* mpeg_version[] = {
    "MPEG Version 2.5", "Reserved", "MPEG Version 2", "MPEG Version 1"
};

const char* layer_description[] = {
    "Reserved", "Layer III", "Layer II", "Layer I"
};

const int bitrate_table[2][3][16] = {
    // MPEG Version 1
    {
        // Layer I
        {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0},
        // Layer II
        {0,32,48,56,64,80,96,112,128,160,192,224,256,320,384,0},
        // Layer III
        {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
    },
    // MPEG Version 2/2.5
    {
        // Layer I
        {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0},
        // Layer II
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
        // Layer III
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}
    }
};

const int sampling_rates[4][3] = {
    {11025, 12000, 8000},   // MPEG 2.5
    {0, 0, 0},              // Reserved
    {22050, 24000, 16000},  // MPEG 2
    {44100, 48000, 32000}   // MPEG 1
};

const char* channel_mode[] = {
    "Stereo", "Joint stereo", "Dual channel", "Single channel (Mono)"
};

int main() {
    FILE *file = fopen("output.mp3", "rb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    uint8_t buffer[4];
    while (fread(buffer, 1, 4, file) == 4) {
        // Check sync bits (11 bits)
        if (buffer[0] == 0xFF && (buffer[1] & 0xE0) == 0xE0) {
            // Parse header
            uint8_t version_id = (buffer[1] >> 3) & 0x03;
            uint8_t layer = (buffer[1] >> 1) & 0x03;
            uint8_t bitrate_index = (buffer[2] >> 4) & 0x0F;
            uint8_t sampling_index = (buffer[2] >> 2) & 0x03;
            uint8_t chan_mode = (buffer[3] >> 6) & 0x03;

            int version_group = (version_id == 3) ? 0 : 1; // for bitrate table

            printf("MP3 Header Info:\n");
            printf("  MPEG Version     : %s\n", mpeg_version[version_id]);
            printf("  Layer            : %s\n", layer_description[layer]);
            printf("  Bitrate          : %d kbps\n",
                bitrate_table[version_group][layer == 3 ? 0 : (layer == 2 ? 1 : 2)][bitrate_index]);
            printf("  Sampling Rate    : %d Hz\n",
                sampling_rates[version_id][sampling_index]);
            printf("  Channel Mode     : %s\n", channel_mode[chan_mode]);

            break; // exit after first valid header
        } else {
            // Shift window by 1 byte
            fseek(file, -3, SEEK_CUR);
        }
    }
    int i;
    for(int j=0;j<4;j++)
    {
	    i=7;
	    while(i>=0)
	    {
		    printf("%d ",(buffer[j]>>i)&1);
		    i--;
	    }
	    printf("\n");
    }

    fclose(file);
    return 0;
}

