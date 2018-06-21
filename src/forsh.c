#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <pwd.h>
#include "config.h"

void log_user(uid_t uid) 
{
  char *login;

  login = getenv("USER");
  if (login)
    syslog(LOG_NOTICE, "user %s (UID %d) attempted to login interactively", login, uid);
  else
    syslog(LOG_NOTICE, "user ID %d attempted to login interactively", uid);
  closelog();
  return;
}

void log_command(uid_t uid, char *ssh_command)
{
  char *login;

  login = getenv("USER");
  if (login)
    syslog(LOG_NOTICE, "user %s (UID %d) attempted to run \"%s\"", login, uid, ssh_command);
  else
    syslog(LOG_NOTICE, "user ID %d attempted to run \"%s\"", uid, ssh_command);
  return;
}

int main()
{
  char *ssh_command;
  uid_t uid;

  /*

  forsh is intended to be run via ForceCommand directive in sshd_config. It will not run if only forwarding (-W or ProxyJump) is
  requested. So, if it IS run, it means either interactive login or remote command execution is requested.

  If remote command execution is requested, sshd will set SSH_ORIGINAL_COMMAND variable and then will run what is defined via
  ForceCommand (or shell, passing original command via -c switch). Hence, to distinguish between interactive login and remote
  command execution we check for SSH_ORIGINAL_COMMAND variable.

  */

  uid = getuid();
  openlog("forsh", LOG_PID, LOG_FACILITY);
  ssh_command = getenv("SSH_ORIGINAL_COMMAND");
  if (ssh_command)
    log_command(uid, ssh_command);
  else
    log_user(uid);
  closelog();
  puts ("You are not allowed to login to this system. This incident will be reported.");
  return EXIT_SUCCESS;
}
