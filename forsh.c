#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <pwd.h>
#include "config.h"

void log_user(char *ssh_command) 
{
  uid_t uid;
  struct passwd *pw;

  openlog("forsh", LOG_PID, LOG_FACILITY);
  uid = getuid();
#ifdef RESOLVE_USERNAME
  pw = getpwuid(uid);
  if (pw == NULL) {
    syslog(LOG_ERR, "uname to get user name for UID %d. Aborting!", uid);
    closelog();
    abort();
  }
  if (ssh_command) 
    syslog(LOG_NOTICE, "user %s (UID %d) attempted to run \"%s\"", pw->pw_name, uid, ssh_command);
  else
    syslog(LOG_NOTICE, "user %s (UID %d) attempted to login interactively", pw->pw_name, uid);
#else
  if (ssh_command)
    syslog(LOG_NOTICE, "user ID %d attempted to run \"%s\"", uid, ssh_command);
  else
    syslog(LOG_NOTICE, "user ID %d attempted to login interactively", uid);
#endif
  closelog();
}

int main(int argc, char **argv)
{
  char *ssh_command;

  /*

  forsh is intended to be run via ForceCommand directive in sshd_config. It will not run if only forwarding (-W or ProxyJump) is
  requested. So, if it IS run, it means either interactive login or remote command execution is requested.

  If remote command execution is requested, sshd will set SSH_ORIGINAL_COMMAND variable and then will run what is defined via
  ForceCommand (or shell, passing original command via -c switch). Hence, to distinguish between interactive login and remote
  command execution we check for SSH_ORIGINAL_COMMAND variable.

  */

  ssh_command = getenv("SSH_ORIGINAL_COMMAND");
  log_user(ssh_command);
  puts ("You are not allowed to login to this system. This incident will be reported.");
  return EXIT_SUCCESS;
}
