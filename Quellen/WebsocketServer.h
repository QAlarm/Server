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
#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QtCore>
#include <QHostAddress>
#include <QSslError>
#include <QWebSocketServer>

Q_DECLARE_LOGGING_CATEGORY(qalarm_serverWebsocketServer)
class WebsocketServer : public QObject
{
	Q_OBJECT
	public:
		explicit WebsocketServer(QObject *eltern, const QString& name);
							~WebsocketServer();
		void				initialisieren(const QString &ipAdresse, const int &anschluss,const QStringList &sslAlgorithmen,
										   const QStringList &ssl_EK, const QString &ssl_DH, const QString &zertifikatSchluessel,
										   const QString &zertifikat, const QString &zertifkatKette);
		void				starten();

	Q_SIGNALS:
		void				Fehler(const QString &text);
		void				Initialisiert();

	private Q_SLOTS:
		void				SSL_Fehler(const QList<QSslError> &fehler);
		void				SSL_Serverfehler(QWebSocketProtocol::CloseCode fehlerkode);
		void				NeuerKlient();
		void				Verbindungsfehler(QAbstractSocket::SocketError fehler);

	private:
		Q_DISABLE_COPY(WebsocketServer)
		QWebSocketServer*	K_Server;
		QHostAddress		K_IPAdresse;
		int					K_Anschluss;
		QFile*				DateiLaden(const QString &datei, const QString &fehlertext);
		bool				K_Initfehler;
		QList<QWebSocket*>	*K_Klientliste;
};

#endif // WEBSOCKETSERVER_H
