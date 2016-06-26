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

Q_LOGGING_CATEGORY(qalarm_serverSteuerung, "QAlarm Server.Steuerung")
Steuerung::Steuerung(QObject *eltern, const QString &konfigdatei):QObject(eltern)
{
	K_Konfiguration=Q_NULLPTR;
	K_Konfigurationsdatei=konfigdatei;
	QTimer::singleShot(0,this,SLOT(Start()));
}

void Steuerung::Start()
{
	qCInfo(qalarm_serverSteuerung)<<QObject::tr("Starte %1 %2").arg(PROGRAMMNAME)
															   .arg(VERSION);
	if (!K_Konfiguration)
	{
		K_Konfiguration=new Konfiguration(this,K_Konfigurationsdatei);
		connect(K_Konfiguration,&Konfiguration::DateiNichtGefunden,this,&Steuerung::KonfigDateiNichtGefunden);
	}
}

void Steuerung::KonfigDateiNichtGefunden()
{
	qCCritical(qalarm_serverSteuerung)<<QObject::tr("Die Konfigurationsdatei konnte nicht geladen werden.");
	Beenden(-1);
}

void Steuerung::Beenden(int rueckgabe)
{
	qApp->exit(rueckgabe);
}
