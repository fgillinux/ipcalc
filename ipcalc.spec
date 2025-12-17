Name:           ipcalc
Version:        1.1
Release:        1%{?dist}
Summary:        IP Calculator for IPv4
%define debug_package %{nil}

License:        GPLv3
URL:            https://github.com/fgillinux/ipcalc
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make

%description
ipcalc is a simple command-line tool for calculating IPv4 network details. It accepts an IP address and a CIDR mask, 
or it can attempt to discover the CIDR automatically using whois for public IPs. Additionally, the tool can calculate 
the minimum number of subnets needed to meet a given host number requirement, which is useful for computer network planning.

%prep
%setup -q

%build
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT PREFIX=/usr

%files
/usr/bin/ipcalc
%doc README.md CHANGELOG.md LICENSE

%changelog
* Sat Dec 14 2025 Fabio Gil <fabiogil@gmail.com> - 1.1-1
- Initial RPM release
