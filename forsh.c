#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <pwd.h>
#include "config.h"

int main(int argc, char **argv)
{
  uid_t uid;
  struct passwd *pw;

  if (isatty(fileno(stdout))) {
    openlog("forsh", LOG_PID, LOG_AUTH);
    uid = getuid();
#ifdef RESOLVE_USERNAME
    pw = getpwuid(uid);
    if (pw == NULL) {
      syslog(LOG_ERR, "uname to get user name for UID %d. Aborting!", uid);
      closelog();
      abort();
    }
    syslog(LOG_NOTICE, "user %s (UID %d) attempted to login interactively", pw->pw_name, uid);
#else
    syslog(LOG_NOTICE, "user ID %d attempted to login interactively", uid);
#endif
    closelog();
    puts ("You are not allowed to login inteactively to this system. This incident will be reported.");
  }
  return EXIT_SUCCESS;
}
