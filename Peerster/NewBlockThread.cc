#include "NewBlockThread.hh"

NewBlockThread::NewBlockThread(quint32 index, NetSocket* sock, Block lastBlock)
{
    this->index = index;
    this->sock = sock;
    this->lastBlock = lastBlock;
}

void NewBlockThread::setCurIndex(quint32 index)
{
    this->index = index;
}

void NewBlockThread::updateData(QVariantMap data)
{
    this->data = data;
}

void NewBlockThrea::updateLastBlock(Block lastBlock)
{
	this->lastBlock = lastBlock;
}

Block* NewBlockThread::createNewBlock(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap data)
{
	quint64 i = qrand();
	while(true)
	{
		if(verifyHash(hashPartialBlock(name, index, time, prev_hash, i)))
		{
			break;
		}
		i++;
	}
	Block* newBlock = new Block(name, index, time, prev_hash, data, hashBlock(name, index, time, prev_hash, data, i), i);
	return newBlock;
}

void NewBlockThread::run()
{
	
}
