/* posix_timer_signal.c - demo of POSIX timer_create + SIGRTMIN */

/* Standard I/O */
#include <stdio.h>              /* printf, perror */

/* Standard utilities */
#include <stdlib.h>             /* exit */

/* Signal definitions */
#include <signal.h>             /* sigaction, siginfo_t */

/* Time / timer APIs */
#include <time.h>               /* timer_create, timer_settime, struct itimerspec */

/* For memset */
#include <string.h>             /* memset */

/* Handler for real-time signal */
static void rt_handler(int sig, siginfo_t *si, void *uc) {
    (void)uc; /* unused in this demo */
    /* si->si_value contains user-supplied data (if any) */
    printf("POSIX Timer expired: signal=%d, si_value.sival_int=%d\n",
           sig, si->si_value.sival_int);
}

int main(void) {
    struct sigaction sa;                /* for registering signal handler */
    timer_t timerid;                    /* holds the created timer ID */
    struct sigevent sev;                 /* to describe how timer notifies */
    struct itimerspec its;               /* to set initial and interval times */
    int ret;

    /* Setup signal action to handle the realtime signal (SIGRTMIN) */
    memset(&sa, 0, sizeof(sa));          /* clear structure */
    sa.sa_flags = SA_SIGINFO;            /* get extended info in handler */
    sa.sa_sigaction = rt_handler;        /* handler function */

    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* Specify notification via signal (SIGRTMIN). We can attach a data value. */
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_SIGNAL;     /* notify by signal */
    sev.sigev_signo = SIGRTMIN;          /* use realtime signal SIGRTMIN */
    sev.sigev_value.sival_int = 42;      /* arbitrary user data passed to handler */

    /* Create the timer associated with CLOCK_MONOTONIC */
    if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    /* Set timer: first expiry after 1.5s, then periodic every 2s */
    its.it_value.tv_sec = 1;             /* initial expiration seconds */
    its.it_value.tv_nsec = 500000000L;   /* initial expiration nanoseconds = 0.5s */

    its.it_interval.tv_sec = 2;          /* interval seconds */
    its.it_interval.tv_nsec = 0;         /* interval nanoseconds */

    /* Arm the timer */
    ret = timer_settime(timerid, 0, &its, NULL);
    if (ret == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    printf("POSIX timer set: first in 1.5s, then every 2s. Press Ctrl+C to exit.\n");

    /* Wait indefinitely; the signal handler will print messages */
    while (1) {
        pause();                         /* sleep until signal arrives */
    }

    /* Cleanup (never reached in this example) */
    /* timer_delete(timerid); */

    return 0;
}

