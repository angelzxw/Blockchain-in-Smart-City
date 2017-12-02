
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
        return QCA::Hash("sha1").hash(secByteArray).toByteArray();
    }
}

QByteArray sha256(QByteArray byteArray)
{
	if(!QCA::isSupported("sha256"))
	{
		qDebug() << "SHA256 not supported!\n";
		return QByteArray();
	}
    else
    {
    	QCA::SecureArray secByteArray = byteArray;
        return QCA::Hash("sha256").hash(secByteArray).toByteArray();
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

	sock = new NetSocket();
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
  		sendRouteRumor(name, seqNo ,peerList[i]->IPAddress, peerList[i]->UDPPortNum, 0, 0);
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

  /*
  QString x= "viper.zoo.cs.yale.edu";
	QHostInfo info = QHostInfo::fromName(x);
	qDebug() << info.addresses()[0];
	sendMsg("testmsg","me",1,QHostAddress("128.36.232.39"),40108);*/
}

void ChatDialog::popUpPrivateChatWindow(QListWidgetItem *item)
{
	QString dest = item->text();
	privateChatWindow->exec();
	if(cIsClicked == true)
	{
		sendPrivateMsg(dest, name, privateMsg, 10, routeTable.value(dest).first, routeTable.value(dest).second);
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
			sendBlockRequest(reqDest, name, 10, fileLink->metadata.first, routeTable.value(reqDest).first, routeTable.value(reqDest).second);
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
			sendBlockRequest(fOwner, name, 10, fileList[i]->metadata.first, routeTable.value(fOwner).first, routeTable.value(fOwner).second);
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
void ChatDialog::sendMsg(QString text, QString origin, quint32 seq, const QHostAddress addr, quint16 port, quint32 lastIP = 0, quint16 lastPort = 0)
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
	sock->writeDatagram(dataArray,addr,port);
}

void ChatDialog::sendRouteRumor(QString origin, quint32 seq, const QHostAddress addr, quint16 port, quint32 lastIP = 0, quint16 lastPort = 0)
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
	sock->writeDatagram(dataArray,addr,port);
}

void ChatDialog::sendPrivateMsg(QString dest, QString origin, QString text, quint32 ttl, const QHostAddress addr,quint16 port)
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
	sock->writeDatagram(dataArray,addr,port);
}

void ChatDialog::sendBlockRequest(QString dest, QString origin, quint32 ttl, QByteArray sha1Hash, const QHostAddress addr,quint16 port)
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
	sock->writeDatagram(dataArray,addr,port);
}
void ChatDialog::sendBlockReply(QString dest, QString origin, quint32 ttl, QByteArray sha1Hash, QByteArray data, const QHostAddress addr,quint16 port)
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

void ChatDialog::sendStatusMsg(const QHostAddress addr, quint16 port, QString origin = "", quint16 seq = 0)
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

	qDebug() << "sent want msg:" << dataMap;

	sock->writeDatagram(dataArray,addr,port);
}

void ChatDialog::sendInquiry()
{
	Peer* p =getRandNeighbor();
  	sendStatusMsg(p->IPAddress, p->UDPPortNum);
  	qDebug() << "sent inquiry to: "<< p->IPAddress << p->UDPPortNum;
  	//updateInquiryInx();
}

void ChatDialog::sendPeriodicRouteRumor()
{
	Peer* p = getRandNeighbor();
	sendRouteRumor(name, seqNo ,p->IPAddress, p->UDPPortNum);
	msgList[0]->msg.append(QString());
	seqNo++;

	QTimer* msgTimer = new QTimer();
  	msgTimer->setSingleShot(true);
  	msgTimerList.append(new QTimer());
  	msgTimer->start(2000);
	qDebug() << "sent rumor to: "<< p->IPAddress << p->UDPPortNum;
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
			sendBlockRequest(reqDest, name, 10, fileLink->metadata.first, routeTable.value(reqDest).first, routeTable.value(reqDest).second);
		}
		else if(fileLink->curBlockSize < fileLink->blockSize)
		{
			for(auto k: fileLink->dataMap.keys())
			{
				if(fileLink->dataMap.value(k).isNull())
				{
					sendBlockRequest(reqDest, name, 10, k, routeTable.value(reqDest).first, routeTable.value(reqDest).second);
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
  sendMsg(textinput->toPlainText(), name ,seqNo, p->IPAddress, p->UDPPortNum);

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
	qDebug() << "new routeTable: " << routeTable;
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
			qDebug() << "name: "<< fileLink->fileName;
			continue;
		}

		if(fileLink->metadata.first == hashData && fileLink->metadata.second.isNull())
		{
			qDebug() << "here1";
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
			qDebug() << "here2";
			fileLink->dataMap.insert(hashData, data);
			fileLink->curBlockSize++;
		}
		if(fileLink->curBlockSize < fileLink->blockSize)
		{
			qDebug() << "here3";
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
			qDebug() << "here4";
			writeToFile(fileLink);
			fileLink->isDownloadable = false;
		}
		else
		{
			qDebug() << "here5";
			return QByteArray();
		}
	}
	return QByteArray();
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
	if(dataMap.contains("Want"))
	{
        QVariantMap qMap = dataMap.value("Want").value<QVariantMap>();
        for(quint16 i =0; i < qMap.keys().size() ; i++)
        {
	        QString origin = qMap.keys()[i];
	        quint16 seq = qMap.value(origin).toInt();

		    int diff = seq - getCurMsgSize(origin);
		    qDebug() << "got want msg for " << dataMap.value("Want") << " from "<< sender << " "<<senderPort;
		    qDebug() << "seq curMsgSize diff="<<seq<<getCurMsgSize(origin)<< diff;
			if(diff > 1)
			{
				qDebug() <<"origin: "<< origin << getCurMsgSize(origin) + 1;
				sendStatusMsg(sender, senderPort, origin, getCurMsgSize(origin) + 1);
			}
			else if(diff < 1)
			{
				qDebug() << "msg: "<<msgList[getMsgInx(origin)]->msg[seq-1]<<" origin "<<origin<<" seq "<<seq<<" sender " <<sender <<" port "<<senderPort;
				QString msgString = msgList[getMsgInx(origin)]->msg[seq-1];
				quint32 lastIP = msgList[getMsgInx(origin)]->lastIP;
				quint16 lastPort = msgList[getMsgInx(origin)]->lastPort;
				if(msgString.isNull())
				{
					sendRouteRumor(origin, seq, sender, senderPort, lastIP, lastPort);
				}
				else if(!noForward)
				{
					sendMsg(msgString, origin, seq, sender, senderPort, lastIP, lastPort);
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
		    			sendRouteRumor(origin, seq-1, p->IPAddress, p->UDPPortNum, lastIP, lastPort);
		    		}
		    		else if(!noForward)
		    		{
		    			sendMsg(msgString, origin, seq-1, p->IPAddress, p->UDPPortNum, lastIP, lastPort);
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
					sendBlockReply(origin, name, 10, sha1(data), data, sender , senderPort);
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
						sendBlockRequest(origin, name, 10, reqByteArray, sender, senderPort);
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
					//sendBlockRequest(origin, name, 10, hashBlockList, routeTable.value(origin).first, routeTable.value(origin).second);
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
				sendPrivateMsg(dest, origin, dataMap.value("ChatText").toString(), ttl - 1, addr, port);
			}
			else if(crq)
			{
				sendBlockRequest(dest, origin, ttl - 1, dataMap.value("BlockRequest").value<QByteArray>(), addr, port);
			}
			else if(crp)
			{
				sendBlockReply(dest, origin, ttl-1, dataMap.value("BlockReply").value<QByteArray>(), dataMap.value("Data").value<QByteArray>(), addr, port);
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
	    qDebug() << "got message from" << sender << " " <<senderPort;
	  	//qDebug() << "text: "<<text<<" origin: "<< origin << " seqNo: "<<seq;
	    if(cc && ud>0)
	    {
	      textview->append("New MSG from " + origin + ":");
	      textview->append(dataMap.value("ChatText").toString());
	    }
	    sendStatusMsg(sender,senderPort, origin, getCurMsgSize(origin) + 1);
	    qDebug() << "received: " << (dataMap.contains("ChatText")? dataMap.value("ChatText").toString(): "route rumor") << " from: "<<origin<< seq;
	    //qDebug() << "cc: " << QString::number(cc) << " cl: "<< QString::number(cl);

	    if(cc && !noForward)
  		{
  			Peer* p =getRandNeighbor(sender, senderPort);
  			if(p != 0)
  			{
  				sendMsg(dataMap.value("ChatText").toString(), origin ,seq, p->IPAddress, p->UDPPortNum, sender.toIPv4Address(), senderPort);
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
  					sendRouteRumor(origin, seq, addr, port, sender.toIPv4Address(), senderPort);
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
    		//qDebug()<<"forward: bud="<<budget-1;
    		sendSearchRequestToNeighbors(origin, searchString, budget - 1, sender, senderPort);
    	}
    }
}

NetSocket::NetSocket()
{
	// Pick a range of four UDP ports to try to allocate by default,
	// computed based on my Unix user ID.
	// This makes it trivial for up to four Peerster instances per user
	// to find each other on the same host,
	// barring UDP port conflicts with other applications
	// (which are quite possible).
	// We use the range from 32768 to 49151 for this purpose.
        
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
	return app.exec();
}

