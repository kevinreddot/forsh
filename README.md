**forsh** stands for "forwarder's shell". It is intended to be used on ssh
jump hosts as a shell for users only doing port forwarding. It can also could
be in conjunction with `ForceCommand` directive of `sshd`.

It can be statically linked and easily be put in a chroot jail.  Effectively,
it is just `/bin/true` with some logging.  If a user attempts to login
interactively or run a command, **forsh** will log this attempt. If user just
requests port forwarding or stdin/stdout/stderr forwarding (as `ProxyJump` and
`-W` do), this is not considered a violation and nothing is logged.

## Configuration
Configuration is done at compile-time via `config.h` file. Edit it and
recompile. Options are:

- **RESOLVE_USERNAME** is used to allow `forsh` 
  to convert user ID of a logging-in user into username via `getpwuid()`
  function. You need a fully functioning NSS subsystem in order to
  successfully run `getpwuid()`. In a case of a chrooted
  environment this requires `nsswitch.conf` files along with the NSS
  libraries being used, possibly a real `passwd` file and/or other configuration and binary
  files like `sssd`-related stuff. Some of files might be sensitive, E.g.,
  `sssd.conf` may contain passwords for LDAP server. In this case you can
  simply undefine **RESOLVE_USERNAME** effectively disabling username
  resolution and **forsh** will only log user ID.

- **LOG_FACILITY** allows to change syslog(3) facility used by `forsh`.
  Default is **LOG_AUTH** which is compatible with the [default value for
  OpenSSH](https://man.openbsd.org/sshd_config#SyslogFacility).
  
## Usage
There are different ways to use **forsh** in your system, I will only
highlight some of them.

**WARNING:** modifying sshd configuration may lock you out of your bastion
host! You might break your `sshd_config`, in order to avoid such situation make
sure you have a way to revert things.

Assuming that your bastion host uses the centralizied
[name resolution service](https://en.wikipedia.org/wiki/Name_Service_Switch),
like LDAP, shell for every user will be set from it. To get around this,
if users only use the bastion host as ssh proxy, use the `ForceCommand` ssh
directive and set it to `/path/to/forsh`. For users requiring interactive
login, do not set `ForceCommand` or set it to `none`.

For example, if you have the group `users`, who should not login interactively to
the bastion host and group `admins`, who may need to login interactively, you
should use the following settings in `sshd_config` on the abstion host:

```
AllowGroups users admins
Match Group users
  ForceCommand /usr/local/bin/forsh
```

You do not need to set `ForceCommand` for `admins` group as by default it
equals to `none`, which allows sshd to execute shell on login.

Another example is, if all of your users are allowed to use the bastion host as a
proxy and the `admins` group should be permitted to login interactively, you may
put `ForceCommand` in the global section of `sshd_config` and make an exception in
the match block for admins:

```
ForceCommand /usr/local/bin/forsh
Match Group admins
  ForceCommand none
```

That will be enough for the most uses.

If for some reason you want additional security, you might run **forsh**
from a chroot() jail. To do so, compile **forsh** as a static binary (add
`-static` option to gcc in the `Makefile`). I also recommend to unset
**RESOLVE_USERNAME** option in `config.h` in this case. Otherwise you will have to put all
NSS configuration into your chroot jail, including credentials to access your
centralized name service. The result of disabling **RESOLVE_USERNAME** is that
only uid and not login name will be logged.  

To setup your chroot jail, check the appropriate manual for your distribution.

Then, set `ChrootDirectory` directive in `sshd_config` like this:

```
ChrootDirectory /var/chroot/forsh
ForceCommand /bin/forsh
Match Group admins
  ChrootDirectory none
  ForceCommand none
```

## Links

- https://news.ycombinator.com/item?id=12206628
- https://en.wikibooks.org/wiki/OpenSSH/Cookbook/Proxies_and_Jump_Hosts
