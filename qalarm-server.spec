%{?systemd_requires}
%global _hardened_build 1

Name:		qalarm-server
Version:	VERSION
Release:	1
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
%setup -q -n

%build
qmake-qt5
make %{?_smp_mflags}

%install
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_unitdir}
mkdir -p %{buildroot}%{_qalarm_conf}

install bin/qalarm-server %{buildroot}%{_bindir}
install qalarm.service %{buildroot}%{_unitdir}
%find_lang %{name}

%post
%systemd_post qalarm.service

%preun
%systemd_preun qalarm.service

%postun
%systemd_postun_with_restart qalarm.service

%files -f %{name}.lang
%doc README.md LICENSE
%{_bindir}/%{name}
%{_unitdir}/qalarm.service
%config(noreplace) %{_qalarm_conf}/server.ini

%changelog
* Tag Monat 2016 tuxmaster <qalarm@terrortux.de> - 0.0.1-1
- start
