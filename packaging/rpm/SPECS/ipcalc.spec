%global debug_package %{nil}
Name:           ipcalc-custom
Version:        1.0
Release:        1%{?dist}
Summary:        Custom IP Calculator

License:        GPLv3
URL:            http://example.com/ipcalc
Source0:        ipcalc-1.0.tar.gz

BuildRequires:  gcc
# Requires:       whois

%description
Calculates network details from IP/CIDR.
Supports auto-discovery of CIDR for public IPs using whois.

%prep
%setup -c -T
cp %{SOURCE0} .
tar -xf %{SOURCE0}

%build
gcc -o ipcalc ipcalc.c

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/bin
install -m 755 ipcalc %{buildroot}/usr/bin/ipcalc

%files
/usr/bin/ipcalc

%changelog
* Tue Dec 03 2025 Fabio Gil <fabiogil@gmail.com> - 1.0-1
- Initial package
