#ifndef PEERSTER_RSA_HH
#define PEERSTER_RSA_HH

/*
 * C++ Program to Implement the RSA Algorithm
 */

#include <cmath>
#include <time.h>
#include <QList>
#include <QtCrypto>
#include <QString>
#include <QDebug>

class RSA
{
public:
    RSA();
    int prime(long int);
    void ce();
    long int cd(long int);
    int getRandPrime();
    QVariantList encrypt(QString msg, long int pubkey = 0);
    QString decrypt(QVariantList en, long int seckey = 0, long int n =0);

    QString sha1(QString msg);

    QVariantList signMessage(QString msg);//QPari<msg, enc(hash)>
    bool verifySignature(QString, QVariantList, long int pubkey, long int n);

    long int getPubKey();
    long int getN();

private:
    long int p, q, n, t, e, d;
};


#endif
