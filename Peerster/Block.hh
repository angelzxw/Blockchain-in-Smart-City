#ifndef PEERSTER_BLOCK_HH
#define PEERSTER_BLOCK_HH

#include <unistd.h>
#include <QTime>
#include <QVariantMap>

class Block
{
public:
    Block()
    {
        isNullFlag = true;
    }
    Block(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap data, QByteArray hash, quint64 rand)
    {
        isNullFlag = false;
        this->name = name;
        this->index = index;
        this->time = time;
        this->prev_hash = prev_hash;
        this->data = data;
        this->hash = hash;
        this->rand = rand;
    }

    bool isNull()
    {
        return isNullFlag;
    }

    bool isNullFlag;

    QString name;
    quint32 index;
    QTime time;
    QByteArray prev_hash;
    QVariantMap data;
    QByteArray hash;
    quint64 rand;
};

#endif
