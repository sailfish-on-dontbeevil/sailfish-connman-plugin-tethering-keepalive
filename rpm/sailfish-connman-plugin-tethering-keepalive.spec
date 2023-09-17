Name: sailfish-connman-plugin-tethering-keepalive
Version: 1.0.0
Release: 1
Summary: keepalive plugin for tethering
License: GPLv2
URL: https://github.com/sailfish-on-dontbeevil/sailfish-connman-plugin-tethering-keepalive
Source: %{name}-%{version}.tar.bz2
Requires: connman >= 1.32+git36
BuildRequires: pkgconfig(connman) >= 1.32+git36
BuildRequires: pkgconfig(libgsupplicant) >= 1.0.11
BuildRequires: pkgconfig(libglibutil)

%define plugin_dir %{_libdir}/connman/plugins

%description
This plugin switches take a wakelock when tethering is enabled to prevent full system sleep

%prep
%setup -q -n %{name}-%{version}

%build
make %{_smp_mflags} KEEP_SYMBOLS=1 release

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} LIBDIR=%{_libdir} install

%files
%defattr(-,root,root,-)
%{plugin_dir}/*.so
