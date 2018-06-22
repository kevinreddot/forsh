# Running in chroot

Some notes:

- `journald` cannot have multiple syslog sockets (see
  https://github.com/systemd/systemd/issues/4726), so you cannot have seconday
  syslog socket in the chroot jail. Alternative is to do `mount --bind` of
  existing socket into jail

- `sshd` runs what is specified in the **ForceCommand** not directly, but via
  the respective user's shell. E.g., if user has `/bin/bash` as shell, and
  `/usr/bin/forsh` is set with **ForceCommand**, `sshd` will run `/bin/bash -c
  /usr/bin/forsh`. Make sure you needed shells and libraries in the chroot
  jail.
