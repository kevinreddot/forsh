Name:           forsh
Version:        1.0
Release:        1%{?dist}
Summary:        Shell intended for forwarding-only ssh connection via jumphost

License:        Public Domain
URL:            https://github.com/ivladdalvi/forsh
Source0:        https://github.com/ivladdalvi/forsh/archive/v1.0.tar.gz

BuildRequires:  gcc make
Requires:       syslog

%description
"forsh" stands for "forwarder's shell". It is intended to be used on ssh
jumphosts via "ForceCommand" directive.

Users with enforced forsh will not be able to login interactively or execute
commands. However, they will be able to use the host for port forwarding or as
jumphost via "ProxyJump" ssh_config directive.
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
