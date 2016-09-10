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
#include "WebsocketServer.h"

#include <QtNetwork>
#include <QWebSocket>

Q_LOGGING_CATEGORY(qalarm_serverWebsocketServer, "QAlarm Server.WebsocketServer")
WebsocketServer::WebsocketServer(QObject *eltern, const QString &name) : QObject(eltern)
{
	qCDebug(qalarm_serverWebsocketServer)<<tr("Bereite Serversocket für %1 vor").arg(name);
	K_Server=new QWebSocketServer(name,QWebSocketServer::SecureMode,this);
	connect(K_Server,&QWebSocketServer::sslErrors,this, &WebsocketServer::SSL_Fehler);
	connect(K_Server,&QWebSocketServer::serverError,this,&WebsocketServer::SSL_Serverfehler);
	connect(K_Server,&QWebSocketServer::newConnection,this,&WebsocketServer::NeuerKlient);
	K_Fehler=false;
}
void WebsocketServer::initialisieren(const QString &ipAdresse, const int &anschluss,const QStringList &sslAlgorithmen,
									 const QStringList &ssl_EK, const QString &zertifikatSchluessel,
									 const QString &zertifikat, const QString &zertifkatKette)
{
	K_IPAdresse=QHostAddress(ipAdresse);
	K_Anschluss=anschluss;
	QSslConfiguration SSL;
	QList<QSslCipher> Algorithmen;
	QVector<QSslEllipticCurve> EK;

	SSL.setProtocol(QSsl::TlsV1_2OrLater);

	for (auto Eintrag : sslAlgorithmen)
		Algorithmen.append(QSslCipher(Eintrag));
	SSL.setCiphers(Algorithmen);

	for (auto Eintrag : ssl_EK)
		EK.append(QSslEllipticCurve::fromShortName(Eintrag));
	SSL.setEllipticCurves(EK);

	QFile *Datei;
	SSL.setLocalCertificate(QSslCertificate(DateiLaden(zertifikat,tr("Zertifikat %1 konnte nicht geladen werden.").arg(zertifikat))));
	SSL.setPrivateKey(QSslKey(DateiLaden(zertifikatSchluessel,tr("Der Schlüssel %1 für das Zertifikat konnte nicht geladen werden.").arg(zertifikatSchluessel))));
	Datei=DateiLaden(zertifkatKette,tr("Die Zertifikatskette %1 konnte nicht geladen werden.").arg(zertifkatKette));
	if(Datei)
		SSL.setLocalCertificateChain(QSslCertificate::fromDevice(Datei));

	if(SSL.privateKey().isNull() || SSL.localCertificate().isNull() || SSL.localCertificateChain().isEmpty())
		return;
	qCDebug(qalarm_serverWebsocketServer)<<tr("Setze SSL Konfigurration");
	K_Server->setSslConfiguration(SSL);
	if(!K_Fehler)
		Q_EMIT Initialisiert();
}

QFile* WebsocketServer::DateiLaden(const QString &datei,const QString &fehlertext)
{
	QFile* Datei=new QFile(datei,this);
	if (Datei->exists())
	{
		qCDebug(qalarm_serverWebsocketServer)<<tr("Datei %1 existiert").arg(datei);
		if (Datei->open(QFile::ReadOnly))
			return Datei;
	}
	qCDebug(qalarm_serverWebsocketServer)<<tr("Datei %1 konnte nicht geladen werden.\n\tFehler: %2").arg(datei).arg(Datei->errorString()).toUtf8().constData();
	K_Fehler=true;
	Q_EMIT Fehler(fehlertext);
	return Q_NULLPTR;
}

void WebsocketServer::SSL_Fehler(const QList<QSslError> &fehler)
{
	for (auto Fehler : fehler)
		qCWarning(qalarm_serverWebsocketServer)<<tr("SSL Fehler: %1").arg(Fehler.errorString());
}

void WebsocketServer::SSL_Serverfehler(QWebSocketProtocol::CloseCode fehlerkode)
{
	Q_UNUSED(fehlerkode);
	Q_EMIT Fehler(tr("Fehler beim erstellen des WSS Socket. %1").arg(K_Server->errorString()));
}

void WebsocketServer::NeuerKlient()
{
	QWebSocket *Klient=K_Server->nextPendingConnection();
	qCInfo(qalarm_serverWebsocketServer)<<tr("Neuer Klient: IP: %1 Anschluss: %2").arg(Klient->peerAddress().toString())
																				  .arg(Klient->peerPort());
	qCDebug(qalarm_serverWebsocketServer)<<tr("Klient fordert an: %1").arg(Klient->requestUrl().toString());
	Klient->deleteLater();
}

void WebsocketServer::starten()
{
	if (!K_Server->listen(K_IPAdresse,K_Anschluss))
		Q_EMIT Fehler(tr("Konnte den Server nicht starten. %1").arg(K_Server->errorString()));
	else
		qCInfo(qalarm_serverWebsocketServer)<<tr("Der Server lauscht auf der Adresse %1 und dem Anschluss %2").arg(K_Server->serverAddress().toString())
																											  .arg(K_Server->serverPort());

}
