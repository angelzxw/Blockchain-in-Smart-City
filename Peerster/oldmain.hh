#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QUdpSocket>
#include <QTimer>
#include <QTime>
#include <QHostInfo>
#include <QHash>
#include <QPair>
#include <QListWidget>
#include <QFileDialog>
#include <QFile>
#include <QtCrypto>
#include <QStringList>

class NewBlockThread : public QThread
{
    Q_OBJECT
public:
    NewBlockThread()
    {
      i = 0;
    }
    void setI(quint64 i)
    {
      this->i = i;
    }

protected:
    void run()
    {
      while(true)
      {
        qDebug() << i;
        msleep(1000);
      }
    }

private:
  quint64 i;
};

class Block
{
public:
    Block(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap data, QByteArray hash, quint64 rand)
    {
        this->name = name;
        this->index = index;
        this->time = time;
        this->prev_hash = prev_hash;
        this->data = data;
        this->hash = hash;
        this->rand = rand;
    }

    QString name;
    quint32 index;
    QTime time;
    QByteArray prev_hash;
    QVariantMap data;
    QByteArray hash;
    quint64 rand;
};

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

class MsgRcvd
{
public:
  MsgRcvd(QString name)
  {
    this->name = name;
    lastIP = 0;
    lastPort = 0;
  }
  QString name;
  QVector<QString> msg;
  quint32 lastIP;
  quint16 lastPort;
};

class File
{
public:
	File(QString owner, QString name)
	{
		fileOwner = owner;
		fileName = name;
		isDownloadable = true;
		blockSize = 0;
		curBlockSize = 0;
	}
	bool isDownloadable;
	QString fileName;
	QString fileOwner;
	quint32 blockSize;
	quint32 curBlockSize;
	QPair<QByteArray,QByteArray> metadata;//hashmeta->blocklistmeta
	QMap<QByteArray,QByteArray> dataMap;//blockhash->data
};

class SearchResult
{
public:
	SearchResult()
	{
		curFileNum = 0;
		curBud = 2;
	}
	quint16 updateBud()
	{
		curBud *= 2;
		return curBud;
	}
	void updateFile(quint16 incFile)
	{	
		curFileNum += incFile;
	}
	void reset()
	{
		curFileNum = 0;
		curBud = 2;
		name = QString();
	}
	bool checkStatus()
	{
		return (curFileNum >= 10) || (curBud >= 100);
	}
	QString name;
	quint16 curFileNum;
	quint16 curBud;
};

class NetSocket : public QUdpSocket
{
	Q_OBJECT
        
public:
  NetSocket();

	// Bind this socket to a Peerster-specific default port.
	int bind();
  quint16 getPortNum();
  QString getPortString();

private:
	int myPortMin, myPortMax;
  quint16 portNum;
};

class PrivateChatWindow : public QDialog
{
  Q_OBJECT

public:
  PrivateChatWindow(QString*, bool*);
public slots:
  void quit();
private:
  QTextEdit *textinput;
  QPushButton *btnSend;

  QString dest;

  QString *privateMsg;
  bool* isClicked;
};

class PrivateDownloadWindow : public QDialog
{
	Q_OBJECT

public:
	PrivateDownloadWindow(QString*, QString*, bool*);
public slots:
	void quit();
private:
	QLineEdit* sha1input;
	QLineEdit* nameinput;
	bool* isClicked;
	QString* downloadSHA1;
	QString* downloadFileName;
	QPushButton* btnSubmit;
};

class PrivateSearchWindow : public QDialog
{
	Q_OBJECT

public:
	PrivateSearchWindow(QString*, bool*);
public slots:
	void quit();
private:
	QLineEdit* stringinput;
	QString* searchString;
	bool* isClicked;
	QPushButton* btnSearch;
};

class ChatDialog : public QDialog
{
  Q_OBJECT

public:
  ChatDialog();
  void addPeerFromCMD(QString);
  void setDest(QString);
  void setText(QString);
  void setNoForward(bool);

public slots:
  void gotSubmitPressed();
  void readPendingDatagrams();
  void sendInquiry();
  void gotAddPeerPressed();
  void lookedUp(const QHostInfo&);
  void sendPeriodicRouteRumor();
  void sendPeriodicFileInquiry();
  void popUpPrivateChatWindow(QListWidgetItem *);
  void popUpFileSelection();
  void popUpFileDownload();
  void popUpPrivateSearchWindow();
  void startDownloadingFile(QListWidgetItem *);
  void updateSearchResult();

private:
  void sendMsg(QString, QString, quint32,const QHostAddress,quint16, quint32, quint16);
  void sendRouteRumor(QString, quint32, const QHostAddress,quint16, quint32, quint16);
  void sendStatusMsg(const QHostAddress,quint16,QString,quint16);
  void sendBlockRequest(QString, QString, quint32, QByteArray, const QHostAddress, quint16);
  void sendBlockReply(QString, QString, quint32, QByteArray, QByteArray, const QHostAddress,quint16);
  void sendSearchRequest(QString, QString, quint32, const QHostAddress, quint16);
  void sendSearchRequestToNeighbors(QString, QString, quint32, QHostAddress, quint16);
  void sendSearchReply(QString, QString, quint32, QString, QVariantList, QVariantList, const QHostAddress,quint16);

  void sendNewBLOCK(Block block, const QHostAddress addr, quint16 port);
  void sendWantBLOCKMsg(const QHostAddress addr, quint16 port, quint16 seq);
  void sendNewBLOCKToNeighbors(Block, QHostAddress, quint16);

  Block* getGenesisBlock();
  Block* createNewBlock(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap data);

  int updateMsgRcvd(QVariantMap,QHostAddress,quint16);
  quint16 getCurMsgSize(QString);
  void updateInquiryInx();
  Peer* getRandNeighbor(QHostAddress, quint16);
  int getMsgInx(QString);
  void sendPrivateMsg(QString, QString, QString, quint32, const QHostAddress,quint16);
  void updateRouteTable(QString, const QHostAddress, quint16);
  QByteArray searchData(QByteArray);
  QByteArray saveData(QByteArray, QByteArray, QString);
  void writeToFile(File*);
  PrivateChatWindow *privateChatWindow;
  PrivateDownloadWindow *privateDownloadWindow;
  PrivateSearchWindow *privateSearchWindow;

  QListWidget *listWidget;
  QListWidget *fileWidget;

  QString name;

  QTimer *timer;
  QTimer *inquiryTimer;
  QTimer *routeRumorTimer;
  QTimer *fileInquiryTimer;
  QVector<QTimer*> msgTimerList;
  QTimer *searchTimer;

  quint32 seqNo;
  quint16 inquiryInx;
  QTextEdit *textview;
  QTextEdit *textinput;
  //QTextEdit *privateTextInput;
  QPushButton *btnSend;
  QPushButton *btnShareFile;
  QPushButton *btnDownload;
  QPushButton *btnSearch;
  QPushButton *btnAddPeer;
  //QPushButton *btnSendPrivateMsg;
  NetSocket *sock;
  QVector<QString> portNames;
  QVector<QString> msgRcvd[256];

  QVector<Peer*> peerList;
  QVector<MsgRcvd*> msgList;
  QVector<File*> fileList;

  quint16 newPort;

  QHash<QString, QPair<QHostAddress,quint16>> routeTable;

  QString privateMsg;

  bool cIsClicked;
  bool dIsClicked;
  bool sIsClicked;

  QString downloadSHA1;
  QString downloadFileName;

  QString searchString;

  SearchResult* searchResult;

  bool noForward;

  QVector<Block*> blockchain;
};

#endif // PEERSTER_MAIN_HH
