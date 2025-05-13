#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define MAX_INTERFACES 20

typedef struct {
    char name[IFNAMSIZ];
    char ip_addr[INET6_ADDRSTRLEN];
    char mac_addr[18];
    long rx_bytes;
    long tx_bytes;
    int is_up;
    int is_ipv6;
} Interface;

Interface interfaces[MAX_INTERFACES];
int interface_count = 0;

void print_help() {
    printf("\n===== NETWOI HELP =====\n");
    printf("Usage: netwoi [interface]\n");
    printf("Description: Display network information\n\n");
    printf("Flags:\n");
    printf("  -a: Show all interfaces\n");
    printf("  -i: Show IP addresses only\n");
    printf("  -s: Show connection statistics\n");
    printf("  -m: Show MAC addresses\n");
    printf("  -h, --help: Show this help message\n\n");
    printf("Examples:\n");
    printf("  netwoi: Show main interface information\n");
    printf("  netwoi eth0: Show information for eth0\n");
    printf("  netwoi -a: Show all interfaces\n");
    printf("  netwoi -im: Show IP and MAC addresses for all interfaces\n");
}

// Get MAC address for an interface
void get_mac_address(const char *interface_name, char *mac_addr, size_t size) {
    struct ifreq ifr;
    int s;
    
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        snprintf(mac_addr, size, "Unknown");
        return;
    }
    
    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ);
    if (ioctl(s, SIOCGIFHWADDR, &ifr) == -1) {
        snprintf(mac_addr, size, "Unknown");
        close(s);
        return;
    }
    
    unsigned char *hwaddr = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    snprintf(mac_addr, size, "%02X:%02X:%02X:%02X:%02X:%02X",
            hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
    
    close(s);
}

// Get statistics for an interface
void get_interface_stats(const char *interface_name, long *rx_bytes, long *tx_bytes, int *is_up) {
    char path[256];
    FILE *file;
    
    // Check if interface is up
    snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", interface_name);
    file = fopen(path, "r");
    if (file) {
        char state[20];
        if (fgets(state, sizeof(state), file)) {
            *is_up = (strncmp(state, "up", 2) == 0);
        } else {
            *is_up = 0;
        }
        fclose(file);
    } else {
        *is_up = 0;
    }
    
    // Get received bytes
    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/rx_bytes", interface_name);
    file = fopen(path, "r");
    if (file) {
        fscanf(file, "%ld", rx_bytes);
        fclose(file);
    } else {
        *rx_bytes = 0;
    }
    
    // Get transmitted bytes
    snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/tx_bytes", interface_name);
    file = fopen(path, "r");
    if (file) {
        fscanf(file, "%ld", tx_bytes);
        fclose(file);
    } else {
        *tx_bytes = 0;
    }
}

// Get all network interfaces
void get_interfaces() {
    struct ifaddrs *ifaddr, *ifa;
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }
    
    for (ifa = ifaddr; ifa != NULL && interface_count < MAX_INTERFACES; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        
        // Check if we already have this interface
        int found = 0;
        for (int i = 0; i < interface_count; i++) {
            if (strcmp(interfaces[i].name, ifa->ifa_name) == 0) {
                found = 1;
                break;
            }
        }
        
        if (found) {
            continue;
        }
        
        // Add new interface
        if (ifa->ifa_addr->sa_family == AF_INET || ifa->ifa_addr->sa_family == AF_INET6) {
            strncpy(interfaces[interface_count].name, ifa->ifa_name, IFNAMSIZ);
            
            // Get IP address
            void *addr_ptr;
            if (ifa->ifa_addr->sa_family == AF_INET) {
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)ifa->ifa_addr;
                addr_ptr = &ipv4->sin_addr;
                interfaces[interface_count].is_ipv6 = 0;
            } else {
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ifa->ifa_addr;
                addr_ptr = &ipv6->sin6_addr;
                interfaces[interface_count].is_ipv6 = 1;
            }
            
            inet_ntop(ifa->ifa_addr->sa_family, addr_ptr, 
                     interfaces[interface_count].ip_addr, INET6_ADDRSTRLEN);
            
            // Get MAC address
            get_mac_address(ifa->ifa_name, interfaces[interface_count].mac_addr, 18);
            
            // Get statistics
            get_interface_stats(ifa->ifa_name, 
                               &interfaces[interface_count].rx_bytes,
                               &interfaces[interface_count].tx_bytes,
                               &interfaces[interface_count].is_up);
            
            interface_count++;
        }
    }
    
    freeifaddrs(ifaddr);
}

// Format bytes to human readable format
void format_bytes(long bytes, char *buffer, size_t buffer_size) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = bytes;
    
    while (size > 1024 && unit_index < 4) {
        size /= 1024;
        unit_index++;
    }
    
    snprintf(buffer, buffer_size, "%.2f %s", size, units[unit_index]);
}

int main(int argc, char *argv[]) {
    int show_all = 0;
    int show_ip_only = 0;
    int show_stats = 0;
    int show_mac = 0;
    char *specified_interface = NULL;
    
    // Check for help flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
    }
    
    // Parse flags and arguments
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // It's a flag
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'a': show_all = 1; break;
                    case 'i': show_ip_only = 1; break;
                    case 's': show_stats = 1; break;
                    case 'm': show_mac = 1; break;
                    case 'h': print_help(); return 0;
                    default:
                        fprintf(stderr, "Unknown flag: %c\n", argv[i][j]);
                        print_help();
                        return 1;
                }
            }
        } else {
            specified_interface = argv[i];
        }
    }
    
    // Get interfaces
    get_interfaces();
    
    if (interface_count == 0) {
        printf("No network interfaces found\n");
        return 1;
    }
    
    // Print interfaces
    for (int i = 0; i < interface_count; i++) {
        // Skip if not showing all and not the specified interface and not the first if none specified
        if (!show_all && 
            specified_interface != NULL && 
            strcmp(interfaces[i].name, specified_interface) != 0 &&
            !(specified_interface == NULL && i == 0)) {
            continue;
        }
        
        if (show_ip_only) {
            // Only show IP address
            printf("%s: %s\n", interfaces[i].name, interfaces[i].ip_addr);
        } else {
            // Show interface info
            printf("\n--- %s ---\n", interfaces[i].name);
            printf("Status: %s\n", interfaces[i].is_up ? "UP" : "DOWN");
            printf("IP Address: %s (%s)\n", interfaces[i].ip_addr, 
                  interfaces[i].is_ipv6 ? "IPv6" : "IPv4");
            
            if (show_mac) {
                printf("MAC Address: %s\n", interfaces[i].mac_addr);
            }
            
            if (show_stats) {
                char rx_buffer[20], tx_buffer[20];
                format_bytes(interfaces[i].rx_bytes, rx_buffer, sizeof(rx_buffer));
                format_bytes(interfaces[i].tx_bytes, tx_buffer, sizeof(tx_buffer));
                
                printf("Received: %s\n", rx_buffer);
                printf("Transmitted: %s\n", tx_buffer);
            }
        }
    }
    
    return 0;
}