#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

void print_help() {
    printf("Simple calculator to add, subtract, multiply, or divide two numbers.\n");
    printf("Usage: itungwoi {add|sub|mul|div} num1 num2\n");
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
}

int main(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Check for -h / --help
    while ((opt = getopt_long(argc, argv, "h", long_options, NULL)) != -1) {
        if (opt == 'h') {
            print_help();
            return 0;
        }
    }

    // After getopt, optind points to the next argument
    if (argc - optind < 3) {
        printf("Usage: itungwoi {add|sub|mul|div} num1 num2\n");
        return 1;
    }

    char *operation = argv[optind];
    char *num1_str = argv[optind + 1];
    char *num2_str = argv[optind + 2];

    float num1 = atof(num1_str);
    float num2 = atof(num2_str);
    float result;

    if (strcmp(operation, "add") == 0) {
        result = num1 + num2;
        printf("%.2f\n", result);
    } else if (strcmp(operation, "sub") == 0) {
        result = num1 - num2;
        printf("%.2f\n", result);
    } else if (strcmp(operation, "mul") == 0) {
        result = num1 * num2;
        printf("%.2f\n", result);
    } else if (strcmp(operation, "div") == 0) {
        if (num2 == 0) {
            printf("Error: Division by zero\n");
            return 1;
        }
        result = num1 / num2;
        printf("%.2f\n", result);
    } else {
        printf("Invalid operation. Use add, sub, mul, or div\n");
        return 1;
    }

    return 0;
}
