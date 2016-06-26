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
#include "Konfiguration.h"
#include "Parameter.h"

Q_LOGGING_CATEGORY(qalarm_serverKonfiguration, "QAlarm Server.Konfiguration")
Konfiguration::Konfiguration(QObject *eltern, const QString &datei):QObject (eltern)
{
	K_Konfig=Q_NULLPTR;
	K_Datei=datei;
	QTimer::singleShot(0,this,SLOT(Laden()));
}

void Konfiguration::Laden()
{
	if (K_Konfig)
		return;

	qCInfo(qalarm_serverKonfiguration)<<QObject::tr("Lade Datei %1").arg(K_Datei);

	if (!QFile::exists(K_Datei))
	{
		Q_EMIT DateiNichtGefunden();
		return;
	}
	K_Konfig=new QSettings(K_Datei,QSettings::IniFormat,this);
}
