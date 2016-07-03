%{?systemd_requires}
%global _hardened_build 1

Name:		qalarm-server
Version:	VERSION
Release:	1%{?dist}
License:	GPLv3
Summary:	The server component of the qalarm system.
Summary(de):	Die Serverkomponente des QAlarm System.

Source0:	SOURCE0

BuildRequires:	qt5-qtbase-devel qt5-qtwebsockets-devel systemd qalarm-lib-devel

%description
The server module of the QAlarm system.

%description -l de
Das Servermodul vom QAlarm System.

%prep
%autosetup

%build
qmake-qt5
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT

mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_unitdir}
mkdir -p %{buildroot}%{_qalarm_conf}
mkdir -p %{buildroot}%{_qt5_translationdir}

install bin/qalarm-server %{buildroot}%{_bindir}
install qalarm.service %{buildroot}%{_unitdir}
%find_lang %{name} --with-qt

%post
%systemd_post qalarm.service

%preun
%systemd_preun qalarm.service

%postun
%systemd_postun_with_restart qalarm.service

%files -f %{name}.lang
%defattr(-,root,root,-)
%doc README.md LICENSE
%{_bindir}/%{name}
%{_unitdir}/qalarm.service
%config(noreplace) %{_qalarm_conf}/server.ini

%changelog
* Tag Monat 2016 tuxmaster <qalarm@terrortux.de> - 0.0.1-1
- start
