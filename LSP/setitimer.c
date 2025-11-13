/* setitimer_example.c - demo of ITIMER_REAL + SIGALRM */

/* Include standard I/O functions */
#include <stdio.h>          /* printf, perror */

/* Include general purpose utilities */
#include <stdlib.h>         /* exit */

/* Include signal handling definitions */
#include <signal.h>         /* sigaction, SIGALRM */

/* Include timeval and itimerval structures */
#include <sys/time.h>       /* setitimer, struct itimerval */

/* Include sleep/usleep */
#include <unistd.h>         /* pause */

/* Signal handler for SIGALRM */
static void alarm_handler(int signum) {               /* handler receives signal number */
    /* Print a message to demonstrate we've been called */
    printf("SIGALRM received (signum=%d) — periodic task executed\n", signum);
    /* Note: avoid complex non-reentrant logic in real handlers */
}

int main(void) {
    struct sigaction sa;               /* structure to specify signal action */
    struct itimerval timer;            /* structure for timer value and interval */

    /* Zero-initialize sigaction structure (good practice) */
    sigemptyset(&sa.sa_mask);          /* no signals blocked during handler */
    sa.sa_flags = 0;                   /* default flags (no SA_RESTART here) */
    sa.sa_handler = alarm_handler;     /* set our handler function */

    /* Register SIGALRM handler using sigaction */
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction");           /* print error and exit if registration fails */
        exit(EXIT_FAILURE);
    }

    /* Configure the timer:
     *  - timer.it_value is first expiration time
     *  - timer.it_interval is interval for periodic timer after first expiry
     */
    timer.it_value.tv_sec = 1;         /* first expiration after 1 second */
    timer.it_value.tv_usec = 0;        /* microseconds = 0 */

    timer.it_interval.tv_sec = 2;      /* then every 2 seconds */
    timer.it_interval.tv_usec = 0;     /* microseconds = 0 */

    /* Start the real (wall-clock) timer. When it expires SIGALRM is raised. */
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");           /* print error and exit if setitimer fails */
        exit(EXIT_FAILURE);
    }

    /* Main program loop: pause() sleeps until a signal arrives */
    printf("Timer set: first expiry in 1s, then every 2s. Press Ctrl+C to exit.\n");
    while (1) {
        pause();                        /* wait for signals (like SIGALRM) */
    }

    return 0;                           /* never reached */
}

