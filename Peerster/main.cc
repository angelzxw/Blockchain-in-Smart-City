
#include <unistd.h>

#include <QVBoxLayout>
#include <QApplication>
#include <QPushButton>
#include <QTime>
#include <QCoreApplication>
#include <QGroupBox>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>


#include "main.hh"

QByteArray sha1(QByteArray byteArray)
{
	if(!QCA::isSupported("sha1"))
	{
		qDebug() << "SHA1 not supported!\n";
		return QByteArray();
	}
    else
    {
    	QCA::SecureArray secByteArray = byteArray;
        return QCA::Hash("sha1").hash(secByteArray).toByteArray().toHex();
    }
}

QByteArray hashPartialBlock(QString name, quint32 index, QTime time, QByteArray prev_hash, quint64 rand)
{
	QByteArray byteArray;
	byteArray.append(name.toAscii());
	byteArray.append(QByteArray::number(index));
	byteArray.append(time.toString());
	byteArray.append(prev_hash);
	byteArray.append(QString::number(rand));
	return sha1(byteArray);
}

QByteArray hashBlock(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap data, quint64 rand)
{
	QByteArray byteArray;
	byteArray.append(name.toAscii());
	byteArray.append(QByteArray::number(index));
	byteArray.append(time.toString());
	byteArray.append(prev_hash);

	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	out << data;

	byteArray.append(dataArray);
	byteArray.append(QString::number(rand));
	return sha1(byteArray);
}

bool verifyHash(Block block)
{
	return hashPartialBlock(block.name, block.index, block.time, block.prev_hash, block.rand).startsWith(QString("000000").toAscii());
}

inline bool verifyHash(QByteArray hash)
{
	return hash.startsWith(QString("000000").toAscii());
}

NewBlockThread::NewBlockThread(NetSocket* sock, QVector<Block*> *blockchain, QString newName, QVariantMap *data)
{
	this->contCompFlag = true;
	this->quitThreadFlag = false;
	this->sock = sock;
	this->blockchain = blockchain;
	this->newName = newName;
    this->data = data;
}

// void NewBlockThread::setCurIndex(quint32 index)
// {
//     this->lastIndex = index;
// }


// void NewBlockThread::updateLastBlockHash(QByteArray lastBlockHash)
// {
// 	this->lastBlockHash = lastBlockHash;
// }

void NewBlockThread::setContFlag(bool flag)
{
	this->contCompFlag = flag;
}

void NewBlockThread::setQuitThreadFlag(bool flag)
{
	this->quitThreadFlag = flag;
}

Block* NewBlockThread::createNewBlock(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap* data)
{
	quint64 i = qrand();
	static Block *nullBlock = new Block();

	qDebug() << "trying to find "<< blockchain->size() + 1;

	while(contCompFlag)
	{
		if(verifyHash(hashPartialBlock(name, index, time, prev_hash, i)))
		{
			Block *newBlock = new Block(name, index, time, prev_hash, *data, hashBlock(name, index, time, prev_hash, *data, i), i);
			qDebug() << "found new block";//<< i;
			return newBlock;
		}
		i++;
	}
	return nullBlock;
}

void NewBlockThread::run()
{
	while(!quitThreadFlag)
	{
		if(contCompFlag)
		{
			Block* newBlock = createNewBlock(newName, blockchain->size() + 1, QTime::currentTime(), blockchain->last()->hash, data);
			//qDebug() << "cur inner blockchainsize: " << blockchain->size() << QTime::currentTime() << blockchain->last()->hash;
			//Block* newBlock = createNewBlock(newName, lastIndex, QTime::currentTime(), lastBlockHash, data);
			qDebug() << !newBlock->isNull()
				<< (newBlock->index == blockchain->size() + 1) << (newBlock->prev_hash == blockchain->last()->hash)
				<< verifyHash(hashPartialBlock(newBlock->name, newBlock->index, newBlock->time, newBlock->prev_hash, newBlock->rand))
				<< (newBlock->hash == hashBlock(newBlock->name, newBlock->index, newBlock->time, newBlock->prev_hash, newBlock->data, newBlock->rand));
			//qDebug() << "newblockinx: " << newBlock->index;
			if( !newBlock->isNull()
				&& newBlock->index == blockchain->size() + 1 && newBlock->prev_hash == blockchain->last()->hash
				&& verifyHash(hashPartialBlock(newBlock->name, newBlock->index, newBlock->time, newBlock->prev_hash, newBlock->rand))
				&& newBlock->hash == hashBlock(newBlock->name, newBlock->index, newBlock->time, newBlock->prev_hash, newBlock->data, newBlock->rand))
			{
				data->clear();
				blockchain->append(newBlock);
				sock->sendNewBLOCKToNeighbors(*newBlock);

				QFile file("/home/accts/yj252/Downloads/block" + QString::number(sock->getPortNum()) + ".js");
				if(!file.open(QIODevice::ReadWrite))
				{
					qDebug() << "fail to open file";
					return;
				}

				QTextStream out(&file);

				QString all = out.readAll();

				file.resize(0);
				
				QString dataString;

				QMapIterator<QString, QVariant> i(newBlock->data);
				while (i.hasNext()) {
				    i.next();
				    QString transac = i.key();

				    dataString.append(transac + '\t');
				}
				dataString.chop(1);

				quint32 inx = all.lastIndexOf("];");
				all.insert(inx, "{\n\tname: " + newBlock->name + ",\n\tindex: " + QString::number(newBlock->index)
					 + ",\n\ttime: " + newBlock->time.toString() + ",\n\tprev_hash: " + newBlock->prev_hash+ ",\n\tdata: " + dataString
					 + ",\n\thash: " + newBlock->hash + ",\n\trand: " + QString::number(newBlock->rand)
					 + "\n},\n");

				out << all;

				file.close();
			}
		}
	}
}

PrivateChatWindow::PrivateChatWindow(QString *privateMsg, bool* iCl)
{
	setWindowTitle("Private Message");

	this->privateMsg = privateMsg;
	this->isClicked = iCl;

	textinput = new QTextEdit(this);
	btnSend = new QPushButton("&Send", this);

	QVBoxLayout *privateChatWindowLayout = new QVBoxLayout();
	privateChatWindowLayout->addWidget(textinput);
	privateChatWindowLayout->addWidget(btnSend);

	setLayout(privateChatWindowLayout);
	textinput->setFocus();
	connect(btnSend, SIGNAL(clicked()), this, SLOT(quit()));
}

PrivateDownloadWindow::PrivateDownloadWindow(QString* dSHA1, QString* dName, bool* iCl)
{
	resize(350,100);

	this->downloadSHA1 = dSHA1;
	this->downloadFileName = dName;
	this->isClicked = iCl;

	sha1input = new QLineEdit(this);
	nameinput = new QLineEdit(this);
	btnSubmit = new QPushButton("&Submit", this);

	sha1input->setPlaceholderText("Enter sha-1 hash here...");
	nameinput->setPlaceholderText("Enter file name here...");

	QVBoxLayout *privateDownloadWindowLayout = new QVBoxLayout();
	privateDownloadWindowLayout->addWidget(sha1input);
	privateDownloadWindowLayout->addWidget(nameinput);
	privateDownloadWindowLayout->addWidget(btnSubmit);

	setLayout(privateDownloadWindowLayout);
	connect(btnSubmit, SIGNAL(clicked()), this, SLOT(quit()));
}

PrivateSearchWindow::PrivateSearchWindow(QString* sString, bool* iCl)
{
	resize(350, 80);

	this->searchString = sString;
	this->isClicked = iCl;

	stringinput = new QLineEdit(this);
	btnSearch = new QPushButton("&Search", this);

	stringinput->setPlaceholderText("Enter text here...");

	QVBoxLayout *privateSearchWindowLayout = new QVBoxLayout();
	privateSearchWindowLayout->addWidget(stringinput);
	privateSearchWindowLayout->addWidget(btnSearch);

	setLayout(privateSearchWindowLayout);
	connect(btnSearch, SIGNAL(clicked()), this, SLOT(quit()));
}

void PrivateChatWindow::quit()
{
	*privateMsg = textinput->toPlainText();
	textinput->clear();
	*isClicked = true;
	close();
}

void PrivateDownloadWindow::quit()
{
	*downloadSHA1 = sha1input->text();
	*downloadFileName = nameinput->text();
	sha1input->clear();
	nameinput->clear();
	*isClicked = true;
	close();
}

void PrivateSearchWindow::quit()
{
	*searchString = stringinput->text();
	stringinput->clear();
	*isClicked = true;
	close();
}
/*
void privateChatWindow::set(QString dest, QString origin, const QHostAddress addr, quint16 port)
{
	this->dest = dest;
	this->origin = origin;
	this->addr = addr;
	this->port = port;
}

void privateChatWindow::sendPrivateMsg()
{
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("Dest", dest);
	dataMap.insert("Origin",origin);
	dataMap.insert("ChatText",textinput->toPlainText());
	dataMap.insert("HopLimit",10);
	//no seqno
	out << dataMap;
	//qDebug() << "sent: "<< origin<<" "<<seq<<" "<<text;
	QUdpSocket *sock = new QUdpSocket();
	sock->writeDatagram(dataArray,addr,port);

	//ChatDialog::textview->append("Me to " + dest + ":");
	//ChatDialog::textview->append(textinput->toPlainText());
	textinput->clear();
	close();
}
*/

ChatDialog::ChatDialog()
{	
	seqNo = 1;
	inquiryInx = 0;
	noForward = false;
	cIsClicked = false;
	dIsClicked = false;

	setWindowTitle("Peerster");

	sock = new NetSocket(&peerList);
	privateChatWindow = new PrivateChatWindow(&privateMsg, &cIsClicked);
	privateDownloadWindow = new PrivateDownloadWindow(&downloadSHA1, &downloadFileName, &dIsClicked);
	privateSearchWindow = new PrivateSearchWindow(&searchString, &sIsClicked);
	// Read-only text box where we display messages from everyone.
	// This widget expands both horizontally and vertically.
	textview = new QTextEdit(this);
	textview->setReadOnly(true);

	// Small text-entry box the user can enter messages.
	// This widget normally expands only horizontally,
	// leaving extra vertical space for the textview widget.
	//
	// You might change this into a read/write QTextEdit,
	// so that the user can easily enter multi-line messages.
	textinput = new QTextEdit(this);
	//privateTextInput = new QTextEdit(this);
	//privateTextInput->setReadOnly(true);

	//! must add '&'
	btnSend = new QPushButton("&Send",this);
	btnShareFile = new QPushButton("&Share File...", this);
	btnDownload = new QPushButton("&Download", this);
	btnSearch = new QPushButton("&Search", this);
	btnAddPeer = new QPushButton("&Add Peer",this);
	//btnSendPrivateMsg = new QPushButton("&Send Private Message",this);
	listWidget = new QListWidget(this);
	listWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
	listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	listWidget->setSortingEnabled(true);

	fileWidget = new QListWidget(this);

	searchResult = new SearchResult();

	// Lay out the widgets to appear in the main window.
	// For Qt widget and layout concepts see:
	// http://doc.qt.nokia.com/4.7-snapshot/widgets-and-layouts.html


	QHBoxLayout *mainLayout = new QHBoxLayout();

	QGroupBox *qLeftGroupBox = new QGroupBox();
	QGroupBox *qRightGroupBox = new QGroupBox();
	QVBoxLayout *leftVLayout = new QVBoxLayout();
	QVBoxLayout *rightVLayout = new QVBoxLayout();

	QGroupBox *btnGroupBox = new QGroupBox();
	QHBoxLayout *btnLayout = new QHBoxLayout();

	btnLayout->addWidget(btnShareFile);
	btnLayout->addWidget(btnDownload);
	btnLayout->addWidget(btnSearch);
	btnGroupBox->setLayout(btnLayout);

	leftVLayout->addWidget(textview);
	leftVLayout->addWidget(textinput);
	leftVLayout->addWidget(btnSend);
	leftVLayout->addWidget(btnGroupBox);
	leftVLayout->addWidget(btnAddPeer);
	qLeftGroupBox->setLayout(leftVLayout);

	rightVLayout->addWidget(listWidget);
	rightVLayout->addWidget(fileWidget);
	qRightGroupBox->setLayout(rightVLayout);
/*
	QGroupBox *qRightGroupBox = new QGroupBox();
	QVBoxLayout *rightVLayout = new QVBoxLayout();

	rightVLayout->addWidget(listWidget);
	rightVLayout->addWidget(privateTextInput);
	rightVLayout->addWidget(btnSendPrivateMsg);
	qRightGroupBox->setLayout(rightVLayout);
*/
	mainLayout->addWidget(qLeftGroupBox);
	mainLayout->addWidget(qRightGroupBox);
	setLayout(mainLayout);
	//!
	textinput->setFocus();

	connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(popUpPrivateChatWindow(QListWidgetItem*)));
	connect(fileWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(startDownloadingFile(QListWidgetItem*)));
	// Register a callback on the textline's returnPressed signal
	// so that we can send the message entered by the user.
  	qsrand(QTime::currentTime().msec());
  	name = "Y.Jiang" + QString::number(qrand()%1000000);
  	textview->append("My Identifier:");
  	textview->append(name);

  	unsigned int myPortMin = 32768 + (getuid() % 4096)*4;
  	unsigned myPortMax = myPortMin + 3;
  	peerList.append(new Peer("", QHostAddress::LocalHost, sock->getPortNum()));
  	for(unsigned int portInx = myPortMin; portInx <= myPortMax; portInx++)
  	{
  		if(portInx!=sock->getPortNum())
    	{
      	peerList.append(new Peer("",QHostAddress::LocalHost,portInx));
    	}  
  	}
  	msgList.append(new MsgRcvd(name));

  	for(quint16 i = 1; i < peerList.size(); i++)
  	{
  		sock->sendRouteRumor(name, seqNo ,peerList[i]->IPAddress, peerList[i]->UDPPortNum, 0, 0);
  	}
  	msgList[0]->msg.append(QString());
  	seqNo++;

  	QTimer* msgTimer = new QTimer();
  	msgTimer->setSingleShot(true);
  	msgTimerList.append(new QTimer());
  	msgTimer->start(2000);

  	inquiryTimer = new QTimer();
  	connect(inquiryTimer, SIGNAL(timeout()), this, SLOT(sendInquiry()));
  	inquiryTimer->start(2000);

  	routeRumorTimer = new QTimer();
  	fileInquiryTimer = new QTimer();
  	connect(routeRumorTimer, SIGNAL(timeout()), this, SLOT(sendPeriodicRouteRumor()));
  	connect(fileInquiryTimer, SIGNAL(timeout()), this, SLOT(sendPeriodicFileInquiry()));
  	routeRumorTimer->start(10000);
  	fileInquiryTimer->start(10000);

  	searchTimer = new QTimer();
  	connect(searchTimer, SIGNAL(timeout()), this, SLOT(updateSearchResult()));

  	connect(btnSend, SIGNAL(clicked()), this, SLOT(gotSubmitPressed()));
  	connect(btnShareFile, SIGNAL(clicked()), this, SLOT(popUpFileSelection()));
  	connect(btnDownload, SIGNAL(clicked()), this, SLOT(popUpFileDownload()));
  	connect(btnSearch, SIGNAL(clicked()), this, SLOT(popUpPrivateSearchWindow()));
  	connect(btnAddPeer, SIGNAL(clicked()), this, SLOT(gotAddPeerPressed()));
  	connect(sock ,SIGNAL(readyRead()),this,SLOT(readPendingDatagrams()));

  	Block* genesis = getGenesisBlock();
  	blockchain.append(genesis);
  	if(!verifyHash(*genesis))
  	{
  		qDebug() << "Verification of Genesis Failed.";
  	}
  	//Block* newBlock = createNewBlock("New Block", blockchain.size() + 1, QTime::currentTime(), blockchain.last()->hash, QMap<QString, QVariant>());

  	// qDebug() << "veriGen " << verifyHash(*genesis);
  	// qDebug() << "veriNew" << verifyHash(*newBlock);
  // 	if(sock->getPortNum() == 40108)
  // 	{
	 //  	for(int i = 0; i < 10; i++)
		// {
		// 	Block* newBlock = createNewBlock("New Block", blockchain.size() + 1, QTime::currentTime(), blockchain.last()->hash, QMap<QString, QVariant>());
		// 	blockchain.append(newBlock);
		// 	sendNewBLOCKToNeighbors(*newBlock, QHostAddress::Null, 0);
		// }
  // 	}

  	QFile file("/home/accts/yj252/Downloads/block" + QString::number(sock->getPortNum()) + ".js");
	if(!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "fail to open file";
		return;
	}
	file.resize(0);

	QTextStream out(&file);

	out << "const arrayOfBlocks = [\n\n];\n\nmodule.exports = arrayOfBlocks;";
	file.close();
	
	//qDebug() << out.readLine();
	//qDebug() << out.readLine();

  	rsa = new RSA();

  	nBT = new NewBlockThread(sock, &blockchain, QString::number(rsa->getPubKey()) + ":" + QString::number(rsa->getN()), &transacData);
  	//nBT->start();
  	// for(int i = 0;i<12300045;i++)
  	// {
  	// 	i*qrand();
  	// }
  	//qDebug() << "before quit";
  	//nBT->setContFlag(false);
  	//nBT->quit();
  	//nBT->setContFlag(true);
  	//qDebug() << "after quit";
  	// for(int i = 0;i<12030045;i++)
  	// {
  	// 	i*qrand();
  	// }
  	// nBT->start();

  /*
  QString x= "viper.zoo.cs.yale.edu";
	QHostInfo info = QHostInfo::fromName(x);
	qDebug() << info.addresses()[0];
	sendMsg("testmsg","me",1,QHostAddress("128.36.232.39"),40108);*/

	blockInitialized = false;

	dataTimer = new QTimer();
	dataTimer->start(qrand()%5000 + 3000);

	connect(dataTimer, SIGNAL(timeout()), this, SLOT(genTrans()));

	// initializeRSA();
}

// void ChatDialog::initializeRSA()
// {
// 	qDebug() << "sha1: "<< QCA::isSupported("sha1");
// 	qDebug() << "md5: "<< QCA::isSupported("md5");
// 	qDebug() << "sha256: "<< QCA::isSupported("sha256");
// 	qDebug() <<QCA::PKey::supportedIOTypes();
// 	if(!QCA::isSupported("pkey") ||
//        !QCA::PKey::supportedIOTypes().contains(QCA::PKey::RSA))
//         qDebug() << "RSA not supported!";
//     else {
//         // When creating a public / private key pair, you make the
//         // private key, and then extract the public key component from it
//         // Using RSA is very common, however DSA can provide equivalent
//         // signature/verification. This example applies to DSA to the
//         // extent that the operations work on that key type.

//         // QCA provides KeyGenerator as a convenient source of new keys,
//         // however you could also import an existing key instead.
//         seckey = QCA::KeyGenerator().createRSA(1024);
//         if(seckey.isNull()) {
//             qDebug() << "Failed to make private RSA key";
//         }

//         pubkey = seckey.toPublicKey();
//     }
// }

void ChatDialog::genTrans()
{

	QString transac = "(" + QTime::currentTime().toString();
	if(sock->getPortNum() == 40108)
	{
		transac.append(" : traffic at : XXX road : ");

		switch(qrand()%3)
		{
			case 0: transac.append("high"); break;
			case 1: transac.append("med"); break;
			case 2: transac.append("low"); break;
		}
		transac.append(")");
	}
	else if(sock->getPortNum() == 40109)
	{
		transac.append(" : video at : YYY ave. : ");
		transac.append(QString::number(qrand()) + ")");
	}
	else if(sock->getPortNum() == 40110)
	{
		transac.append(" : temperature at : ZZZ park : ");
		transac.append(QString::number(20 + qrand()%10) + " C)");
	}
	else
	{
		transac.append(" : other data at : WWW squre");
		transac.append(QString::number(qrand()) + ")");
	}
	//QList<quint32> transacSig = rsa->signMessage(transac);
	//QVariantList transacSig;// = rsa->signMessage(transac);

	//QVariant varTransacSig = QVariant::fromValue(transacSig);

	transacData.insert(transac, rsa->signMessage(transac));
	//qDebug() << "sent: " <<transac << rsa->signMessage(transac) << rsa->getPubKey() << rsa->verifySignature(transac, rsa->signMessage(transac), rsa->getPubKey());
	//qDebug() << "dataint: " << verifyDataInt(rsa->getPubKey(), transacData);
}

void ChatDialog::popUpPrivateChatWindow(QListWidgetItem *item)
{
	QString dest = item->text();
	privateChatWindow->exec();
	if(cIsClicked == true)
	{
		sock->sendPrivateMsg(dest, name, privateMsg, 10, routeTable.value(dest).first, routeTable.value(dest).second);
		textview->append("Sent Private MSG to " + dest + ":");
		textview->append(privateMsg);
		cIsClicked = false;
	}
}

void ChatDialog::popUpFileSelection()
{
	QStringList filePaths = QFileDialog::getOpenFileNames(this, "SelectedFiles", "/home/accts/yj252");
	for(quint16 i = 0; i < filePaths.size(); i++)
	{
		QString filePath = filePaths.at(i);
		QFileInfo fi(filePath);
		QString fileName = fi.fileName();

		File* fileLink = new File(name, fileName);
		fileList.append(fileLink);

		QFile file(filePath);

		QByteArray blockList;

		if(!file.open(QIODevice::ReadOnly))
		{
			qDebug() << "fail to open file";
			return;
		}

		while(true)
		{
			char data[8<<10];
			QByteArray byteArray;
			QByteArray hashByteArray;
			int err = file.read(data, 8<<10);
			if(err == 0 || err == -1)
			{
				break;
			}
			fileLink->blockSize++;
			fileLink->curBlockSize++;
			byteArray.append(data, err);
			//qDebug() << byteArray;
			//qDebug() << "block++";
			hashByteArray=sha1(byteArray);
			fileLink->dataMap.insert(hashByteArray,byteArray);
			blockList.append(hashByteArray);
		}
		file.close();
		QByteArray hashBlockList = sha1(blockList);
		fileLink->metadata = qMakePair(hashBlockList, blockList);
		textview->append("Shared File Path: " + filePath);
		textview->append("SHA1 of File: " + hashBlockList.toHex());
	}
}

void ChatDialog::popUpFileDownload()
{
	if(listWidget->selectedItems().size() > 0)
	{
		QString reqDest = listWidget->selectedItems()[0]->text();
		privateDownloadWindow->setWindowTitle("Downloading File from " + reqDest);
		privateDownloadWindow->exec();
		if(dIsClicked == true)
		{
			File* fileLink = new File(reqDest, downloadFileName);
			fileLink->metadata.first = QByteArray::fromHex(downloadSHA1.toAscii());
			fileLink->metadata.second = QByteArray();
			fileList.append(fileLink);
			sock->sendBlockRequest(reqDest, name, 10, fileLink->metadata.first, routeTable.value(reqDest).first, routeTable.value(reqDest).second);
			dIsClicked = false;
		}
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("Please select a peer");
		msgBox.exec();
	}
}

void ChatDialog::startDownloadingFile(QListWidgetItem *item)
{
	QString text = item->text();
	int inx = text.lastIndexOf(":");
	QString fName = text.left(inx);
	QString fOwner = text.mid(inx+1);

	for(quint32 i = 0; i< fileList.size(); i++)
	{
		if(fileList[i]->fileName == fName && fileList[i]->fileOwner == fOwner)
		{
			fileList[i]->isDownloadable = true;
			qDebug() << "setDownloadable true. origin: "<< fOwner << " name: " << fName;
			sock->sendBlockRequest(fOwner, name, 10, fileList[i]->metadata.first, routeTable.value(fOwner).first, routeTable.value(fOwner).second);
		}
	}
}

Peer* ChatDialog::getRandNeighbor(QHostAddress exceptSender = QHostAddress::Null, quint16 exceptSenderPort = 0)
{
  if(peerList.size() == 1 || (peerList.size() == 2 && peerList[1]->IPAddress == exceptSender && peerList[1]->UDPPortNum == exceptSenderPort))
  {
  	return 0;
  }
  quint16 rand = qrand()%(peerList.size() - 1) + 1;
  while(peerList[rand]->IPAddress == exceptSender && peerList[rand]->UDPPortNum == exceptSenderPort)
  {
  	rand = qrand()%(peerList.size() - 1) + 1;
  }
  return peerList[rand];
}
/*
void ChatDialog::updateInquiryInx()
{
  ++inquiryInx;
  if(inquiryInx == msgList.size())
  {
    inquiryInx  = 0 ;
  }
}
*/

void ChatDialog::sendStatusMsg(const QHostAddress addr, quint16 port, QString origin, quint16 seq)
{
	QVariantMap dataMap, nestedMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	if(origin == "" && seq ==0)
	{
		for(quint16 i=0;i<msgList.size();i++)
		{
			nestedMap.insert(msgList[i]->name,msgList[i]->msg.size()+1);
		}
	}
	else
	{
		nestedMap.insert(origin, seq);
	}
	dataMap.insert("Want",nestedMap);
	out << dataMap;

	// qDebug() << "sent want msg:" << dataMap;

	sock->writeDatagram(dataArray,addr,port);
}

void ChatDialog::sendSearchRequest(QString origin, QString searchString, quint32 budget, const QHostAddress addr, quint16 port)
{
	qDebug() << "sent SearchRequests";
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("Origin", origin);
	dataMap.insert("Search", searchString);
	dataMap.insert("Budget", budget);
	out << dataMap;
	sock->writeDatagram(dataArray, addr, port);
}

void ChatDialog::sendSearchRequestToNeighbors(QString origin, QString searchString, quint32 budget, QHostAddress exSender = QHostAddress::Null, quint16 exSenderPort = 0)
{
	quint32 peerSize = ((exSender == QHostAddress::Null && exSenderPort == 0) ? peerList.size() - 1: peerList.size() -2);
	if(peerSize <= 0)
	{
		return ;
	}
	quint32 aveNum = budget/peerSize;
	quint32 num = budget - aveNum*peerSize;
	quint32 cnt = 0;
	for(quint16 i = 1; i < peerList.size(); i++)
	{
		Peer* peer = peerList[i];
		if(!(peer->IPAddress == exSender && peer->UDPPortNum == exSenderPort))
		{	

			quint32 newBudget = cnt < num ? aveNum + 1 : aveNum;
			cnt++;
			if(newBudget > 0)
			{
				sendSearchRequest(origin, searchString, newBudget, peer->IPAddress, peer->UDPPortNum);
			}
		}
	}
}

void ChatDialog::updateSearchResult()
{
	quint16 bud = searchResult->updateBud();

	if(!searchResult->checkStatus())
	{
		sendSearchRequestToNeighbors(name, searchString, bud);
	}
	else
	{
		searchResult->reset();
		searchTimer->stop();
		btnSearch->setEnabled(true);
	}
}

void ChatDialog::popUpPrivateSearchWindow()
{
	privateSearchWindow->exec();

	if(sIsClicked == true)
	{
		btnSearch->setEnabled(false);

		sendSearchRequestToNeighbors(name, searchString, 2);

		searchResult->name = searchString;

  		searchTimer->start(1000);

  		//connect(searchTimer, SIGNAL(timeout()), this, SLOT(updateSearchResult()));

		textview->append("Searching for " + searchString);
		sIsClicked = false;
	}
}

void ChatDialog::sendSearchReply(QString dest, QString origin, quint32 ttl, QString sReply, QVariantList mNames, QVariantList mIDs, const QHostAddress addr, quint16 port)
{
	qDebug() << "sent SearchReply to "<< dest;
	QVariantMap dataMap;
	QByteArray dataArray;
	QDataStream out(&dataArray,QIODevice::WriteOnly);
	dataMap.insert("Dest", dest);
	dataMap.insert("Origin", origin);
	dataMap.insert("HopLimit", ttl);
	dataMap.insert("SearchReply", sReply);
	dataMap.insert("MatchNames", mNames);
	dataMap.insert("MatchIDs", mIDs);
	out << dataMap;
	sock->writeDatagram(dataArray,addr,port);
}

void ChatDialog::sendInquiry()
{
	Peer* p =getRandNeighbor();
  	sendStatusMsg(p->IPAddress, p->UDPPortNum);
  	sock->sendWantBLOCKMsg(p->IPAddress, p->UDPPortNum, blockchain.size() + 1);
  	// qDebug() << "sent both inquiries to: "<< p->IPAddress << p->UDPPortNum;
  	//updateInquiryInx();
}

void ChatDialog::sendPeriodicRouteRumor()
{
	Peer* p = getRandNeighbor();
	sock->sendRouteRumor(name, seqNo ,p->IPAddress, p->UDPPortNum);
	msgList[0]->msg.append(QString());
	seqNo++;

	QTimer* msgTimer = new QTimer();
  	msgTimer->setSingleShot(true);
  	msgTimerList.append(new QTimer());
  	msgTimer->start(2000);
	// qDebug() << "sent rumor to: "<< p->IPAddress << p->UDPPortNum;
}

void ChatDialog::sendPeriodicFileInquiry()
{
	for(quint32 i = 0; i < fileList.size(); i++)
	{
		File* fileLink = fileList.at(i);
		if((fileLink->blockSize > 0 && fileLink->curBlockSize == fileLink->blockSize) || !fileLink->isDownloadable)
		{
			continue;
		}
		QString reqDest = fileLink->fileOwner;
		if(fileLink->metadata.second.isNull())
		{
			sock->sendBlockRequest(reqDest, name, 10, fileLink->metadata.first, routeTable.value(reqDest).first, routeTable.value(reqDest).second);
		}
		else if(fileLink->curBlockSize < fileLink->blockSize)
		{
			for(auto k: fileLink->dataMap.keys())
			{
				if(fileLink->dataMap.value(k).isNull())
				{
					sock->sendBlockRequest(reqDest, name, 10, k, routeTable.value(reqDest).first, routeTable.value(reqDest).second);
				}
			}
		}
	}
}

int ChatDialog::getMsgInx(QString origin)
{
  int inx = 0;
  for(inx = 0; inx<msgList.size(); inx++)
  {
    if(msgList[inx]->name == origin)
    	return inx;
  }
  return -1;
}

quint16 ChatDialog::getCurMsgSize(QString origin)
{
  int inx = getMsgInx(origin);
  return inx==-1?0:msgList[inx]->msg.size();
}

void ChatDialog::gotSubmitPressed()
{
	// Initially, just echo the string locally.
	// Insert some networking code here...
  qDebug() << "FIX: send message to other peers: " << textinput->toPlainText();
  textview->append("Me:");
  textview->append(textinput->toPlainText());

  msgList[0]->msg.append(textinput->toPlainText());

  Peer* p =getRandNeighbor();
  sock->sendMsg(textinput->toPlainText(), name ,seqNo, p->IPAddress, p->UDPPortNum);

  seqNo++;

  QTimer* msgTimer = new QTimer();
  msgTimer->setSingleShot(true);
  msgTimerList.append(new QTimer());
  msgTimer->start(2000);


  //msgTimerList->start(2000);
  //! set timer for each dest
  // Clear the textline to get ready for the next input message.
  textinput->clear();
}

void ChatDialog::lookedUp(const QHostInfo & host)
{
	if (host.error() != QHostInfo::NoError) {
        qDebug() << "Lookup failed:" << host.errorString();
        textview->append("Lookup failed");
        return;
    }
    else
    {
        textview->append("Peer Added");
    }

    const auto addresses = host.addresses();
    for (const QHostAddress &address : addresses)
        qDebug() << "Found address:" << address.toString()<< "hostname: " << host.hostName();
    peerList.append(new Peer(host.hostName(),addresses[0],newPort));
}

void ChatDialog::gotAddPeerPressed()
{
	QString text = textinput->toPlainText();
	int inx = text.lastIndexOf(":");
	QString address = text.left(inx);
	newPort = text.mid(inx+1).toInt();
	QHostInfo::lookupHost(address, this, SLOT(lookedUp(QHostInfo)));
	textinput->clear();
}

int ChatDialog::updateMsgRcvd(QVariantMap dataMap,QHostAddress sender,quint16 senderPort)
{
	QString origin = dataMap.value("Origin").toString();
	quint16 seqNo = dataMap.value("SeqNo").toInt();
	
	int inx = getMsgInx(origin);

	bool cc = dataMap.contains("ChatText");

	bool cl = (dataMap.contains("LastIP") && dataMap.contains("lastPort"));

	if(inx >0)
	{
		if(seqNo - getCurMsgSize(origin) == 1)
		{
			if(cc)
			{
				msgList[inx]->msg.append(dataMap.value("ChatText").toString());
			}
			else
			{
				msgList[inx]->msg.append(QString());
			}
			if(cl)
			{
				msgList[inx]->lastIP = sender.toIPv4Address();
				msgList[inx]->lastPort = senderPort;
			}
			return seqNo + 1;
		}
		else
		{
			return -1;
		}
	}
	else if(inx == -1)
	{	
		peerList.append(new Peer("",sender,senderPort));
		msgList.append(new MsgRcvd(origin));
		if(seqNo == 1)
		{
			if(cc)
			{
				msgList[getMsgInx(origin)]->msg.append(dataMap.value("ChatText").toString());
			}
			else
			{
				msgList[getMsgInx(origin)]->msg.append(QString());
			}
			if(cl)
			{
				msgList[inx]->lastIP = sender.toIPv4Address();
				msgList[inx]->lastPort = senderPort;
			}
			return 2;
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

void ChatDialog::updateRouteTable(QString origin, const QHostAddress sender,quint16 senderPort)
{
	if(!routeTable.contains(origin) && origin != name)
	{
		listWidget->addItem(origin);
	}
	routeTable.insert(origin,qMakePair(sender,senderPort));
	//qDebug() << "new routeTable: " << routeTable;
}
//!
QByteArray ChatDialog::searchData(QByteArray b)
{
	for(quint32 i = 0; i < fileList.size(); i++)
	{
		File* fileLink = fileList.at(i);
		//qDebug() << "file metadata: " << fileLink->metadata.first.toHex();
		if(fileLink->metadata.first == b)
		{
			//qDebug() << "returned file blocklist: " << fileLink->metadata.second.toHex();
			return fileLink->metadata.second;
		}
		else if(fileLink->dataMap.contains(b))
		{
			return fileLink->dataMap.value(b);
		}
	}
	return QByteArray();
}

void ChatDialog::writeToFile(File* fileLink)
{
	QFile file("/home/accts/yj252/Downloads/" + fileLink->fileName);//assm all known
	if(!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "fail to open file";
		return;
	}
	QByteArray blockList = fileLink->metadata.second;
	for(quint32 j = 0; j < blockList.size(); j += 20)
	{
		file.write(fileLink->dataMap.value(blockList.mid(j, 20)));
	}
	file.close();
	textview->append("File " + fileLink->fileName + " has been written to /home/accts/yj252/Downloads/" + fileLink->fileName);
}

QByteArray ChatDialog::saveData(QByteArray hashData, QByteArray data, QString origin)
{
	for(quint32 i = 0; i < fileList.size(); i++)
	{
		File* fileLink = fileList.at(i);
		if((fileLink->blockSize > 0 && fileLink->curBlockSize == fileLink->blockSize) || fileLink->isDownloadable == false)
		{
			continue;
		}

		if(fileLink->metadata.first == hashData && fileLink->metadata.second.isNull())
		{
			fileLink->metadata.second = data;
			fileLink->fileOwner = origin;
			quint32 dataSize = data.size();
			for(quint32 j = 0; j < dataSize; j += 20)
			{
				fileLink->dataMap.insert(data.mid(j, 20), QByteArray());
				fileLink->blockSize++;
			}
		}
		else if(fileLink->dataMap.contains(hashData) && fileLink->dataMap.value(hashData).isNull())
		{
			fileLink->dataMap.insert(hashData, data);
			fileLink->curBlockSize++;
		}
		if(fileLink->curBlockSize < fileLink->blockSize)
		{
			for(auto k: fileLink->dataMap.keys())
			{
				if(fileLink->dataMap.value(k).isNull())
				{
					return k;
				}
			}
		}
		else if(fileLink->curBlockSize == fileLink->blockSize)
		{
			writeToFile(fileLink);
			fileLink->isDownloadable = false;
		}
		else
		{
			return QByteArray();
		}
	}
	return QByteArray();
}

Block* ChatDialog::getGenesisBlock()
{
	QByteArray hash = hashBlock("Genesis", 1, QTime(), QByteArray(), QMap<QString, QVariant>(), 16745122);
	Block* genesis = new Block("Genesis", 1, QTime(), QByteArray(), QMap<QString, QVariant>(), hash, 16745122);
	return genesis;
}

bool ChatDialog::canAppendToChain(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap data, QByteArray hash, quint64 rand)
{
	return (index == blockchain.size() + 1 && prev_hash == blockchain.last()->hash &&
    		verifyHash(hashPartialBlock(name, index, time, prev_hash, rand)) && hash == hashBlock(name, index, time, prev_hash, data, rand));
}

bool ChatDialog::canAppendToChain(Block block)
{
	return (block.index == blockchain.size() + 1 && block.prev_hash == blockchain.last()->hash &&
    		verifyHash(hashPartialBlock(block.name, block.index, block.time, block.prev_hash, block.rand)) &&
    		block.hash == hashBlock(block.name, block.index, block.time, block.prev_hash, block.data, block.rand));
}

bool ChatDialog::verifyDataInt(quint32 key, quint32 n, QVariantMap data)
{
	QMapIterator<QString, QVariant> i(data);
	while (i.hasNext()) {
	    i.next();
	    QString transac = i.key();
	    QVariantList encHash = i.value().toList();

	    //qDebug() << "received: " << transac <<encHash<< key << rsa->verifySignature(transac, encHash, key, n);
	    if(!rsa->verifySignature(transac, encHash, key, n))
	    {
	    	return false;
	    }
	}
	return true;
}

void ChatDialog::readPendingDatagrams()
{
	QVariantMap dataMap;
	QHostAddress sender;
	quint16 senderPort;
    while(sock->hasPendingDatagrams())
    {
      QByteArray dataBuffer;
      dataBuffer.resize(sock->pendingDatagramSize());
      sock->readDatagram(dataBuffer.data(), dataBuffer.size(), &sender, &senderPort);
      QDataStream in(&dataBuffer,QIODevice::ReadOnly);
      in >> dataMap;
    }
    //qDebug()<<"got msg from sender: "<<sender << " port:"<<senderPort;
   /* 
    for(int i=0;i<portNames.size();i++)
    {
      qDebug() << "port: "<< i << "msgs:";
      for(int j=0;j<msgRcvd[i].size();j++)
      {
        qDebug() << msgRcvd[i][j];
      }
    }
*/
    if(dataMap.contains("Hash"))
    {
    	QString name = dataMap.value("Name").toString();
    	quint32 index = dataMap.value("Index").toUInt();
    	QTime time = dataMap.value("Time").value<QTime>();
    	QByteArray prev_hash = dataMap.value("Prev_Hash").value<QByteArray>();
    	QVariantMap data = dataMap.value("Data").value<QVariantMap>();
    	QByteArray hash = dataMap.value("Hash").value<QByteArray>();
    	quint64 rand = dataMap.value("Rand").toUInt();

    	int inx = name.lastIndexOf(":");

    	quint32 pubkey = name.left(inx).toUInt();
    	quint32 pubn = name.mid(inx+1).toUInt();

    	//qDebug() << "received: " << data << name.toUInt() << verifyDataInt(rsa->getPubKey(), transacData);

    	if(verifyDataInt(pubkey, pubn, data) && canAppendToChain(name, index, time, prev_hash, data, hash, rand))
    	{
    		Block* newBlock = new Block(name, index, time, prev_hash, data, hash, rand);
    		nBT->setContFlag(false);
    		nBT->setQuitThreadFlag(true);
    		nBT->quit();
    		while(nBT->isRunning());
    		//qDebug() << "cur outer blockchainsize: " << blockchain.size();
    		blockchain.append(newBlock);
    		//fiexme update data and name
    		//qDebug() << "cur outer blockchainsize after update: " << blockchain.size();
    		nBT->setContFlag(true);
    		nBT->setQuitThreadFlag(false);
    		nBT->start();

    		textview->append("New Block #" + QString::number(index) + " from " + name + " Accepted");
    		//qDebug() << "New Block Data: " << newBlock->data;
    		sock->sendNewBLOCKToNeighbors(*newBlock, sender, senderPort);

    		QFile file("/home/accts/yj252/Downloads/block" + QString::number(sock->getPortNum()) + ".js");
			if(!file.open(QIODevice::ReadWrite))
			{
				qDebug() << "fail to open file";
				return;
			}

			QTextStream out(&file);

			QString all = out.readAll();

			file.resize(0);
			
			QString dataString;

			QMapIterator<QString, QVariant> i(newBlock->data);
			while (i.hasNext()) {
			    i.next();
			    QString transac = i.key();

			    dataString.append(transac + '\t');
			}
			dataString.chop(1);

			quint32 inx = all.lastIndexOf("];");
			all.insert(inx, "{\n\tname: " + newBlock->name + ",\n\tindex: " + QString::number(newBlock->index)
				 + ",\n\ttime: " + newBlock->time.toString() + ",\n\tprev_hash: " + newBlock->prev_hash+ ",\n\tdata: " + dataString
				 + ",\n\thash: " + newBlock->hash + ",\n\trand: " + QString::number(newBlock->rand)
				 + "\n},\n");

			out << all;

				file.close();
    	}
    	else
    	{	
    		// textview->append("New Block #" + QString::number(index) + " from " + name + " Rejected");
    	}
    }
    else if(dataMap.contains("WantBLOCK"))
    {
    	quint32 seq = dataMap.value("WantBLOCK").toUInt();

	    int diff = seq - blockchain.size();

		if(diff > 1)
		{
			sock->sendWantBLOCKMsg(sender, senderPort, blockchain.size() + 1);
		}
		else if(diff < 1)
		{
			sock->sendNewBLOCK(*(blockchain.at(seq - 1)), sender, senderPort);
		}
		if(diff == 1 && !blockInitialized)
		{
			nBT->start();
			blockInitialized = true;
		}
    }
	else if(dataMap.contains("Want"))
	{
        QVariantMap qMap = dataMap.value("Want").value<QVariantMap>();
        for(quint16 i =0; i < qMap.keys().size() ; i++)
        {
	        QString origin = qMap.keys()[i];
	        quint16 seq = qMap.value(origin).toInt();

		    int diff = seq - getCurMsgSize(origin);
		    //qDebug() << "got want msg for " << dataMap.value("Want") << " from "<< sender << " "<<senderPort;
		    //qDebug() << "seq curMsgSize diff="<<seq<<getCurMsgSize(origin)<< diff;
			if(diff > 1)
			{
				//qDebug() <<"origin: "<< origin << getCurMsgSize(origin) + 1;
				sendStatusMsg(sender, senderPort, origin, getCurMsgSize(origin) + 1);
			}
			else if(diff < 1)
			{
				//qDebug() << "msg: "<<msgList[getMsgInx(origin)]->msg[seq-1]<<" origin "<<origin<<" seq "<<seq<<" sender " <<sender <<" port "<<senderPort;
				QString msgString = msgList[getMsgInx(origin)]->msg[seq-1];
				quint32 lastIP = msgList[getMsgInx(origin)]->lastIP;
				quint16 lastPort = msgList[getMsgInx(origin)]->lastPort;
				if(msgString.isNull())
				{
					sock->sendRouteRumor(origin, seq, sender, senderPort, lastIP, lastPort);
				}
				else if(!noForward)
				{
					sock->sendMsg(msgString, origin, seq, sender, senderPort, lastIP, lastPort);
				}
			}
		    else if(qrand()%2==0 && origin == name && msgTimerList[seq-2]->isActive() && getCurMsgSize(origin) > 0)
		    {
		    	//qDebug() << "isActive????????"<<msgTimerList[seq-2]->isActive();
		    	quint32 lastIP = msgList[getMsgInx(origin)]->lastIP;
				quint16 lastPort = msgList[getCurMsgSize(origin)]->lastPort;
		    	Peer* p = getRandNeighbor(sender, senderPort);
		    	if(p != 0)
		    	{
		    		QString msgString = msgList[getMsgInx(origin)]->msg[getCurMsgSize(origin)-1];
		    		if(msgString.isNull())
		    		{
		    			sock->sendRouteRumor(origin, seq-1, p->IPAddress, p->UDPPortNum, lastIP, lastPort);
		    		}
		    		else if(!noForward)
		    		{
		    			sock->sendMsg(msgString, origin, seq-1, p->IPAddress, p->UDPPortNum, lastIP, lastPort);
		    		}
		    	}
		    }
        }

	}
	else if(dataMap.contains("Dest"))
	{
		QString dest = dataMap.value("Dest").toString();
		quint32 ttl = dataMap.value("HopLimit").toInt();
		QString origin = dataMap.value("Origin").toString();
		bool cc = dataMap.contains("ChatText");
		bool crq = dataMap.contains("BlockRequest");
		bool crp = dataMap.contains("BlockReply");
		bool csrp = dataMap.contains("SearchReply");
		if(dest == name)
		{
			if(cc)
			{
				textview->append("New Private MSG from " + origin + ":");
				textview->append(dataMap.value("ChatText").toString());
			}
			else if(crq)
			{
				qDebug() << "got BlockRequest from " << origin;
				QByteArray data = searchData(dataMap.value("BlockRequest").value<QByteArray>());
				if(!data.isNull())
				{
					sock->sendBlockReply(origin, name, 10, sha1(data), data, sender , senderPort);
				}
			}
			else if(crp)
			{
				qDebug() << "got BlockReply from " << origin;
				QByteArray hashData = dataMap.value("BlockReply").value<QByteArray>();
				QByteArray data = dataMap.value("Data").value<QByteArray>();
				if(sha1(data) == hashData)
				{
					QByteArray reqByteArray = saveData(hashData, data, origin);
					if(!reqByteArray.isNull())
					{
						sock->sendBlockRequest(origin, name, 10, reqByteArray, sender, senderPort);
					}
				}
				else
				{
					qDebug() << "error: hash does not match";
				}
			}
			else if(csrp)
			{
				qDebug() << "got SearchReply from " << origin;
				QString searchReply = dataMap.value("SearchReply").toString();
				QVariantList matchNames = dataMap.value("MatchNames").value<QVariantList>();
				QVariantList matchIDs = dataMap.value("MatchIDs").value<QVariantList>();

				for(quint32 i = 0; i < matchNames.size(); i++)
				{
					QString fileName = matchNames[i].toString();
					int inx = fileName.lastIndexOf("/");
					fileName = fileName.mid(inx+1);

					QByteArray hashBlockList = matchIDs[i].value<QByteArray>();
					bool f = false;
					for(quint32 j = 0; j < fileList.size(); j++)
					{
						if(fileList[j]->fileOwner == origin && fileList[j]->fileName == fileName)
						{
							f = true;
							break;
						}
					}
					if(f == false)
					{
						File* fileLink = new File(origin, fileName);
						fileLink->metadata.first = hashBlockList;
						fileLink->metadata.second = QByteArray();
						fileLink->isDownloadable = false;
						fileList.append(fileLink);
						fileWidget->addItem(fileLink->fileName + ":" + origin);
						searchResult->updateFile(1);
					}
					//sock->sendBlockRequest(origin, name, 10, hashBlockList, routeTable.value(origin).first, routeTable.value(origin).second);
				}
			}
		}
		else if(ttl > 0 && routeTable.contains(dest) && !noForward)
		{
			qDebug() << "forwarding to: "<< dest;
			QHostAddress addr = routeTable.value(dest).first;
			quint16 port = routeTable.value(dest).second;
			if(cc)
			{
				sock->sendPrivateMsg(dest, origin, dataMap.value("ChatText").toString(), ttl - 1, addr, port);
			}
			else if(crq)
			{
				sock->sendBlockRequest(dest, origin, ttl - 1, dataMap.value("BlockRequest").value<QByteArray>(), addr, port);
			}
			else if(crp)
			{
				sock->sendBlockReply(dest, origin, ttl-1, dataMap.value("BlockReply").value<QByteArray>(), dataMap.value("Data").value<QByteArray>(), addr, port);
			}
			else if(csrp)
			{
				QString searchReply = dataMap.value("SearchReply").toString();
				QVariantList matchNames = dataMap.value("MatchNames").value<QVariantList>();
				QVariantList matchIDs = dataMap.value("MatchIDs").value<QVariantList>();
				sendSearchReply(dest, origin, ttl-1, searchReply, matchNames, matchIDs, addr, port);
			}
		}
		updateRouteTable(origin, sender, senderPort);
	}
	else if(dataMap.contains("Origin") && dataMap.contains("SeqNo"))
	{
	    QString origin = dataMap.value("Origin").toString();
	    quint16 seq = dataMap.value("SeqNo").toInt();
	    quint16 seqBeforeUpdate = getCurMsgSize(origin);
	    int ud = updateMsgRcvd(dataMap,sender,senderPort);
	    bool cc = dataMap.contains("ChatText");
	    bool cl = (dataMap.contains("LastIP") && dataMap.contains("LastPort"));
	    //qDebug() << "got message from" << sender << " " << senderPort;
	  	//qDebug() << "text: "<<text<<" origin: "<< origin << " seqNo: "<<seq;
	    if(cc && ud>0)
	    {
	      textview->append("New MSG from " + origin + ":");
	      textview->append(dataMap.value("ChatText").toString());
	    }

	    sendStatusMsg(sender, senderPort, origin, getCurMsgSize(origin) + 1);
	    //qDebug() << "received: " << (dataMap.contains("ChatText")? dataMap.value("ChatText").toString(): "route rumor") << " from: "<<origin<< seq;
	    //qDebug() << "cc: " << QString::number(cc) << " cl: "<< QString::number(cl);

	    if(cc && !noForward)
  		{
  			Peer* p =getRandNeighbor(sender, senderPort);
  			if(p != 0)
  			{
  				sock->sendMsg(dataMap.value("ChatText").toString(), origin ,seq, p->IPAddress, p->UDPPortNum, sender.toIPv4Address(), senderPort);
  			}
  		}
  		else if(origin != name && !(routeTable.value(origin).first == sender && routeTable.value(origin).second == senderPort))
  		{
  			//qDebug() << "forwarding to neighbors";
  			for(int i = 0; i < routeTable.values().size(); i++)
  			{
  				QHostAddress addr = routeTable.values().at(i).first;
  				quint16 port = routeTable.values().at(i).second;
  				if(!(addr == sender && port == senderPort))
  				{
  					qDebug() << addr << " "<< port;
  					sock->sendRouteRumor(origin, seq, addr, port, sender.toIPv4Address(), senderPort);
  				}
  			}
  		}

	  	if(seq > seqBeforeUpdate)
	  	{
	  		if(cl)
	  		{
	  			qDebug() << "updated with lastIP lastPort = "<< dataMap.value("LastIP").value<quint32>() <<" "<<dataMap.value("LastPort").value<quint16>();
	  			updateRouteTable(origin, QHostAddress(dataMap.value("LastIP").value<quint32>()), dataMap.value("LastPort").value<quint16>());
	  		}
	  		else
	  		{
	  			updateRouteTable(origin, sender, senderPort);
	  		}
	  	}
	  	else if(seq == seqBeforeUpdate && !cl)
	  	{
	  		updateRouteTable(origin, sender, senderPort);
	  	}
    }
    else if(dataMap.contains("Search") && dataMap.contains("Budget"))
    {
    	QString origin = dataMap.value("Origin").toString();
    	QString searchString = dataMap.value("Search").toString();
    	quint32 budget = dataMap.value("Budget").toUInt();

    	qDebug() << "got SearchRequest from " << origin;

    	QVariantList mNames;
    	QVariantList mIDs;
    	for(quint32 i = 0; i < fileList.size(); i++)
    	{
    		if(fileList[i]->fileName.contains(searchString))
    		{
    			mNames.append(fileList[i]->fileName);
    			mIDs.append(fileList[i]->metadata.first);
    		}
    	}
    	if(mNames.size() > 0)
    	{
    		sendSearchReply(origin, name, 10, searchString, mNames, mIDs, routeTable.value(origin).first, routeTable.value(origin).second);
    	}
    	else if(budget > 1)
    	{
    		//qDebug()<<"forward: bud="<< budget-1;
    		sendSearchRequestToNeighbors(origin, searchString, budget - 1, sender, senderPort);
    	}
    }
}

void ChatDialog::addPeerFromCMD(QString arg)
{
	int inx = arg.lastIndexOf(":");
	QString address = arg.left(inx);
	newPort = arg.mid(inx+1).toInt();
	QHostInfo::lookupHost(address, this, SLOT(lookedUp(QHostInfo)));
}

void ChatDialog::setNoForward(bool b)
{
	this->noForward=b;
	textview->append("Set noForward to " + QString::number(noForward));
}

void ChatDialog::closeEvent(QCloseEvent *event)
{
	nBT->setContFlag(false);
	nBT->setQuitThreadFlag(true);
	nBT->quit();
    // qDebug() << "closeEvent";
}

int main(int argc, char **argv)
{
	QCA::Initializer qcainit;
	// Initialize Qt toolkit
	QApplication app(argc,argv);

	// Create an initial chat dialog window
	
	ChatDialog dialog;
	dialog.show();


	//qDebug() << sha1(QByteArray("hello")).toHex();
	/*
	QString x = "5432";
	QByteArray y = QByteArray::fromHex(x.toAscii());
	qDebug() << y.toHex();*/

	/*
	QByteArray byteArray;
	QVariantMap variantMap;

	variantMap.insert("test1",1);
	variantMap.insert("test2",2);
	qDebug() << variantMap.value("test1").value<int>();
	*/
	//QDataStream  out(&byteArray,QIODevice::WriteOnly);

	// Create a UDP network socket
    	// Enter the Qt main loop; everything else is event driven

	// quint64 i = 0;
	// while(true)
	// {
	// 	QByteArray b = hashPartialBlock("Genesis", 1, QTime(), QByteArray(), i);
	// 	//qDebug() << b << i;
	// 	if(verifyHash(b))
	// 	{
	// 		qDebug() <<"the i: "<< i;
	// 		break;
	// 	}
	// 	i++;
	// }

	QStringList args = QCoreApplication::arguments();
	if(args.size() == 2)
	{
		QString arg1 = args.at(1);
		if(arg1 == "-noforward")
		{
			dialog.setNoForward(true);
		}
		else
		{
			dialog.addPeerFromCMD(args.at(1));
		}
	}
	//qDebug() << QCA::RSAPrivateKey::RSAPrivateKey();

	// qDebug() << "testRSA";
	//RSA rsa;
	// QVariantList msg = rsa.encrypt("HelloThisisYuang Horray!!!");
	// qDebug() << "en: " << msg;
	// QString de = rsa.decrypt(msg);
	// qDebug() << "de: " <<de;

	// qDebug() << "RSA sha1: " << rsa.sha1("Hello");
	//qDebug() << "verifysig: " << rsa.verifySignature("hi!asdasdasd", rsa.signMessage("hi!asdasdasd"), rsa.getPubKey());

	return app.exec();
}

