#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_EXPR_LEN 100

// Global debounce tracking
int last_scancode = -1;
time_t last_time = 0;

// Map scancode to character
char map_scancode_to_char(int sc) {
    switch(sc) {
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';
        case 0x0C: return '+';   // Plus
        case 0x0D: return '-';   // Minus
        case 0x35: return '/';   // Slash
        case 0x37: return '*';   // Asterisk (Numpad *)
        case 0x1C: return '\n';  // Enter
        default: return 0;
    }
}

// Evaluate a simple left-to-right expression without precedence
int eval_expression(const char *expr) {
    int result = 0;
    int i = 0;
    char op = '+';

    if (!isdigit(expr[0])) {
        printf("Invalid expression format\n");
        return 0;
    }

    while (expr[i] != '\0') {
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        if (isdigit(expr[i])) {
            int num = expr[i] - '0';
            switch(op) {
                case '+': result += num; break;
                case '-': result -= num; break;
                case '*': result *= num; break;
                case '/':
                    if (num == 0) {
                        printf("Error: Division by zero\n");
                        return 0;
                    }
                    result /= num;
                    break;
                default:
                    printf("Unknown operator: %c\n", op);
                    return 0;
            }
        } else if (strchr("+-*/", expr[i])) {
            op = expr[i];
        } else {
            printf("Invalid character in expression\n");
            return 0;
        }

        i++;
    }

    return result;
}

int main() {
    FILE *fp = popen("dmesg -w", "r");
    if (!fp) {
        perror("popen");
        return 1;
    }

    char line[256];
    char expression[MAX_EXPR_LEN] = {0};
    int expr_len = 0;

    printf("🧮 Calculator started. Type keys, press [Enter] to evaluate.\n");

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "Keyboard IRQ: key press scancode =")) {
            int scancode;
            if (sscanf(line, " %*[^=]= 0x%x", &scancode) == 1) {
                time_t now = time(NULL);

                // Debounce: skip same scancode within 1 sec
                if (scancode == last_scancode && (now - last_time) < 1) {
                    continue;
                }

                last_scancode = scancode;
                last_time = now;

                char c = map_scancode_to_char(scancode);
                if (c) {
                    if (c == '\n') {
                        if (expr_len > 0) {
                            // Validate: must start and end with digit
                            if (!isdigit(expression[0]) || !isdigit(expression[expr_len - 1])) {
                                printf("\nInvalid expression: must start and end with digit\n");
                            } else {
                                printf("\nExpression: %s\n", expression);
                                int result = eval_expression(expression);
                                printf("Result: %d\n\n", result);
                            }

                            // Reset
                            expr_len = 0;
                            expression[0] = '\0';
                        }
                    } else {
                        if (expr_len < MAX_EXPR_LEN - 1) {
                            expression[expr_len++] = c;
                            expression[expr_len] = '\0';
                            printf("Pressed: %c | Expression: %s\n", c, expression);
                        }
                    }
                }
            }
        }
    }

    pclose(fp);
    return 0;
}

