#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void play_audio(const char *result) {
    if (strcmp(result, "WON") == 0)
        system("aplay won.wav");
    else if (strcmp(result, "LOST") == 0)
        system("aplay lost.wav");
}
void execute_and_print(const char *command)
{
    printf("Executing command: %s\n", command);
    int ret = system(command);
    if (ret == -1)
    {
        perror("system command failed");
    }
    else
    {
        printf("Command exited with status: %d\n", WEXITSTATUS(ret));
    }
}
int main() {

    char buf[128] = {0};

  
    // Prompt user
    char guess;
    int ret;
    printf("Enter your guess (1–4): ");
    scanf(" %c", &guess);
    // Send guess to driver

    int fd = open("/dev/custom_uart_game", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    printf("--- Network Simulation Demonstration ---\n");

    execute_and_print("sudo insmod network_driver.ko");
    sleep(1); 

    execute_and_print("ping 192.168.0.57 -c 3");

    printf("\n--- Kernel Log for dummy_net ---\n");
    execute_and_print("dmesg | grep dummy_net");
    printf("--- End Kernel Log ---\n");

    execute_and_print("sudo ip link set dummy0 down");
    execute_and_print("sudo rmmod network_driver");

    printf("--- Network Simulation Complete ---\n\n");


    write(fd, &guess, 1);
    memset(buf, 0, sizeof(buf));
    // Wait for result
    read(fd, buf, sizeof(buf));
    printf("[Driver]: %s", buf);

   // Play audio
    if ((strcmp(buf,"WON")==0))
    {
        play_audio("WON");
    }
    else
    {
        play_audio("LOST");
    }

    close(fd);
    return 0;
}

