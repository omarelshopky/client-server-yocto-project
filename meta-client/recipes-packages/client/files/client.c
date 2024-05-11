#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MESSAGE "Hello from Yocto"
#define CONFIG_FILE_PATH "/etc/client/client.conf"
#define MAX_LINE_LENGTH 1024

char server_ip[15] = "";
uint16_t server_port = 0;

void read_config_file();
void update_config_file(const char *new_server_ip, uint16_t new_server_port);
void establish_client_connection();
void print_app_usage(char *app_name);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (argc == 2 && strcmp(argv[1], "-h")) {
            print_app_usage(argv[0]);
        }
        else if (argc != 5 || strcmp(argv[1], "--ip") != 0 || strcmp(argv[3], "--port") != 0) {
            print_app_usage(argv[0]);
            return 1;
        }

        update_config_file(argv[2], atoi(argv[4]));
        printf("Updated config with SERVER_IP: %s, SERVER_PORT: %s\n", argv[2], argv[4]);

        return 0;
    }

    while (1) {
        read_config_file();

        if (server_ip != "" && server_port != 0) {
            break;
        }

        printf("ERROR: please, set the server ip and port\n");
        sleep(5);
    }

    establish_client_connection();

    return 0;
}

void read_config_file() {
    FILE *file = fopen(CONFIG_FILE_PATH, "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, " ");

        if (token != NULL) {
            if (strcmp(token, "SERVER_IP") == 0) {
                token = strtok(NULL, " ");

                if (token != NULL) {
                    strcpy(server_ip, token);
                    // Remove trailing newline character if present
                    strtok(server_ip, "\n");
                }
            } else if (strcmp(token, "SERVER_PORT") == 0) {
                token = strtok(NULL, " ");

                if (token != NULL) {
                    server_port = atoi(token);
                }
            }
        }
    }

    fclose(file);
}

void establish_client_connection() {
    struct sockaddr_in server_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        send(sock, MESSAGE, strlen(MESSAGE), 0);
        sleep(5);
    }

    close(sock);
}

void print_app_usage(char *app_name) {
    printf("Usage: %s --ip <SERVER_IP> --port <SERVER_PORT>\n", app_name);
}

void update_config_file(const char *new_server_ip, uint16_t new_server_port) {
    FILE *file = fopen(CONFIG_FILE_PATH, "w");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    fprintf(file, "SERVER_IP %s\n", new_server_ip);
    fprintf(file, "SERVER_PORT %d", new_server_port);

    fclose(file);
}