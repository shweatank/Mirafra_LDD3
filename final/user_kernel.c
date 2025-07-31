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

int main() {
    int fd = open("/dev/custom_uart_game", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    char buf[128] = {0};

    // Wait for random number signal
    printf("Waiting for random number from System B...\n");
    read(fd, buf, sizeof(buf));  // Block until driver receives number
  
    // Prompt user
    char guess;
    int ret;
    char command[128];
    char msg[]="ENTER GUESS";
    snprintf(command,sizeof(command),"./a.out %s",msg);
    if(fork()==0)
    {
	    char *argv[]={"./a.out",msg,NULL};
	    execv("./a.out",argv);
    }
    printf("Enter your guess (0–9): ");
    scanf(" %c", &guess);
    // Send guess to driver
    write(fd, &guess, 1);
    printf("Random Number: %s", buf);
    // Wait for result
    memset(buf, 0, sizeof(buf));
    read(fd, buf, sizeof(buf));
    printf("[Driver]: %s", buf);

   // Play audio
    if ((strcmp(buf,"WON")==0))
    {
        play_audio("WON");
       strcpy(msg,"WON");
       if(fork()==0)
       {
	       char *argv[]={"./a.out",msg,NULL};
	       execv("./a.out",argv);
       }
    }
    else
    {
        play_audio("LOST");

       strcpy(msg,"LOSS");
       if(fork()==0)
       {
	       char *argv[]={"./a.out",msg,NULL};
	       execv("./a.out",argv);
       }
    }
       sleep(2);
       strcpy(msg,"             ");
       if(fork()==0)
       {
	       char *argv[]={"./a.out",msg,NULL};
	       execv("./a.out",argv);
       }
    

    close(fd);
    return 0;
}

