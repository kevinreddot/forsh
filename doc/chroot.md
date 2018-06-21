# Running in chroot

Some notes:

- `journald` cannot have multiple syslog sockets (see
  https://github.com/systemd/systemd/issues/4726), so you cannot have seconday
  syslog socket in the chroot jail. Alternative is to do `mount --bind` of
  existing socket into jail

- in order for `getpwuid()` to function properly, it needs access to user's
  database and NSS configuration. Your options are either to disable name
  resolution via **RESOLVE_USERNAME** as discussed in the README.md, or put
  your NSS modules in the chroot jail. In theory, you can also recompile your
  glibc with [static
  NSS](https://sourceware.org/glibc/wiki/FAQ#Even_statically_linked_programs_need_some_shared_libraries_which_is_not_acceptable_for_me.__What_can_I_do.3F),
  but I did not try it and I am not sure it is well tested.

- `sshd` runs what is specified in the **ForceCommand** not directly, but via
  the respective user's shell. E.g., if user has `/bin/bash` as shell, and
  `/usr/bin/forsh` is set with **ForceCommand**, `sshd` will run `/bin/bash -c
  /usr/bin/forsh`. Make sure you needed shells and libraries in the chroot
  jail.
