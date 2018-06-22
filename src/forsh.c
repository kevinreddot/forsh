#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <pwd.h>
#include <linux/limits.h>
#include "config.h"

struct scp_info {
  char *path;
  int dir; // 1 = to, -1 = from, 0 = not scp command
  char *temp_str;
};

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

struct scp_info is_scp(char *ssh_command) {
  /* 
  When scp is requested, the command passed will be
  SSH_ORIGINAL_COMMAND=scp -t /tmp

  we first check, if it starts with "scp " and returns last token in
  SSH_ORIGINAL_COMMAND which is the path or NULL if this is not an scp call
  */
  char *token;
  char *sep = " ";
  struct scp_info scp;

  scp.dir = 0;
  if (strncmp(ssh_command, "scp ", 4) == 0) {
    if ((scp.temp_str = strdup(ssh_command)) != NULL)
      for (token = strtok(scp.temp_str, sep); token; token = strtok(NULL, sep)) {
        // When token = NULL, scp.path points to the last token
        scp.path = token; 
        if (strncmp(token, "-t", 2) == 0) 
          scp.dir = 1;
        else if (strncmp(token, "-f", 2) == 0)
          scp.dir = -1;
      }
  }
  // scp.dir - direction or 0
  // scp.path - path scp uses
  // scp.temp_str - pointer to duped string to free() later
  return scp;
}

void log_command(uid_t uid, char *ssh_command)
{
  char *login;
  struct scp_info scp;
  char dir[5];

  login = getenv("USER");
  scp = is_scp(ssh_command);
  if (scp.dir != 0) {
    if (scp.dir > 0)
      strncpy(dir, "to", 3);
    else
      strncpy(dir, "from", 5);
    if (login)
      syslog(LOG_NOTICE, "user %s (UID %d) attempted to copy files %s \"%s\" via scp", login, uid, dir, scp.path);
    else
      syslog(LOG_NOTICE, "user ID %d attempted to copy files %s \"%s\" via scp", uid, dir, scp.path);
    free (scp.temp_str);
  } else {
    if (login)
      syslog(LOG_NOTICE, "user %s (UID %d) attempted to run \"%s\"", login, uid, ssh_command);
    else
      syslog(LOG_NOTICE, "user ID %d attempted to run \"%s\"", uid, ssh_command);
  }
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
