#include <stdio.h>
#include <stdlib.h>

int main() {
    char cmd[256];
    for (int i = 1; i <= 150; i++) {
        if (i < 10) {
            // Build resize command
            snprintf(cmd, sizeof(cmd),
                     "./resize_image output-[00%d].png output_resized-[00%d].png 128 32",
                     i, i);
            if (system(cmd) == 0) {
                // Build remove command
                snprintf(cmd, sizeof(cmd),
                         "rm output-[00%d].png", i);
                system(cmd);
            }
        }
	else if (i < 100 && i >= 10) {
            // Build resize command
            snprintf(cmd, sizeof(cmd),
                     "./resize_image output-[0%d].png output_resized-[0%d].png 128 32",
                     i, i);
            if (system(cmd) == 0) {
                // Build remove command
                snprintf(cmd, sizeof(cmd),
                         "rm output-[0%d].png", i);
                system(cmd);
            }
	}
	    else if(i>=100)
	    {
		    // Build resize command
            snprintf(cmd, sizeof(cmd),
                     "./resize_image output-[%d].png output_resized-[%d].png 128 32",
                     i, i);
            if (system(cmd) == 0) {
                // Build remove command
                snprintf(cmd, sizeof(cmd),
                         "rm output-[%d].png", i);
                system(cmd);
            }

	    }
        

    }
    return 0;
}

