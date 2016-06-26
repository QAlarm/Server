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
#ifndef STEUERUNG_H
#define STEUERUNG_H

#include <QtCore>

Q_DECLARE_LOGGING_CATEGORY(qalarm_serverSteuerung)
class Konfiguration;
class Steuerung:  public QObject
{
	Q_OBJECT
	public:
		Steuerung(QObject *eltern,const QString &konfigdatei);

	private Q_SLOTS:
		void			Start();
		void			KonfigDateiNichtGefunden();
		void			KonfigGeladen();

	private:
		Konfiguration*	K_Konfiguration;
		QString			K_Konfigurationsdatei;
		void			Beenden(int rueckgabe=0);
};

#endif // STEUERUNG_H
