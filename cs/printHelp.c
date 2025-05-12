#include <stdio.h>

int main() {
    puts(
        "\n===== NETLAB SHELL HELP =====\n"
        "Copyright @ R. Aisha Syauqi Ramadhani - 2306250554\n"
        "-Use the shell at your own risk...\n"
        "\nList of Commands supported:\n"
        "> print {something}                      : Print something with style\n"
        "> buatdong {filename} {content}          : Write content to a file\n"
        "> bacadong {filename}                    : Read from a file\n"
        "> rahasiabanget {filename} {content}     : Write to a file (encrypted)\n"
        "> pembacapikiran {filename}              : Read from a file (encrypted)\n"
        "> itungwoi {add|sub|mul|div} num1 num2   : Do some calculation\n"
        "> help                                   : Show all commands in shell\n"
        "> exit                                   : Exit the shell\n"
        "> clear                                  : Clear the terminal screen\n"
        "> waktusekarang                          : Show current date and time\n"
        "> hitungram {filename}                   : Count lines, words, and characters in a file\n"
        "> list                                   : List the contents of the current directory\n"
        "> rename {oldname {newname}             : Rename a file or directory\n"
        "> hapusfile {filename}                   : Delete a file\n"
    );
    return 0;
}
