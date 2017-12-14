#ifndef PEERSTER_NETSOCKET_HH
#define PEERSTER_NETSOCKET_HH

#include <QThread>

#include "Block.hh"
#include "NetSocket.hh"

class NewBlockThread : public QThread
{
    Q_OBJECT

public:
    NewBlockThread(quint32 index, NetSocket* sock, Block lastBlock);
    void setCurIndex(quint32 index);
    void updateData(QVariantMap data);
    void updateLastBlock(Block lastBlock);

protected:
    Block* createNewBlock(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap* data);
    void run();

private:
    quint32 index;
    QVariantMap data;
    NetSocket *sock;
    Block lastBlock;
};

#endif
