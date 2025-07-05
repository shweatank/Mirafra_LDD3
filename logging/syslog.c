#include <stdio.h>
#include <syslog.h>

int main() {
    // Open connection to system logger
    openlog("MyProgram", LOG_PID | LOG_CONS, LOG_USER);

    // Log different types of messages
    syslog(LOG_INFO, "This is an INFO message");
    syslog(LOG_WARNING, "This is a WARNING message");
    syslog(LOG_ERR, "This is an ERROR message");

    // Close connection
    closelog();

    return 0;
}
//options in openlog
/*LOG_PID	Include the process ID (PID) with each log message
LOG_CONS	If the system log fails, write the message to the system console
LOG_NDELAY	Open the connection to syslog immediately (not lazily)
LOG_ODELAY	(Default) Delay opening the connection until syslog() is actually called*/
//options for facility
/*LOG_USER	Default: messages from user programs
LOG_DAEMON	For background daemon processes
LOG_LOCAL0–7	Reserved for custom/local use
LOG_AUTH	Security/authorization messages
LOG_CRON	Cron daemon (scheduled jobs)
LOG_MAIL	Mail subsystem
LOG_SYSLOG	Messages from the syslog system
LOG_KERN	Kernel messages (only for kernel use)*/
//priority in sys log
/*LOG_EMERG	System is unusable
LOG_ALERT	Immediate action needed
LOG_CRIT	Critical condition (e.g., disk failure)
LOG_ERR	Standard error condition
LOG_WARNING	Warning, not necessarily an error
LOG_NOTICE	Normal, but important message
LOG_INFO	Informational message
LOG_DEBUG	Debug message, useful during development*/
