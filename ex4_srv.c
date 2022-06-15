//Bar Tawil 209215490
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/wait.h>

int add(int first, int second) {
    return first + second;
}

int sub(int first, int second) {
    return first - second;
}

int multiplication(int first, int second) {
    return first * second;
}

float divide(int first, int second) {
    if (second == 0) {
        return 0;
    }
    return first / second;
}

int calc(int P2, int P3, int P4) {
    int ans;
    float tmp;
    switch (P3) {
        case 1:
            ans =  add(P2, P4);
            break;
        case 2:
            ans = sub(P2, P4);
            break;
        case 3:
            ans = multiplication(P2, P4);
            break;
        case 4:
            tmp = divide(P2, P4);
            ans = tmp;
            break;
        default:
            printf("ERROR_FROM_EX4\n");
            exit(0);
    }
    return ans;
}

void handlerFunc() {
    pid_t val;
    val = fork();
    if (val == 0) {
        // parse client file
        FILE *client_file;
        client_file = fopen("to_srv.txt", "r");
        if (client_file == NULL) {
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
        char line[100];
        int args[4];
        int i = 0;
        while (fgets(line, sizeof(line), client_file)) {
            args[i] = atoi(line);
            i++;
        }
        fclose(client_file);
        if (remove("to_srv.txt") != 0) {
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
        int ans = calc(args[1], args[2], args[3]);
        // send answer to client
        char file_name[100] = "to_client_";
        char client_pid[6];
        sprintf(client_pid, "%d", args[0]);
        strcat(file_name, client_pid);
        strcat(file_name, ".txt");
        FILE *server_file;
        server_file = fopen(file_name, "w");
        if (server_file == NULL) {
            printf("ERROR_FROM_EX4\n");
            exit(1);
        } else if (args[2] == 4 && args[3] == 0) {
            fputs("CANNOT_DIVIDE_BY_ZERO", server_file);
        } else {
            char tmp[100];
            sprintf(tmp, "%d", ans);
            fputs(tmp, server_file);
        }
        fclose(server_file);
        // send signal to client
        if (kill( args[0], SIGUSR2) == -1) {
            if (remove(file_name) != 0) {
                printf("ERROR_FROM_EX4\n");
                exit(1);
            }
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
        exit(0);
    }
}

void alarm_hand(int sig) {
    while (wait(NULL) != -1);
    printf("The server was closed because no service request was received for the last 60 seconds\n");
    exit(1);
}

int main() {
    signal(SIGUSR1, handlerFunc);
    signal(SIGALRM, alarm_hand);

    while (1) {
        alarm(60);
        pause();
        alarm(0);
    }
}