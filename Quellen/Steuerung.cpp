/*
	Copyright (C) 2016 Frank Büttner qalarm@terrortux.de

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "Steuerung.h"
#include "Parameter.h"
#include "Konfiguration.h"
#include "WebsocketServer.h"
#include "Protokollierung.h"

#ifdef Q_OS_UNIX
#include <sys/socket.h>
#include <unistd.h>
#endif

Q_LOGGING_CATEGORY(qalarm_serverSteuerung, "QAlarm Server.Steuerung")
Steuerung::Steuerung(QObject *eltern, const QString &konfigdatei):QObject(eltern)
{
	K_Konfiguration=Q_NULLPTR;
	K_WebsocketServer=Q_NULLPTR;
	K_Konfigurationsdatei=konfigdatei;

#ifdef Q_OS_UNIX
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, K_Unix_Signal_term))
	   qFatal(tr("Konnte das 'TEM' Signal nicht anschließen.").toUtf8().constData());
	K_Socket_Signal_term=new QSocketNotifier(K_Unix_Signal_term[1], QSocketNotifier::Read, this);
	connect(K_Socket_Signal_term, &QSocketNotifier::activated,this, &Steuerung::Qt_Bearbeitung_Unix_Signal_term);

	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, K_Unix_Signal_int))
	   qFatal(tr("Konnte das 'INT' Signal nicht anschließen.").toUtf8().constData());
	K_Socket_Signal_int=new QSocketNotifier(K_Unix_Signal_int[1], QSocketNotifier::Read, this);
	connect(K_Socket_Signal_int, &QSocketNotifier::activated,this, &Steuerung::Qt_Bearbeitung_Unix_Signal_int);
#endif
	QTimer::singleShot(0,this,&Steuerung::Start);
}

void Steuerung::Start()
{
	qCInfo(qalarm_serverSteuerung)<<tr("Starte %1 %2").arg(PROGRAMMNAME)
													  .arg(VERSION);
	if (!K_Konfiguration)
	{
		K_Konfiguration=new Konfiguration(this,K_Konfigurationsdatei);
		connect(K_Konfiguration,&Konfiguration::DateiNichtGefunden,this,&Steuerung::KonfigDateiNichtGefunden);
		connect(K_Konfiguration,&Konfiguration::Geladen,this,&Steuerung::KonfigGeladen);
	}
}

void Steuerung::Ende()
{
	QCoreApplication::exit();
}

void Steuerung::KonfigDateiNichtGefunden()
{
	qCCritical(qalarm_serverSteuerung)<<tr("Die Konfigurationsdatei konnte nicht geladen werden.");
	Beenden(1);
}

void Steuerung::Beenden(const int &rueckgabe)const
{
	Beenden(rueckgabe,"");
}

void Steuerung::Beenden(const int rueckgabe, const QString& meldung)const
{
	if (!meldung.isEmpty())
		qCCritical(qalarm_serverSteuerung)<<meldung.toUtf8().constData();
	qApp->exit(rueckgabe);
}

void Steuerung::KonfigGeladen()
{
	Protokollierung* Protollebene=new Protokollierung(K_Konfiguration->WertHolen(KONFIG_PROTOKOLLEBENE).toInt(),this);
	Q_UNUSED(Protollebene);
	qCInfo(qalarm_serverSteuerung)<<tr("Starte den Websocket ...");
	WebsocketKonfigurieren();
}

void Steuerung::WebsocketKonfigurieren()
{
	QString Servername=K_Konfiguration->WertHolen(KONFIG_SERVERNAME).toString();
	QString IPAdresse=K_Konfiguration->WertHolen(KONFIG_IPADRESSE).toString();
	int Anschluss=K_Konfiguration->WertHolen(KONFIG_ANSCHLUSS).toInt();
	QStringList SSL_Algorithmen=K_Konfiguration->WertHolen(KONIFG_SSLALGORITHMEN).toStringList();
	QStringList SSL_EK=K_Konfiguration->WertHolen(KONFIG_SSLEK).toStringList();
	QString SSL_DH=K_Konfiguration->WertHolen(KONFIG_SSLDH).toString();
	QString SSL_Zertfikat=K_Konfiguration->WertHolen(KONFIG_SSLZERTIFIKAT).toString();
	QString SSL_Schluessel=K_Konfiguration->WertHolen(KONFIG_SSLZERTIFIKATSCHLUESSEL).toString();
	QString SSL_Kette=K_Konfiguration->WertHolen(KONFIG_SSLZERTIFIKATKETTE).toString();
	bool SSL_Aktiv=K_Konfiguration->WertHolen(KONFIG_SSL_AKTIV).toBool();

	if (Servername.isEmpty())
	{
		Beenden(2,tr("Servername nicht gesetzt."));
		return;
	}
	if (IPAdresse.isEmpty() && SSL_Aktiv)
	{
		Beenden(3,tr("IP Adresse nicht gesetzt."));
		return;
	}
	if (Anschluss==0)
	{
		Beenden(4,tr("Anschluss nicht gesetzt"));
		return;
	}
	if (SSL_Algorithmen.isEmpty() && SSL_Aktiv)
	{
		Beenden(5,tr("Keine SSL/TLS Algorithmen gesetzt."));
		return;
	}
	if (SSL_EK.isEmpty() && SSL_Aktiv)
	{
		Beenden(6,tr("Keine elliptischen Kurven für SSL/TLS gesetzt."));
		return;
	}
	if(SSL_Zertfikat.isEmpty() && SSL_Aktiv)
	{
		Beenden(7,tr("Kein X509 Zertifikat gesetzt."));
		return;
	}
	if(SSL_Schluessel.isEmpty() && SSL_Aktiv)
	{
		Beenden(8,tr("Kein privater Schlüssel für das X509 Zertifikat gesetzt."));
		return;
	}
	if(SSL_Kette.isEmpty() && SSL_Aktiv)
	{
		Beenden(9,tr("Es wurde keine Zertifikatskette angegeben."));
		return;
	}
	K_WebsocketServer=new WebsocketServer(this);
	connect(K_WebsocketServer,&WebsocketServer::Fehler,this,&Steuerung::Fehler);
	connect(K_WebsocketServer,&WebsocketServer::Initialisiert,this,&Steuerung::ServerBereit);
	K_WebsocketServer->initialisieren(Servername,IPAdresse,Anschluss,SSL_Algorithmen,SSL_EK,SSL_DH,SSL_Schluessel,
									  SSL_Zertfikat,SSL_Kette,SSL_Aktiv);
}

void Steuerung::Fehler(const QString &text)
{
	Beenden(10,text);
}

void Steuerung::ServerBereit()
{
	K_WebsocketServer->starten();
}

#ifdef Q_OS_UNIX
void Steuerung::Unix_Signal_Bearbeitung_term(int nichtBenutzt)
{
	qCDebug(qalarm_serverSteuerung)<<"Beenden via Unix Signal TERM.";
	Q_UNUSED(nichtBenutzt);
	char tmp = 1;
	::write(K_Unix_Signal_term[0], &tmp, sizeof(tmp));
}

void Steuerung::Unix_Signal_Bearbeitung_int(int nichtBenutzt)
{
	qCDebug(qalarm_serverSteuerung)<<"Beenden via Unix Signal INT.";
	Q_UNUSED(nichtBenutzt);
	char tmp = 1;
	::write(K_Unix_Signal_int[0], &tmp, sizeof(tmp));
}

void Steuerung::Qt_Bearbeitung_Unix_Signal_term()
{
	K_Socket_Signal_term->setEnabled(false);
	char tmp;
	::read(K_Unix_Signal_term[1], &tmp, sizeof(tmp));
	Ende();
}

void Steuerung::Qt_Bearbeitung_Unix_Signal_int()
{
	K_Socket_Signal_int->setEnabled(false);
	char tmp;
	::read(K_Unix_Signal_int[1], &tmp, sizeof(tmp));
	Ende();
}

int Steuerung::K_Unix_Signal_term[2]={0};
int Steuerung::K_Unix_Signal_int[2]={0};
#endif
;
