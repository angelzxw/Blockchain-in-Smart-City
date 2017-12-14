/*
 * qca_securelayer.h - Qt Cryptographic Architecture
 * Copyright (C) 2003-2007  Justin Karneges <justin@affinix.com>
 * Copyright (C) 2004-2006  Brad Hards <bradh@frogmouth.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 *
 */

#ifndef QCA_SECURELAYER_H
#define QCA_SECURELAYER_H

#include <QObject>
#include "qca_core.h"
#include "qca_publickey.h"
#include "qca_cert.h"

namespace QCA {

enum SecurityLevel
{
        SL_None,      
        SL_Integrity, 
        SL_Export,    
        SL_Baseline,  
        SL_High,      
        SL_Highest    
};

class QCA_EXPORT SecureLayer : public QObject
{
        Q_OBJECT
public:
        SecureLayer(QObject *parent = 0);

        virtual bool isClosable() const;

        virtual int bytesAvailable() const = 0;

        virtual int bytesOutgoingAvailable() const = 0;

        virtual void close();

        virtual void write(const QByteArray &a) = 0;

        virtual QByteArray read() = 0;

        virtual void writeIncoming(const QByteArray &a) = 0;

        virtual QByteArray readOutgoing(int *plainBytes = 0) = 0;

        virtual QByteArray readUnprocessed();

        virtual int convertBytesWritten(qint64 encryptedBytes) = 0;

Q_SIGNALS:
        void readyRead();

        void readyReadOutgoing();

        void closed();

        void error();

private:
        Q_DISABLE_COPY(SecureLayer)
};

class QCA_EXPORT TLSSession : public Algorithm
{
public:
        TLSSession();

        TLSSession(const TLSSession &from);

        ~TLSSession();

        TLSSession & operator=(const TLSSession &from);

        bool isNull() const;
};

class QCA_EXPORT TLS : public SecureLayer, public Algorithm
{
        Q_OBJECT
public:
        enum Mode
        {
                Stream,  
                Datagram 
        };

        enum Version
        {
                TLS_v1, 
                SSL_v3, 
                SSL_v2, 
                DTLS_v1 
        };

        enum Error
        {
                ErrorSignerExpired,   
                ErrorSignerInvalid,   
                ErrorCertKeyMismatch, 
                ErrorInit,            
                ErrorHandshake,       
                ErrorCrypt            
        };

        enum IdentityResult
        {
                Valid,              
                HostMismatch,       
                InvalidCertificate, 
                NoCertificate       
        };

        explicit TLS(QObject *parent = 0, const QString &provider = QString());

        explicit TLS(Mode mode, QObject *parent = 0, const QString &provider = QString());

        ~TLS();

        void reset();

        QStringList supportedCipherSuites(const Version &version = TLS_v1) const;

        void setCertificate(const CertificateChain &cert, const PrivateKey &key);

        void setCertificate(const KeyBundle &kb);

        CertificateCollection trustedCertificates() const;

        void setTrustedCertificates(const CertificateCollection &trusted);

        void setConstraints(SecurityLevel s);

        void setConstraints(int minSSF, int maxSSF);

        void setConstraints(const QStringList &cipherSuiteList);

        QList<CertificateInfoOrdered> issuerList() const;

        void setIssuerList(const QList<CertificateInfoOrdered> &issuers);

        void setSession(const TLSSession &session);

        bool canCompress() const;

        bool canSetHostName() const;

        bool compressionEnabled() const;

        void setCompressionEnabled(bool b);

        QString hostName() const;

        void startClient(const QString &host = QString());

        void startServer();

        void continueAfterStep();

        bool isHandshaken() const;

        bool isCompressed() const;

        Version version() const;

        QString cipherSuite() const;

        int cipherBits() const;

        int cipherMaxBits() const;

        TLSSession session() const;

        Error errorCode() const;

        IdentityResult peerIdentityResult() const;

        Validity peerCertificateValidity() const;

        CertificateChain localCertificateChain() const;

        PrivateKey localPrivateKey() const;

        CertificateChain peerCertificateChain() const;

        // reimplemented
        virtual bool isClosable() const;
        virtual int bytesAvailable() const;
        virtual int bytesOutgoingAvailable() const;
        virtual void close();
        virtual void write(const QByteArray &a);
        virtual QByteArray read();
        virtual void writeIncoming(const QByteArray &a);
        virtual QByteArray readOutgoing(int *plainBytes = 0);
        virtual QByteArray readUnprocessed();
        virtual int convertBytesWritten(qint64 encryptedBytes);

        int packetsAvailable() const;

        int packetsOutgoingAvailable() const;

        int packetMTU() const;

        void setPacketMTU(int size) const;

Q_SIGNALS:
        void hostNameReceived();

        void certificateRequested();

        void peerCertificateAvailable();

        void handshaken();

protected:
        void connectNotify(const char *signal);

        void disconnectNotify(const char *signal);

private:
        Q_DISABLE_COPY(TLS)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT SASL : public SecureLayer, public Algorithm
{
        Q_OBJECT
public:
        enum Error
        {
                ErrorInit,      
                ErrorHandshake, 
                ErrorCrypt      
        };

        enum AuthCondition
        {
                AuthFail,          
                NoMechanism,       
                BadProtocol,       
                BadServer,         
                BadAuth,           
                NoAuthzid,         
                TooWeak,           
                NeedEncrypt,       
                Expired,           
                Disabled,          
                NoUser,            
                RemoteUnavailable  
        };

        enum AuthFlags
        {
                AuthFlagsNone          = 0x00,
                AllowPlain             = 0x01,
                AllowAnonymous         = 0x02,
                RequireForwardSecrecy  = 0x04,
                RequirePassCredentials = 0x08,
                RequireMutualAuth      = 0x10,
                RequireAuthzidSupport  = 0x20  // server-only
        };

        enum ClientSendMode
        {
                AllowClientSendFirst,
                DisableClientSendFirst
        };

        enum ServerSendMode
        {
                AllowServerSendLast,
                DisableServerSendLast
        };

        class QCA_EXPORT Params
        {
        public:
                Params();

                Params(bool user, bool authzid, bool pass, bool realm);

                Params(const Params &from);
                ~Params();

                Params & operator=(const Params &from);

                bool needUsername() const;

                bool canSendAuthzid() const;

                bool needPassword() const;

                bool canSendRealm() const;

        private:
                class Private;
                Private *d;
        };

        explicit SASL(QObject *parent = 0, const QString &provider = QString());

        ~SASL();

        void reset();

        void setConstraints(AuthFlags f, SecurityLevel s = SL_None);

        void setConstraints(AuthFlags f, int minSSF, int maxSSF);

        void setLocalAddress(const QString &addr, quint16 port);

        void setRemoteAddress(const QString &addr, quint16 port);

        void setExternalAuthId(const QString &authid);

        void setExternalSSF(int strength);

        void startClient(const QString &service, const QString &host, const QStringList &mechlist, ClientSendMode mode = AllowClientSendFirst);

        void startServer(const QString &service, const QString &host, const QString &realm, ServerSendMode mode = DisableServerSendLast);

        void putServerFirstStep(const QString &mech);

        void putServerFirstStep(const QString &mech, const QByteArray &clientInit);

        void putStep(const QByteArray &stepData);

        QString mechanism() const;

        QStringList mechanismList() const;

        QStringList realmList() const;

        int ssf() const;

        Error errorCode() const;

        AuthCondition authCondition() const;

        void setUsername(const QString &user);

        void setAuthzid(const QString &auth);

        void setPassword(const SecureArray &pass);

        void setRealm(const QString &realm);

        void continueAfterParams();

        void continueAfterAuthCheck();

        // reimplemented
        virtual int bytesAvailable() const;
        virtual int bytesOutgoingAvailable() const;
        virtual void write(const QByteArray &a);
        virtual QByteArray read();
        virtual void writeIncoming(const QByteArray &a);
        virtual QByteArray readOutgoing(int *plainBytes = 0);
        virtual int convertBytesWritten(qint64 encryptedBytes);

Q_SIGNALS:
        void clientStarted(bool clientInit, const QByteArray &clientInitData);

        void serverStarted();

        void nextStep(const QByteArray &stepData);

        void needParams(const QCA::SASL::Params &params);

        void authCheck(const QString &user, const QString &authzid);

        void authenticated();

private:
        Q_DISABLE_COPY(SASL)

        class Private;
        friend class Private;
        Private *d;
};

}

#endif