**forsh** is "forwarder's shell". It is intended to be used on ssh jump hosts
as a shell for users only doing port forwarding. It also could be in
conjunction with `ForceCommand` directive of `sshd`.

It is meant to be statically linked so it could easily be put in chroot jail.
Effectively it is just `/bin/true` linked statically.  Upon login it will just
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
  

## Links

- https://news.ycombinator.com/item?id=12206628
- https://en.wikibooks.org/wiki/OpenSSH/Cookbook/Proxies_and_Jump_Hosts
