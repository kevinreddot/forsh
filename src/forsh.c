#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <pwd.h>
#include <assert.h>
#include "config.h"

struct scp_info
{
  char *path, *temp_str;
  int dir;                              // 1 = to, -1 = from, 0 = not scp command
};

struct scp_info is_scp(char *ssh_command)
{
  assert(ssh_command);
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
  if (strncmp(ssh_command, "scp ", 4) == 0)
  {
    if ((scp.temp_str = strdup(ssh_command)) != NULL)
      for (token = strtok(scp.temp_str, sep); token; token = strtok(NULL, sep))
      {
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
  assert(scp.dir == 0 || scp.dir == 1 || scp.dir == -1);
  return scp;
}

int is_sftp(char *ssh_command)
{
  assert(ssh_command);
  /*
     When sftp is requested, sshd launches sftp-server configured via Subsystem
     directive in sshd_config. Environment looks like this:

     SSH_ORIGINAL_COMMAND=/usr/lib/openssh/sftp-server

     Because location of sftp-server is OS-specific, we have it configured via
     config.h.
   */
  return !strncmp(ssh_command, SFTP_LOCATION, strlen(SFTP_LOCATION));
}

void log_user(uid_t uid)
{
  char *login;

  login = getenv("LOGNAME");
  assert(login);
  syslog(LOG_NOTICE, "user %s (UID %d) attempted to login interactively", login, uid);
  return;
}

void log_command(uid_t uid, char *ssh_command)
{
  assert(ssh_command);

  char *login;
  struct scp_info scp;
  char dir[5];

  login = getenv("LOGNAME");
  assert(login);
  scp = is_scp(ssh_command);
  if (scp.dir != 0)                     // Check, if this is scp
  {
    if (scp.dir > 0)
      strncpy(dir, "to", 3);
    else
      strncpy(dir, "from", 5);
    syslog(LOG_NOTICE, "user %s (UID %d) attempted to copy files %s \"%s\" via scp", login, uid, dir, scp.path);
  }
  else if (is_sftp(ssh_command))        // Check, if this is sftp
    syslog(LOG_NOTICE, "user %s (UID %d) attempted to copy files via sftp", login, uid);
  else                                  // if this is neither scp not sftp, log command as-is
    syslog(LOG_NOTICE, "user %s (UID %d) attempted to run \"%s\"", login, uid, ssh_command);
  free(scp.temp_str);
  return;
}

int main()
{
  char *ssh_command;
  uid_t uid;

  /* 
     forsh is intended to be run via ForceCommand directive in sshd_config. It
     will not run if only forwarding (-W or ProxyJump) is requested. So, if it
     IS run, it means either interactive login or remote command execution is
     requested.

     If remote command execution is requested, sshd will set
     SSH_ORIGINAL_COMMAND variable and then will run what is defined via
     ForceCommand (or shell, passing original command via -c switch). Hence, to
     distinguish between interactive login and remote command execution we
     check for SSH_ORIGINAL_COMMAND variable.
   */

  uid = getuid();
  openlog("forsh", LOG_PID, LOG_FACILITY);
  ssh_command = getenv("SSH_ORIGINAL_COMMAND");
  if (ssh_command)
    log_command(uid, ssh_command);
  else
    log_user(uid);
  closelog();
  puts("You are not allowed to login to this system. This incident will be reported.");
  return EXIT_SUCCESS;
}
