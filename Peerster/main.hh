#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
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

#include <RSA.hh>
#include "NetSocket.hh"



class NewBlockThread : public QThread
{
    Q_OBJECT

public:
    NewBlockThread(NetSocket* sock, QVector<Block*> *blockchain, QString newName, QVariantMap *data);

    void setContFlag(bool flag);
    void setQuitThreadFlag(bool flag);

protected:
    Block* createNewBlock(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap *data);
    void run();

private:
    bool contCompFlag;
    bool quitThreadFlag;
    NetSocket *sock;
    QVector<Block*> *blockchain;
    QString newName;
    QVariantMap *data;
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
  // void initializeRSA();
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
  void genTrans();

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  // void sendMsg(QString, QString, quint32,const QHostAddress,quint16, quint32, quint16);
  // void sendRouteRumor(QString, quint32, const QHostAddress,quint16, quint32, quint16);
  void sendStatusMsg(const QHostAddress addr, quint16 port, QString origin = "", quint16 seq = 0);
  // void sendBlockRequest(QString, QString, quint32, QByteArray, const QHostAddress, quint16);
  // void sendBlockReply(QString, QString, quint32, QByteArray, QByteArray, const QHostAddress,quint16);

  // void sendPrivateMsg(QString, QString, QString, quint32, const QHostAddress,quint16);

  void sendSearchRequest(QString, QString, quint32, const QHostAddress, quint16);
  void sendSearchRequestToNeighbors(QString, QString, quint32, QHostAddress, quint16);
  void sendSearchReply(QString, QString, quint32, QString, QVariantList, QVariantList, const QHostAddress,quint16);

  // void sendNewBLOCK(Block block, const QHostAddress addr, quint16 port);
  // void sendWantBLOCKMsg(const QHostAddress addr, quint16 port, quint16 seq);
  // void sendNewBLOCKToNeighbors(Block, QHostAddress, quint16);

  Block* getGenesisBlock();
  Block* createNewBlock(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap data);

  bool canAppendToChain(QString name, quint32 index, QTime time, QByteArray prev_hash, QVariantMap data, QByteArray hash, quint64 rand);
  bool canAppendToChain(Block block);

  int updateMsgRcvd(QVariantMap,QHostAddress,quint16);
  quint16 getCurMsgSize(QString);
  // void updateInquiryInx();
  Peer* getRandNeighbor(QHostAddress, quint16);
  int getMsgInx(QString);
  
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
  QTimer *dataTimer;

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
  QVariantMap transacData;

  NewBlockThread *nBT;

  bool blockInitialized;

  RSA *rsa;

  bool verifyDataInt(quint32 key, quint32 n, QVariantMap data);
};

#endif // PEERSTER_MAIN_HH
