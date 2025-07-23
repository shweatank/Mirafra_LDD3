#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// ---------- OS Detection ----------
#if defined(_WIN32) || defined(_WIN64)
    #define OS_NAME "Windows"
#elif defined(_linux_)
    #define OS_NAME "Linux"
#elif defined(_APPLE) && defined(MACH_)
    #define OS_NAME "macOS"
#else
    #define OS_NAME "Unknown OS"
#endif

// ---------- Architecture Detection ----------
#if defined(_x86_64_) || defined(_M_X64)
    #define ARCH_NAME "x86_64 (64-bit)"
#elif defined(_i386_) || defined(_M_IX86)
    #define ARCH_NAME "x86 (32-bit)"
#elif defined(_arm_) || defined(_M_ARM)
    #define ARCH_NAME "ARM (32-bit)"
#elif defined(_aarch64_)
    #define ARCH_NAME "ARM64 (64-bit)"
#elif defined(_16BIT_) || defined(_M_I86)
    #define ARCH_NAME "16-bit"
#else
    #define ARCH_NAME "Unknown Architecture"
#endif

// ---------- Function to Detect Endianness ----------
const char* detect_endianness() {
    union {
        uint32_t i;
        uint8_t c[4];
    } endian_test = { 0x01020304 };

    if (endian_test.c[0] == 0x04) return "Little Endian";
    if (endian_test.c[0] == 0x01) return "Big Endian";
    return "Unknown Endian";
}

// ---------- String Reversal (Generic Function) ----------
void reverse_string(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    for (size_t i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

// ---------- Main ----------
int main() {
    printf("==== Platform Independent Program ====\n");
    printf("Operating System  : %s\n", OS_NAME);
    printf("Architecture      : %s\n", ARCH_NAME);
    printf("Pointer size      : %zu bits\n", sizeof(void*) * 8);
    printf("Endianness        : %s\n", detect_endianness());

    printf("Integer Type Sizes:\n");
    printf("  int8_t   : %zu bytes\n", sizeof(int8_t));
    printf("  int16_t  : %zu bytes\n", sizeof(int16_t));
    printf("  int32_t  : %zu bytes\n", sizeof(int32_t));
    printf("  int64_t  : %zu bytes\n", sizeof(int64_t));

    int32_t a = 12345;
    int64_t b = 98765432101234LL;
    printf("\na = %" PRId32 "\n", a);
    printf("b = %" PRId64 "\n", b);

    char buffer[128];
    printf("\nEnter a string to reverse: ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
        reverse_string(buffer);
        printf("Reversed string: %s\n", buffer);
    }

    return 0;
}
