#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>

int main(int argc, char **argv)
{
  if (isatty(fileno(stdout))) {
    openlog("forsh", LOG_PID, LOG_AUTH);
    syslog(LOG_NOTICE, "user %d attempted to login interactively", getuid());
    closelog();
    puts ("You are not allowed to login inteactively to this system. This incident will be reported.");
  }
  return EXIT_SUCCESS;
}
