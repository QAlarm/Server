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
WebsocketServer::WebsocketServer(QObject *eltern) : QObject(eltern)
{
	qCDebug(qalarm_serverWebsocketServer)<<tr("Bereite Serversocket vor");
	K_Initfehler=false;
	K_Klientliste=new QList<QWebSocket*>;
}
WebsocketServer::~WebsocketServer()
{
	for (auto Klient : *K_Klientliste)
	{
		Klient->close(QWebSocketProtocol::CloseCodeGoingAway,tr("Der Server fährt runter."));
		Klient->deleteLater();
	}
}

void WebsocketServer::initialisieren(const QString &name, const QString &ipAdresse, const int &anschluss, const QStringList &sslAlgorithmen,
									 const QStringList &ssl_EK, const QString &ssl_DH, const QString &zertifikatSchluessel,
									 const QString &zertifikat, const QString &zertifkatKette, const bool &ssl_aktiv)
{
	QWebSocketServer::SslMode SSL_Modus;
	if (ssl_aktiv)
	{
		SSL_Modus=QWebSocketServer::SecureMode;
		K_IPAdresse=QHostAddress(ipAdresse);
	}
	else
	{
		SSL_Modus=QWebSocketServer::NonSecureMode;
		K_IPAdresse=QHostAddress(QHostAddress::LocalHost);
	}
	K_Server=new QWebSocketServer(name,SSL_Modus,this);
	connect(K_Server,&QWebSocketServer::sslErrors,this, &WebsocketServer::SSL_Fehler);
	connect(K_Server,&QWebSocketServer::serverError,this,&WebsocketServer::SSL_Serverfehler);
	connect(K_Server,&QWebSocketServer::newConnection,this,&WebsocketServer::NeuerKlient);
	connect(K_Server,&QWebSocketServer::acceptError,this,&WebsocketServer::Verbindungsfehler);


	K_Anschluss=anschluss;
	QSslConfiguration SSL;
	QList<QSslCipher> Algorithmen;
	QVector<QSslEllipticCurve> EK;

	SSL.setProtocol(QSsl::TlsV1_2OrLater);
	SSL.setPeerVerifyMode(QSslSocket::VerifyNone);


	QSslCipher Algorithmus;
	QSslEllipticCurve Kurve;
	if (ssl_aktiv)
	{
//BUG Das mit der Reihenfolge geht nicht

		for (auto Eintrag : sslAlgorithmen)
		{
			Algorithmus=QSslCipher(Eintrag);
			if (Algorithmus.isNull())
				qCWarning(qalarm_serverWebsocketServer)<< tr("Algorithmus %1 wird nicht unterstützt.").arg(Eintrag);
			else
				Algorithmen.append(Algorithmus);
		}
		SSL.setCiphers(Algorithmen);
//BUG Es werden nie Kurven angeboten

		for (auto Eintrag : ssl_EK)
		{
			Kurve=QSslEllipticCurve::fromShortName(Eintrag);
			if (!Kurve.isValid())
				qCWarning(qalarm_serverWebsocketServer)<< tr("Kurve %1 wird nicht unterstützt.").arg(Eintrag);
			else
				EK.append(Kurve);
		}
		SSL.setEllipticCurves(EK);

		if(!ssl_DH.isEmpty())
		{
#if (QT_VERSION >= QT_VERSION_CHECK(5,8,0))
			SSL.setDiffieHellmanParameters(QSslDiffieHellmanParameters::fromEncoded(DateiLaden(ssl_DH,tr("Die DH Parameter %1 konnten nicht geladen werden."))));
#else
			qCWarning(qalarm_serverWebsocketServer)<<tr("Qt kann die DH Parameter erst ab Version 5.8.0 setzen");
#endif
		}

		QList<QSslCertificate> Zertifikate;
		Zertifikate=QSslCertificate::fromDevice(DateiLaden(zertifikat,tr("Zertifikat %1 konnte nicht geladen werden.").arg(zertifikat)));
		Zertifikate.append(QSslCertificate::fromDevice(DateiLaden(zertifkatKette,tr("Die Zertifikatskette %1 konnte nicht geladen werden.").arg(zertifkatKette))));
		SSL.setLocalCertificateChain(Zertifikate);

		SSL.setPrivateKey(QSslKey(DateiLaden(zertifikatSchluessel,tr("Der Schlüssel %1 für das Zertifikat konnte nicht geladen werden.").arg(zertifikatSchluessel)),QSsl::Rsa));

		if(SSL.privateKey().isNull() || SSL.localCertificate().isNull() || SSL.localCertificateChain().isEmpty())
			return;
		qCDebug(qalarm_serverWebsocketServer)<<tr("Setze SSL Konfiguration");
		qCDebug(qalarm_serverWebsocketServer)<<tr("Privater Schlüssel: ")<<SSL.privateKey();
		qCDebug(qalarm_serverWebsocketServer)<<tr("Zertifikate: ")<<SSL.localCertificateChain();
#if (QT_VERSION >= QT_VERSION_CHECK(5,8,0))
		qCDebug(qalarm_serverWebsocketServer)<<tr("DH Parameter: ")<<SSL.diffieHellmanParameters();
#endif
		qCDebug(qalarm_serverWebsocketServer)<<tr("Zerttest: ")<<SSL.peerVerifyMode();
		qCDebug(qalarm_serverWebsocketServer)<<tr("Elliptische Kurven: ")<<SSL.ellipticCurves();
		qCDebug(qalarm_serverWebsocketServer)<<tr("Algorithmen: ")<<SSL.ciphers();
		K_Server->setSslConfiguration(SSL);
	}
	if(!K_Initfehler)
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
	K_Initfehler=true;
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

void WebsocketServer::Verbindungsfehler(QAbstractSocket::SocketError fehler)
{
	qCDebug(qalarm_serverWebsocketServer)<<tr("Fehler beim Verbínungsaufbau: %1 %2").arg(fehler)
																					.arg(K_Server->errorString());
}
