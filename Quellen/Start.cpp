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
#include <QtCore>

#include "Parameter.h"
#include "Steuerung.h"

int main(int anzahlArgumente, char *argumente[])
{
	QCoreApplication Anwendung(anzahlArgumente, argumente);

	QLoggingCategory::setFilterRules("*.critical=true\n.*.warning=true\n.*.info=true\n.*.debug=false");

	Anwendung.setApplicationName(PROGRAMMNAME);
	Anwendung.setApplicationVersion(VERSION);

	QCommandLineParser Optionen;

	QTranslator QtUebersetzung;
	QTranslator AnwendungUeberstezung;

	QString Uebersetzungspfgad=QLibraryInfo::location(QLibraryInfo::TranslationsPath);

	QtUebersetzung.load(QString("qt_%1").arg(QLocale::system().name()),Uebersetzungspfgad);
	AnwendungUeberstezung.load(QString("%1_%2").arg(PROGRAMMNAME_KLEIN)
											   .arg(QLocale::system().name()),Uebersetzungspfgad);

	Anwendung.installTranslator(&QtUebersetzung);
	Anwendung.installTranslator(&AnwendungUeberstezung);

	Optionen.setApplicationDescription(QObject::tr("%1").arg(PROGRAMMNAME));
	Optionen.addHelpOption();
	Optionen.addVersionOption();

	QString KonfigdateiLang=QObject::tr("konfig");

	Optionen.addOptions({
							{{QObject::tr("k"),KonfigdateiLang},
								QObject::tr("Den Namen der Konfigurationsdatei"),
								QObject::tr("Datei"),
								QString("%1").arg(KONFIG_DATEI)
							}
						});

	Optionen.process(Anwendung);

	QString Konfigurationsdatei=Optionen.value(KonfigdateiLang);

	new Steuerung(&Anwendung,Konfigurationsdatei);

	return Anwendung.exec();
}
