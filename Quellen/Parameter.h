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
#ifndef PARAMETER_H
#define PARAMETER_H

#define PROGRAMMNAME_KLEIN				"qalarm_server"
#define PROGRAMMNAME					"QAlarm Server"
#define VERSION							"0.0.1"

#define KONFIG_DATEI					"/etc/qalarm/server.ini"
#define KONFIG_PROTOKOLLEBENE			"Server/Protokollebene"
#define KONFIG_SERVERNAME				"Server/Servername"
#define KONFIG_IPADRESSE				"Server/IPAdresse"
#define KONFIG_ANSCHLUSS				"Server/Anschluss"
#define KONIFG_SSLALGORITHMEN			"SSL/Algorithmen"
#define KONFIG_SSLEK					"SSL/EK"
#define KONFIG_SSLZERTIFIKAT			"SSL/Zertifikat"
//BUG QTBUG-54510
//#define KONFIG_SSLZERTIFIKATSCHLUESSEL  "SSL/Zertifikat Schlüssel"
#define KONFIG_SSLZERTIFIKATSCHLUESSEL  "SSL/Zertifikat Schluessel"
#define KONFIG_SSLZERTIFIKATKETTE		"SSL/Zertifikatskette"

#define KONFIG_SMTP_SERVER				"SMTP/Server"
#define KONFIG_SMTP_PORT				"SMTP/Port"
#define KONFIG_SMTP_NUTZER				"SMTP/Nutzer"
#define KONFIG_SMTP_PASSWORT			"SMTP/Passwort"
#define KONFIG_SMTP_ABSENDER			"SMTP/Absender"

#endif // PARAMETER_H
