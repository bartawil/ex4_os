//Bar Tawil 209215490
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>

int doneFlag = 0;

void clearFiles() {
    if (access("to_srv.txt", F_OK) == 0) {
        if (remove("to_srv.txt") != 0) {
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
    }
}

void handlerFunc() {
    doneFlag = 1;
    char file_name[100] = "to_client_";
    char client_pid[6];
    sprintf(client_pid, "%d", (int)getpid());
    strcat(file_name, client_pid);
    strcat(file_name, ".txt");
    FILE *file_to_client;
    file_to_client = fopen(file_name, "r");
    if (file_to_client == NULL) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    char line[100];
    fgets(line, sizeof(line), file_to_client);
    printf("%s\n", line);
    fclose(file_to_client);
    if (remove(file_name) != 0) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
}

void alarm_hand(int sig) {
    signal(SIGALRM, alarm_hand);
    atexit(clearFiles);
    printf("Client closed because no response was received from the server for 30 seconds\n");
    exit(1);
}

int main(int argc, char** argv) {
    // check if there is enough argument
    if (argc != 5) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }
    // check if another client exist
    int counter = 1; // count till 10
    while (!access("to_srv.txt", F_OK)) {
        if (counter == 10) {
            printf("ERROR_FROM_EX4\n");
            exit(1);
        }
        int r = rand() % 5  + 1;
        sleep(r);
        counter++;
    }
    // create file to server
    FILE *file_to_srv;
    file_to_srv = fopen("to_srv.txt", "w");
    if (file_to_srv == NULL) {
        printf("ERROR_FROM_EX4\n");
        exit(1);
    } else {
        char client_pid[6];
        sprintf(client_pid, "%d", (int)getpid());
        fputs(client_pid, file_to_srv);
        fputs("\n", file_to_srv);
        int i;
        for (i = 2; i < argc; i++) {
            fputs(argv[i], file_to_srv);
            fputs("\n", file_to_srv);
        }
    }
    fclose(file_to_srv);

    // send signal to server
    int srv_pid = atoi(argv[1]);
    if (kill(srv_pid, SIGUSR1) == -1) {
        atexit(clearFiles);
        printf("ERROR_FROM_EX4\n");
        exit(1);
    }

    signal(SIGUSR2, handlerFunc);
    signal(SIGALRM, alarm_hand);

    while (!doneFlag) {
        alarm(30);
        pause();
        alarm(0);
    }

    return 0;
}