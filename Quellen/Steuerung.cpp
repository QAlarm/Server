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

Q_LOGGING_CATEGORY(qalarm_serverSteuerung, "QAlarm Server.Steuerung")
Steuerung::Steuerung(QObject *eltern, const QString &konfigdatei):QObject(eltern)
{
	K_Konfiguration=Q_NULLPTR;
	K_WebsocketServer=Q_NULLPTR;
	K_Konfigurationsdatei=konfigdatei;
	QTimer::singleShot(0,this,SLOT(Start()));
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

void Steuerung::KonfigDateiNichtGefunden()
{
	qCCritical(qalarm_serverSteuerung)<<tr("Die Konfigurationsdatei konnte nicht geladen werden.");
	Beenden(-1);
}

void Steuerung::Beenden(const int &rueckgabe)const
{
	Beenden(rueckgabe,"");
}

void Steuerung::Beenden(const int rueckgabe, const QString& meldung)const
{
	if (!meldung.isEmpty())
		qCCritical(qalarm_serverSteuerung)<<meldung;
	qApp->exit(rueckgabe);
}

void Steuerung::KonfigGeladen()
{
	QString Protokollfilter;
	switch (K_Konfiguration->WertHolen(KONFIG_PROTOKOLLEBENE).toInt())
	{
		case 1:
				Protokollfilter="*.critical=true\n*.warning=false\n*.info=false\n*.debug=false";
				break;
		case 2:
				Protokollfilter="*.critical=true\n*.warning=true\n*.info=false\n*.debug=false";
				break;
		case 3:
				Protokollfilter="*.critical=true\n*.warning=true\n*.info=true\n*.debug=false";
				break;
		case 4:
		default:
				Protokollfilter="*.critical=true\n*.warning=true\n*.info=true\n*.debug=true";
				break;
	}
	QLoggingCategory::setFilterRules(Protokollfilter);
	QLoggingCategory *Protokollkategorie=QLoggingCategory::defaultCategory();
	qCInfo(qalarm_serverSteuerung)<<tr("Setzte Protokoll auf: \n\tKritisch: %1.\n\tWarnung: %2\n\tInfo: %3\n\tDebug: %4.").arg(Protokollkategorie->isCriticalEnabled())
																														   .arg(Protokollkategorie->isWarningEnabled())
																														   .arg(Protokollkategorie->isInfoEnabled())
																														   .arg(Protokollkategorie->isDebugEnabled()).toUtf8().constData();
	qCInfo(qalarm_serverSteuerung)<<tr("Starte WSS ...");
	QString Servername=K_Konfiguration->WertHolen(KONFIG_SERVERNAME).toString();
	QString IPAdresse=K_Konfiguration->WertHolen(KONFIG_IPADRESSE).toString();
	int Anschluss=K_Konfiguration->WertHolen(KONFIG_ANSCHLUSS).toInt();
	QStringList SSL_Algorithmen=K_Konfiguration->WertHolen(KONIFG_SSLALGORITHMEN).toString().split(":");
	QStringList SSL_EK=K_Konfiguration->WertHolen(KONFIG_SSLEK).toString().split(":");
	if (Servername.isEmpty())
	{
		Beenden(-2,tr("Servername nicht gesetzt."));
		return;
	}
	if (IPAdresse.isEmpty())
	{
		Beenden(-3,tr("IP Adresse nicht gesetzt."));
		return;
	}
	if (Anschluss==0)
	{
		Beenden(-4,tr("Anschluss nicht gesetzt"));
		return;
	}
	if (SSL_Algorithmen.isEmpty())
	{
		Beenden(-5,tr("Keine SSL/TLS Algorithmen gesetzt."));
		return;
	}
	if (SSL_EK.isEmpty())
	{
		Beenden(-6,tr("Keine elliptischen Kurven für SSL/TLS gesetzt."));
		return;
	}
	K_WebsocketServer=new WebsocketServer(this,Servername,IPAdresse,Anschluss,SSL_Algorithmen,SSL_EK);
}
