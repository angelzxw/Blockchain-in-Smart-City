#include "NetSocket.hh"

NetSocket::NetSocket(QVector<Peer*> *peerList)
{
	// Pick a range of four UDP ports to try to allocate by default,
	// computed based on my Unix user ID.
	// This makes it trivial for up to four Peerster instances per user
	// to find each other on the same host,
	// barring UDP port conflicts with other applications
	// (which are quite possible).
	// We use the range from 32768 to 49151 for this purpose.
    
	this->peerList = peerList;

	myPortMin = 32768 + (getuid() % 4096)*4;
	myPortMax = myPortMin + 3;
	int p=bind();
	if (p == -1)
	{
		exit(1);
	}
	else
	{
		portNum=p;
	}
}

int NetSocket::bind()
{
	// Try to bind to each of the range myPortMin..myPortMax in turn.
	for (int p = myPortMin; p <= myPortMax; p++) {
		if (QUdpSocket::bind(p)) {
			qDebug() << "bound to UDP port " << p;
			return p;
		}
	}

	qDebug() << "Oops, no ports in my default range " << myPortMin
		<< "-" << myPortMax << " available";
	return -1;
}

QString NetSocket::getPortString()
{
	return QString::number(portNum);
}
quint16 NetSocket::getPortNum()
{
	return portNum;
}

void NetSocket::sendMsg(QString text, QString origin, quint32 seq, const QHostAddress addr, quint16 port, quint32 lastIP, quint16 lastPort)
{
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("ChatText",text);
	dataMap.insert("Origin",origin);
	dataMap.insert("SeqNo",seq);
	if(!(lastIP == 0 && lastPort == 0))
	{
		dataMap.insert("LastIP",lastIP);
	  	dataMap.insert("LastPort",lastPort);
	}
	out << dataMap;
	qDebug() << "sent: "<< origin<<" "<<seq<<" "<<text;
	this->writeDatagram(dataArray,addr,port);
}

void NetSocket::sendRouteRumor(QString origin, quint32 seq, const QHostAddress addr, quint16 port, quint32 lastIP, quint16 lastPort)
{
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("Origin",origin);
	dataMap.insert("SeqNo",seq);

	if(!(lastIP == 0 && lastPort == 0))
	{
		dataMap.insert("LastIP",lastIP);
	  	dataMap.insert("LastPort",lastPort);
	}
	out << dataMap;
	qDebug() << "sent: "<< origin<<" "<<seq<<" routeRumor";
	this->writeDatagram(dataArray,addr,port);
}

void NetSocket::sendBlockRequest(QString dest, QString origin, quint32 ttl, QByteArray sha1Hash, const QHostAddress addr,quint16 port)
{
	qDebug() << "sent BlockRequest to "<< dest;
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("Dest", dest);
	dataMap.insert("Origin",origin);
	dataMap.insert("HopLimit",ttl);
	dataMap.insert("BlockRequest",sha1Hash);
	out << dataMap;
	this->writeDatagram(dataArray,addr,port);
}
void NetSocket::sendBlockReply(QString dest, QString origin, quint32 ttl, QByteArray sha1Hash, QByteArray data, const QHostAddress addr,quint16 port)
{
	qDebug() << "sent BlockReply to "<< dest;
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("Dest", dest);
	dataMap.insert("Origin", origin);
	dataMap.insert("HopLimit", ttl);
	dataMap.insert("BlockReply", sha1Hash);
	dataMap.insert("Data", data);
	out << dataMap;
	this->writeDatagram(dataArray,addr,port);
}

void NetSocket::sendNewBLOCK(Block block, const QHostAddress addr, quint16 port)
{
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("Name", block.name);
	dataMap.insert("Index", block.index);
	dataMap.insert("Time", block.time);
	dataMap.insert("Prev_Hash", block.prev_hash);
	dataMap.insert("Data", block.data);
	dataMap.insert("Hash", block.hash);
	dataMap.insert("Rand", block.rand);

	out << dataMap;
	
	this->writeDatagram(dataArray,addr,port);

	qDebug() << "send new BLOCK to: " << addr << ":" << port;
}

void NetSocket::sendWantBLOCKMsg(const QHostAddress addr, quint16 port, quint16 seq)
{
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("WantBLOCK",seq);

	out << dataMap;

	this->writeDatagram(dataArray,addr,port);
}

void NetSocket::sendNewBLOCKToNeighbors(Block block, QHostAddress exSender, quint16 exSenderPort)
{
	quint32 peerSize = ((exSender == QHostAddress::Null && exSenderPort == 0) ? peerList->size() - 1: peerList->size() -2);

	if(peerSize <= 0)
	{
		return ;
	}

	for(quint16 i = 1; i < peerList->size(); i++)
	{
		Peer* peer = peerList->at(i);
		if(!(peer->IPAddress == exSender && peer->UDPPortNum == exSenderPort))
		{	
			this->sendNewBLOCK(block, peer->IPAddress, peer->UDPPortNum);
		}
	}
}

void NetSocket::sendPrivateMsg(QString dest, QString origin, QString text, quint32 ttl, const QHostAddress addr,quint16 port)
{
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("Dest", dest);
	dataMap.insert("Origin",origin);
	dataMap.insert("ChatText",text);
	dataMap.insert("HopLimit",ttl);
	//no seqno
	out << dataMap;
	//qDebug() << "sent: "<< origin<<" "<<seq<<" "<<text;
	this->writeDatagram(dataArray,addr,port);
}
