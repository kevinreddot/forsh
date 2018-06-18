**forsh** is "forwarder's shell". It is intended to be used on ssh jump hosts
as a shell for users only doing port forwarding. It also could be in
conjunction with `ForceCommand` directive of `sshd`.

It could be statically linked so it could easily be put in a chroot jail.
Effectively it is just `/bin/true` with some logging.  Upon login it will just
exit. If login has terminal allocated, it will log this event as it would
mean, the user tried to login interactively.

## Configuration
Configuration is compile-time and is done via `config.h` file. Edit it and
recompile. Options are:

- **RESOLVE_USERNAME** is used to control, if `forsh` will call `getpwuid()`
  to convert user ID of a logging-in user into username. To successfully run
  `getpwuid()` one need NSS subsystem fully functioning. In a case of chrooted
  environment this requires `nsswitch.conf` files together with used NSS
  libraries, possibly real `passwd` file and/or other configuration and binary
  files like `sssd`-related stuff. Those files might be sensitive (i.e.,
  `sssd.conf` may contain passwords for LDAP) and you may decide you don't
  want to have them in the chrooted environment. In this case you may disable
  username resolution and `forsh` will only log user ID.

- **LOG_FACILITY** allows to change syslog(3) facility used by `forsh`.
  Default is **LOG_AUTH** which is compatible with the [default value for
  OpenSSH](https://man.openbsd.org/sshd_config#SyslogFacility).
  
## Usage
There are different ways to use **forsh** in your system, I will only
highlight some of them.

**WARNING:** modifying sshd configuration may lock you out of your bastion
host! Make sure, you have a way to revert things, if you break your
`sshd_config`.

I assume your bastion host uses the centralizied
[name resolution service](https://en.wikipedia.org/wiki/Name_Service_Switch),
like LDAP. If so, shell for every user will be set from it. To overcome this,
for users that will only use bastion host as ssh proxy, use `ForceCommand` ssh
directive and set it to `/path/to/forsh`. For users requiring interactive
logins, do not set `ForceCommand` or set it to `none`.

For example, if you have group `users`, who should not login interactively to
the bastion host and group `admins`, who may need to login interactively, you
may use the following settings in `sshd_config` on the abstion host:

```
AllowGroups users admins
Match Group users
  ForceCommand /usr/local/bin/forsh
```

You do not need to set `ForceCommand` for `admins` group as by default it
equals `none` which allows sshd to execute shell on login.

Another example, if all of your users are allowed to use the bastion host as a
proxy and `admins` group should be permitted to login interactively, you may
put `ForceCommand` in global section of `sshd_config` and make an exception in
the match block for admins:

```
ForceCommand /usr/local/bin/forsh
Match Group admins
  ForceCommand none
```

That will be enough for the most uses.

If you for some reason want additional security, you may want to run **forsh**
from a chroot() jail. To do so, compile **forsh** as a static binary (add
`-static` option to gcc in the `Makefile`). I would also recommend to unset
**RESOLVE_USERNAME** option in `config.h`. Otherwise you will have to put all
NSS configuration into your chroot jail, including credentials to access your
centralized name service. The result of disabling **RESOLVE_USERNAME** is that
only uid and not login name will be logged.  

To setup your chroot jail, check appropriate manual for your distribution.

Then, you set `ChrootDirectory` directive in `sshd_config` like this:

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
