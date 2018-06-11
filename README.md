**forsh** is "forwarder's shell". It is intended to be used on ssh jump hosts
as a shell for users only doing port forwarding. It also could be in
conjunction with `ForceCommand` directive of `sshd`.

It is meant to be statically linked so it could easily be put in chroot jail.
Effectively it is just `/bin/true` linked statically.  Upon login it will just
exit. If login has terminal allocated, it will log this event as it would
mean, the user tried to login interactively.

## Links

- https://news.ycombinator.com/item?id=12206628
