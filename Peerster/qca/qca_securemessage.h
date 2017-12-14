/*
 * qca_securemessage.h - Qt Cryptographic Architecture
 * Copyright (C) 2003-2007  Justin Karneges <justin@affinix.com>
 * Copyright (C) 2004,2005  Brad Hards <bradh@frogmouth.net>
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

#ifndef QCA_SECUREMESSAGE_H
#define QCA_SECUREMESSAGE_H

#include <QObject>
#include "qca_core.h"
#include "qca_publickey.h"
#include "qca_cert.h"

class QDateTime;

namespace QCA {

class SecureMessageSystem;

class QCA_EXPORT SecureMessageKey
{
public:
        enum Type
        {
                None, 
                PGP,  
                X509  
        };

        SecureMessageKey();

        SecureMessageKey(const SecureMessageKey &from);

        ~SecureMessageKey();

        SecureMessageKey & operator=(const SecureMessageKey &from);

        bool isNull() const;

        Type type() const;

        PGPKey pgpPublicKey() const;

        PGPKey pgpSecretKey() const;

        void setPGPPublicKey(const PGPKey &pub);

        void setPGPSecretKey(const PGPKey &sec);

        CertificateChain x509CertificateChain() const;

        PrivateKey x509PrivateKey() const;

        void setX509CertificateChain(const CertificateChain &c);

        void setX509PrivateKey(const PrivateKey &k);

        void setX509KeyBundle(const KeyBundle &kb);

        bool havePrivate() const;

        QString name() const;

private:
        class Private;
        QSharedDataPointer<Private> d;
};

typedef QList<SecureMessageKey> SecureMessageKeyList;

class QCA_EXPORT SecureMessageSignature
{
public:
        enum IdentityResult
        {
                Valid,            
                InvalidSignature, 
                InvalidKey,       
                NoKey             
        };

        SecureMessageSignature();

        SecureMessageSignature(IdentityResult r, Validity v, const SecureMessageKey &key, const QDateTime &ts);

        SecureMessageSignature(const SecureMessageSignature &from);

        ~SecureMessageSignature();

        SecureMessageSignature & operator=(const SecureMessageSignature &from);

        IdentityResult identityResult() const;

        Validity keyValidity() const;

        SecureMessageKey key() const;

        QDateTime timestamp() const;

private:
        class Private;
        QSharedDataPointer<Private> d;
};

typedef QList<SecureMessageSignature> SecureMessageSignatureList;


class QCA_EXPORT SecureMessage : public QObject, public Algorithm
{
        Q_OBJECT
public:
        enum Type
        {
                OpenPGP, 
                CMS      
        };

        enum SignMode
        {
                Message,    
                Clearsign,  
                Detached    
        };

        enum Format
        {
                Binary, 
                Ascii   
        };

        enum Error
        {
                ErrorPassphrase,       
                ErrorFormat,           
                ErrorSignerExpired,    
                ErrorSignerInvalid,    
                ErrorEncryptExpired,   
                ErrorEncryptUntrusted, 
                ErrorEncryptInvalid,   
                ErrorNeedCard,         
                ErrorCertKeyMismatch,  
                ErrorUnknown           
        };

        SecureMessage(SecureMessageSystem *system);
        ~SecureMessage();

        Type type() const;

        bool canSignMultiple() const;

        bool canClearsign() const;

        bool canSignAndEncrypt() const;

        void reset();

        bool bundleSignerEnabled() const;

        bool smimeAttributesEnabled() const;

        Format format() const;

        SecureMessageKeyList recipientKeys() const;

        SecureMessageKeyList signerKeys() const;

        void setBundleSignerEnabled(bool b);

        void setSMIMEAttributesEnabled(bool b);

        void setFormat(Format f);

        void setRecipient(const SecureMessageKey &key);

        void setRecipients(const SecureMessageKeyList &keys);

        void setSigner(const SecureMessageKey &key);

        void setSigners(const SecureMessageKeyList &keys);

        void startEncrypt();

        void startDecrypt();

        void startSign(SignMode m = Message);

        void startVerify(const QByteArray &detachedSig = QByteArray());

        void startSignAndEncrypt();

        void update(const QByteArray &in);

        QByteArray read();

        int bytesAvailable() const;

        void end();

        bool waitForFinished(int msecs = 30000);

        bool success() const;

        Error errorCode() const;

        QByteArray signature() const;

        QString hashName() const;

        bool wasSigned() const;

        bool verifySuccess() const;

        SecureMessageSignature signer() const;

        SecureMessageSignatureList signers() const;

        QString diagnosticText() const;

Q_SIGNALS:
        void readyRead();

        void bytesWritten(int bytes);

        void finished();

private:
        Q_DISABLE_COPY(SecureMessage)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT SecureMessageSystem : public QObject, public Algorithm
{
        Q_OBJECT
public:
        ~SecureMessageSystem();

protected:
        SecureMessageSystem(QObject *parent, const QString &type, const QString &provider);

private:
        Q_DISABLE_COPY(SecureMessageSystem)
};

class QCA_EXPORT OpenPGP : public SecureMessageSystem
{
        Q_OBJECT
public:
        explicit OpenPGP(QObject *parent = 0, const QString &provider = QString());
        ~OpenPGP();

private:
        Q_DISABLE_COPY(OpenPGP)

        class Private;
        Private *d;
};

class QCA_EXPORT CMS : public SecureMessageSystem
{
        Q_OBJECT
public:
        explicit CMS(QObject *parent = 0, const QString &provider = QString());
        ~CMS();

        CertificateCollection trustedCertificates() const;

        CertificateCollection untrustedCertificates() const;

        SecureMessageKeyList privateKeys() const;

        void setTrustedCertificates(const CertificateCollection &trusted);

        void setUntrustedCertificates(const CertificateCollection &untrusted);

        void setPrivateKeys(const SecureMessageKeyList &keys);

private:
        Q_DISABLE_COPY(CMS)

        class Private;
        Private *d;
};

}

#endif