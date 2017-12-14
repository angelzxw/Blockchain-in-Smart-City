#ifndef PEERSTER_PEER_HH
#define PEERSTER_PEER_HH

class Peer
{
public:
  Peer(QString DNSHostName, QHostAddress IPAddress, quint16 UDPPortNum)
  {
    this->DNSHostName = DNSHostName;
    this->IPAddress = IPAddress;
    this->UDPPortNum = UDPPortNum;
  }
  QString DNSHostName;
  QHostAddress IPAddress;
  quint16 UDPPortNum;
};

#endif
