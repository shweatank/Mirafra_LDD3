#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char scancode_to_ascii(unsigned char sc) {
    char map[128] = {0};
    map[0x02] = '1'; map[0x03] = '2'; map[0x04] = '3';
    map[0x05] = '4'; map[0x06] = '5'; map[0x07] = '6';
    map[0x08] = '7'; map[0x09] = '8'; map[0x0A] = '9';
    map[0x0B] = '0'; map[0x1C] = '\n';
    map[0x4A] = '-'; map[0x4E] = '+'; map[0x37] = '*'; map[0x35] = '/';
    return map[sc];
}

int main() {
    char expr[32] = {0};
    int pos = 0;
    unsigned char last_sc = 0;

    printf("Enter expression (e.g., 12+3). Press Enter to calculate:\n");

    while (1) {
        FILE *fp = fopen("/proc/kbd_irq", "r");
        if (!fp) {
            perror("fopen");
            return 1;
        }

        unsigned int sc = 0;
        fscanf(fp, "%u", &sc);
        fclose(fp);

        if (sc != last_sc) {
            last_sc = sc;
            char ch = scancode_to_ascii(sc);

            if (ch == '\n') {
                expr[pos] = '\0';
                int a, b;
                char op;

                if (sscanf(expr, "%d%c%d", &a, &op, &b) == 3) {
                    int result = 0;
                    switch (op) {
                        case '+': result = a + b; break;
                        case '-': result = a - b; break;
                        case '*': result = a * b; break;
                        case '/': result = (b != 0) ? a / b : 0; break;
                        default: printf("\nInvalid operator\n"); continue;
                    }
                    printf("\nResult: %d %c %d = %d\n", a, op, b, result);
                } else {
                    printf("\nInvalid expression: %s\n", expr);
                }

                pos = 0;
                memset(expr, 0, sizeof(expr));
            } else if (ch && pos < sizeof(expr) - 1) {
                expr[pos++] = ch;
                printf("%c", ch);
                fflush(stdout);
            }
        }

        usleep(100000);  // 100ms
    }

    return 0;
}

