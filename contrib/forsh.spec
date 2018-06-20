Name:           forsh
Version:        1.0
Release:        1%{?dist}
Summary:        Shell intended for forwarding-only ssh connection via jumphost

License:        Unlicense
URL:            https://github.com/ivladdalvi/forsh
Source0:        https://github.com/ivladdalvi/forsh/archive/v1.0.tar.gz

BuildRequires:  gcc make
Requires:       syslog

%description
"forsh" stands for "forwarder's shell". It is intended to be used on ssh jump hosts as a shell for users only doing port forwarding. It can also could be in conjunction with ForceCommand directive of
sshd.

It can be statically linked and easily be put in a chroot jail. Effectively, it is just /bin/true with some logging. If a user attempts to login interactively or run a command, forsh will log this
attempt. If user just requests port forwarding or stdin/stdout/stderr forwarding (as ProxyJump and -W do), this is not considered a violation and nothing is logged.


%prep
%setup -q


%build
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
%make_install


%files
%doc README.md
%license LICENSE
%{_bindir}/forsh



%changelog
* Wed Jun 20 2018 Vladimir Ivanov <ivlad@yandex.com> - 1.0-1
- first build
