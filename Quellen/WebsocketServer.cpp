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

#include <QWebSocketServer>

Q_LOGGING_CATEGORY(qalarm_serverWebsocketServer, "QAlarm Server.WebsocketServer")
WebsocketServer::WebsocketServer(QObject *eltern, const QString &name) : QObject(eltern)
{
	qCDebug(qalarm_serverWebsocketServer)<<tr("Bereite Serversocket für %1 vor").arg(name);
	K_Server=new QWebSocketServer(name,QWebSocketServer::SecureMode,this);
}
