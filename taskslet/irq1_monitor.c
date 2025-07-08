/**
 * irq1_monitor.c – show the running keyboard‑IRQ counter
 *
 * Build :  gcc -Wall -O2 irq1_monitor.c -o irq1_monitor
 * Run   :  ./irq1_monitor
 * Stop  :  Ctrl‑C
 *
 * You do **not** need any special privileges; it only reads
 * /proc/interrupts.  Make sure your kernel module is already
 * loaded so key‑presses really hit IRQ 1 and schedule the tasklet.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void dump_irq1(void)
{
    FILE *fp = fopen("/proc/interrupts", "r");
    if (!fp) {
        perror("/proc/interrupts");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        /* Each line starts with the IRQ number followed by ':' */
        if (strncmp(line, "  1:", 4) == 0 || strncmp(line, " 1:", 3) == 0) {
            /* Print everything after trimming the trailing \n */
            line[strcspn(line, "\n")] = '\0';
            puts(line);
            break;
        }
    }
    fclose(fp);
}

int main(void)
{
    puts("IRQ‑1 counter (updates every 1 s) — hit a key to watch it grow\n");
    while (1) {
        dump_irq1();
        sleep(1);
    }
    return 0;
}

