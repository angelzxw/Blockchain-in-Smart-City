/*
 * qcaprovider.h - QCA Plugin API
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

#ifndef QCAPROVIDER_H
#define QCAPROVIDER_H

#include "qca_core.h"
#include "qca_basic.h"
#include "qca_publickey.h"
#include "qca_cert.h"
#include "qca_keystore.h"
#include "qca_securelayer.h"
#include "qca_securemessage.h"

#include <limits>

#ifndef DOXYGEN_NO_PROVIDER_API

class QCA_EXPORT QCAPlugin
{
public:
        virtual ~QCAPlugin() {}

        virtual QCA::Provider *createProvider() = 0;
};

Q_DECLARE_INTERFACE(QCAPlugin, "com.affinix.qca.Plugin/1.0")

namespace QCA {

class QCA_EXPORT InfoContext : public BasicContext
{
        Q_OBJECT
public:
        InfoContext(Provider *p) : BasicContext(p, "info") {}

        virtual QStringList supportedHashTypes() const;

        virtual QStringList supportedCipherTypes() const;

        virtual QStringList supportedMACTypes() const;
};

class QCA_EXPORT RandomContext : public BasicContext
{
        Q_OBJECT
public:
        RandomContext(Provider *p) : BasicContext(p, "random") {}

        virtual SecureArray nextBytes(int size) = 0;
};

class QCA_EXPORT HashContext : public BasicContext
{
        Q_OBJECT
public:
        HashContext(Provider *p, const QString &type) : BasicContext(p, type) {}

        virtual void clear() = 0;

        virtual void update(const MemoryRegion &a) = 0;

        virtual MemoryRegion final() = 0;
};

class QCA_EXPORT CipherContext : public BasicContext
{
        Q_OBJECT
public:
        CipherContext(Provider *p, const QString &type) : BasicContext(p, type) {}

        virtual void setup(Direction dir, const SymmetricKey &key, const InitializationVector &iv) = 0;

        virtual KeyLength keyLength() const = 0;

        virtual int blockSize() const = 0;

        virtual bool update(const SecureArray &in, SecureArray *out) = 0;

        virtual bool final(SecureArray *out) = 0;
};

class QCA_EXPORT MACContext : public BasicContext
{
        Q_OBJECT
public:
        MACContext(Provider *p, const QString &type) : BasicContext(p, type) {}

        virtual void setup(const SymmetricKey &key) = 0;

        virtual KeyLength keyLength() const = 0;

        virtual void update(const MemoryRegion &in) = 0;

        virtual void final(MemoryRegion *out) = 0;

protected:
        KeyLength anyKeyLength() const
        {
                // this is used instead of a default implementation to make sure that
                // provider authors think about it, at least a bit.
                // See Meyers, Effective C++, Effective C++ (2nd Ed), Item 36
                return KeyLength( 0, INT_MAX, 1 );
        }
};

class QCA_EXPORT KDFContext : public BasicContext
{
        Q_OBJECT
public:
        KDFContext(Provider *p, const QString &type) : BasicContext(p, type) {}

        virtual SymmetricKey makeKey(const SecureArray &secret, const InitializationVector &salt, unsigned int keyLength, unsigned int iterationCount) = 0;
};

class QCA_EXPORT DLGroupContext : public Provider::Context
{
        Q_OBJECT
public:
        DLGroupContext(Provider *p) : Provider::Context(p, "dlgroup") {}

        virtual QList<DLGroupSet> supportedGroupSets() const = 0;

        virtual bool isNull() const = 0;

        virtual void fetchGroup(DLGroupSet set, bool block) = 0;

        virtual void getResult(BigInteger *p, BigInteger *q, BigInteger *g) const = 0;

Q_SIGNALS:
        void finished();
};

class QCA_EXPORT PKeyBase : public BasicContext
{
        Q_OBJECT
public:
        PKeyBase(Provider *p, const QString &type);

        virtual bool isNull() const = 0;

        virtual PKey::Type type() const = 0;

        virtual bool isPrivate() const = 0;

        virtual bool canExport() const = 0;

        virtual void convertToPublic() = 0;

        virtual int bits() const = 0;

        virtual int maximumEncryptSize(EncryptionAlgorithm alg) const;

        virtual SecureArray encrypt(const SecureArray &in, EncryptionAlgorithm alg);

        virtual bool decrypt(const SecureArray &in, SecureArray *out, EncryptionAlgorithm alg);

        virtual void startSign(SignatureAlgorithm alg, SignatureFormat format);

        virtual void startVerify(SignatureAlgorithm alg, SignatureFormat format);

        virtual void update(const MemoryRegion &in);

        virtual QByteArray endSign();

        virtual bool endVerify(const QByteArray &sig);

        virtual SymmetricKey deriveKey(const PKeyBase &theirs);

Q_SIGNALS:
        void finished();
};

class QCA_EXPORT RSAContext : public PKeyBase
{
        Q_OBJECT
public:
        RSAContext(Provider *p) : PKeyBase(p, "rsa") {}

        virtual void createPrivate(int bits, int exp, bool block) = 0;

        virtual void createPrivate(const BigInteger &n, const BigInteger &e, const BigInteger &p, const BigInteger &q, const BigInteger &d) = 0;

        virtual void createPublic(const BigInteger &n, const BigInteger &e) = 0;

        virtual BigInteger n() const = 0;

        virtual BigInteger e() const = 0;

        virtual BigInteger p() const = 0;

        virtual BigInteger q() const = 0;

        virtual BigInteger d() const = 0;
};

class QCA_EXPORT DSAContext : public PKeyBase
{
        Q_OBJECT
public:
        DSAContext(Provider *p) : PKeyBase(p, "dsa") {}

        virtual void createPrivate(const DLGroup &domain, bool block) = 0;

        virtual void createPrivate(const DLGroup &domain, const BigInteger &y, const BigInteger &x) = 0;

        virtual void createPublic(const DLGroup &domain, const BigInteger &y) = 0;

        virtual DLGroup domain() const = 0;

        virtual BigInteger y() const = 0;

        virtual BigInteger x() const = 0;
};

class QCA_EXPORT DHContext : public PKeyBase
{
        Q_OBJECT
public:
        DHContext(Provider *p) : PKeyBase(p, "dh") {}

        virtual void createPrivate(const DLGroup &domain, bool block) = 0;

        virtual void createPrivate(const DLGroup &domain, const BigInteger &y, const BigInteger &x) = 0;

        virtual void createPublic(const DLGroup &domain, const BigInteger &y) = 0;

        virtual DLGroup domain() const = 0;

        virtual BigInteger y() const = 0;

        virtual BigInteger x() const = 0;
};

class QCA_EXPORT PKeyContext : public BasicContext
{
        Q_OBJECT
public:
        PKeyContext(Provider *p) : BasicContext(p, "pkey") {}

        virtual QList<PKey::Type> supportedTypes() const = 0;

        virtual QList<PKey::Type> supportedIOTypes() const = 0;

        virtual QList<PBEAlgorithm> supportedPBEAlgorithms() const = 0;

        virtual PKeyBase *key() = 0;

        virtual const PKeyBase *key() const = 0;

        virtual void setKey(PKeyBase *key) = 0;

        virtual bool importKey(const PKeyBase *key) = 0;

        virtual QByteArray publicToDER() const;

        virtual QString publicToPEM() const;

        virtual ConvertResult publicFromDER(const QByteArray &a);

        virtual ConvertResult publicFromPEM(const QString &s);

        virtual SecureArray privateToDER(const SecureArray &passphrase, PBEAlgorithm pbe) const;

        virtual QString privateToPEM(const SecureArray &passphrase, PBEAlgorithm pbe) const;

        virtual ConvertResult privateFromDER(const SecureArray &a, const SecureArray &passphrase);

        virtual ConvertResult privateFromPEM(const QString &s, const SecureArray &passphrase);
};

class QCA_EXPORT CertBase : public BasicContext
{
        Q_OBJECT
public:
        CertBase(Provider *p, const QString &type) : BasicContext(p, type) {}

        virtual QByteArray toDER() const = 0;

        virtual QString toPEM() const = 0;

        virtual ConvertResult fromDER(const QByteArray &a) = 0;

        virtual ConvertResult fromPEM(const QString &s) = 0;
};

class QCA_EXPORT CertContextProps
{
public:
        int version;

        QDateTime start;

        QDateTime end;

        CertificateInfoOrdered subject;

        CertificateInfoOrdered issuer;

        Constraints constraints;

        QStringList policies;

        QStringList crlLocations;

        QStringList issuerLocations;

        QStringList ocspLocations;

        BigInteger serial;

        bool isCA;

        bool isSelfSigned;

        int pathLimit;

        QByteArray sig;

        SignatureAlgorithm sigalgo;

        QByteArray subjectId;

        QByteArray issuerId;

        QString challenge;

        CertificateRequestFormat format;
};

class QCA_EXPORT CRLContextProps
{
public:
        CertificateInfoOrdered issuer;

        int number;

        QDateTime thisUpdate;

        QDateTime nextUpdate;

        QList<CRLEntry> revoked;

        QByteArray sig;

        SignatureAlgorithm sigalgo;

        QByteArray issuerId;
};

class CRLContext;

class QCA_EXPORT CertContext : public CertBase
{
        Q_OBJECT
public:
        CertContext(Provider *p) : CertBase(p, "cert") {}

        virtual bool createSelfSigned(const CertificateOptions &opts, const PKeyContext &priv) = 0;

        virtual const CertContextProps *props() const = 0;

        virtual bool compare(const CertContext *other) const = 0;

        virtual PKeyContext *subjectPublicKey() const = 0;

        virtual bool isIssuerOf(const CertContext *other) const = 0;

        virtual Validity validate(const QList<CertContext*> &trusted, const QList<CertContext*> &untrusted, const QList<CRLContext*> &crls, UsageMode u, ValidateFlags vf) const = 0;

        virtual Validity validate_chain(const QList<CertContext*> &chain, const QList<CertContext*> &trusted, const QList<CRLContext*> &crls, UsageMode u, ValidateFlags vf) const = 0;
};

class QCA_EXPORT CSRContext : public CertBase
{
        Q_OBJECT
public:
        CSRContext(Provider *p) : CertBase(p, "csr") {}

        virtual bool canUseFormat(CertificateRequestFormat f) const = 0;

        virtual bool createRequest(const CertificateOptions &opts, const PKeyContext &priv) = 0;

        virtual const CertContextProps *props() const = 0;

        virtual bool compare(const CSRContext *other) const = 0;

        virtual PKeyContext *subjectPublicKey() const = 0;

        virtual QString toSPKAC() const = 0;

        virtual ConvertResult fromSPKAC(const QString &s) = 0;
};

class QCA_EXPORT CRLContext : public CertBase
{
        Q_OBJECT
public:
        CRLContext(Provider *p) : CertBase(p, "crl") {}

        virtual const CRLContextProps *props() const = 0;

        virtual bool compare(const CRLContext *other) const = 0;
};

class QCA_EXPORT CertCollectionContext : public BasicContext
{
        Q_OBJECT
public:
        CertCollectionContext(Provider *p) : BasicContext(p, "certcollection") {}

        virtual QByteArray toPKCS7(const QList<CertContext*> &certs, const QList<CRLContext*> &crls) const = 0;

        virtual ConvertResult fromPKCS7(const QByteArray &a, QList<CertContext*> *certs, QList<CRLContext*> *crls) const = 0;
};

class QCA_EXPORT CAContext : public BasicContext
{
        Q_OBJECT
public:
        CAContext(Provider *p) : BasicContext(p, "ca") {}

        virtual void setup(const CertContext &cert, const PKeyContext &priv) = 0;

        virtual CertContext *certificate() const = 0;

        virtual CertContext *signRequest(const CSRContext &req, const QDateTime &notValidAfter) const = 0;

        virtual CertContext *createCertificate(const PKeyContext &pub, const CertificateOptions &opts) const = 0;

        virtual CRLContext *createCRL(const QDateTime &nextUpdate) const = 0;

        virtual CRLContext *updateCRL(const CRLContext &crl, const QList<CRLEntry> &entries, const QDateTime &nextUpdate) const = 0;
};

class QCA_EXPORT PKCS12Context : public BasicContext
{
        Q_OBJECT
public:
        PKCS12Context(Provider *p) : BasicContext(p, "pkcs12") {}

        virtual QByteArray toPKCS12(const QString &name, const QList<const CertContext*> &chain, const PKeyContext &priv, const SecureArray &passphrase) const = 0;

        virtual ConvertResult fromPKCS12(const QByteArray &in, const SecureArray &passphrase, QString *name, QList<CertContext*> *chain, PKeyContext **priv) const = 0;
};

class QCA_EXPORT PGPKeyContextProps
{
public:
        QString keyId;

        QStringList userIds;

        bool isSecret;

        QDateTime creationDate;

        QDateTime expirationDate;

        QString fingerprint;

        bool inKeyring;

        bool isTrusted;
};

class QCA_EXPORT PGPKeyContext : public BasicContext
{
        Q_OBJECT
public:
        PGPKeyContext(Provider *p) : BasicContext(p, "pgpkey") {}

        virtual const PGPKeyContextProps *props() const = 0;

        virtual QByteArray toBinary() const = 0;

        virtual QString toAscii() const = 0;

        virtual ConvertResult fromBinary(const QByteArray &a) = 0;

        virtual ConvertResult fromAscii(const QString &s) = 0;
};

class QCA_EXPORT KeyStoreEntryContext : public BasicContext
{
        Q_OBJECT
public:
        KeyStoreEntryContext(Provider *p) : BasicContext(p, "keystoreentry") {}

        virtual KeyStoreEntry::Type type() const = 0;

        virtual QString id() const = 0;

        virtual QString name() const = 0;

        virtual QString storeId() const = 0;

        virtual QString storeName() const = 0;

        virtual bool isAvailable() const;

        virtual QString serialize() const = 0;

        virtual KeyBundle keyBundle() const;

        virtual Certificate certificate() const;

        virtual CRL crl() const;

        virtual PGPKey pgpSecretKey() const;

        virtual PGPKey pgpPublicKey() const;

        virtual bool ensureAccess();
};

class QCA_EXPORT KeyStoreListContext : public Provider::Context
{
        Q_OBJECT
public:
        KeyStoreListContext(Provider *p) : Provider::Context(p, "keystorelist") {}

        virtual void start();

        virtual void setUpdatesEnabled(bool enabled);

        virtual QList<int> keyStores() = 0;

        virtual KeyStore::Type type(int id) const = 0;

        virtual QString storeId(int id) const = 0;

        virtual QString name(int id) const = 0;

        virtual bool isReadOnly(int id) const;

        virtual QList<KeyStoreEntry::Type> entryTypes(int id) const = 0;

        virtual QList<KeyStoreEntryContext*> entryList(int id) = 0;

        virtual KeyStoreEntryContext *entry(int id, const QString &entryId);

        virtual KeyStoreEntryContext *entryPassive(const QString &serialized);

        virtual QString writeEntry(int id, const KeyBundle &kb);

        virtual QString writeEntry(int id, const Certificate &cert);

        virtual QString writeEntry(int id, const CRL &crl);

        virtual QString writeEntry(int id, const PGPKey &key);

        virtual bool removeEntry(int id, const QString &entryId);

Q_SIGNALS:
        void busyStart();

        void busyEnd();

        void updated();

        void diagnosticText(const QString &str);

        void storeUpdated(int id);
};

class QCA_EXPORT TLSSessionContext : public BasicContext
{
        Q_OBJECT
public:
        TLSSessionContext(Provider *p) : BasicContext(p, "tlssession") {}
};

class QCA_EXPORT TLSContext : public Provider::Context
{
        Q_OBJECT
public:
        class SessionInfo
        {
        public:
                bool isCompressed;

                TLS::Version version;

                QString cipherSuite;

                int cipherBits;

                int cipherMaxBits;

                TLSSessionContext *id;
        };

        enum Result
        {
                Success, 
                Error,   
                Continue 
        };

        TLSContext(Provider *p, const QString &type) : Provider::Context(p, type) {}

        virtual void reset() = 0;

        virtual QStringList supportedCipherSuites(const TLS::Version &version) const = 0;

        virtual bool canCompress() const = 0;

        virtual bool canSetHostName() const = 0;

        virtual int maxSSF() const = 0;

        virtual void setup(bool serverMode, const QString &hostName, bool compress) = 0;

        virtual void setConstraints(int minSSF, int maxSSF) = 0;

        virtual void setConstraints(const QStringList &cipherSuiteList) = 0;

        virtual void setTrustedCertificates(const CertificateCollection &trusted) = 0;

        virtual void setIssuerList(const QList<CertificateInfoOrdered> &issuerList) = 0;

        virtual void setCertificate(const CertificateChain &cert, const PrivateKey &key) = 0;

        virtual void setSessionId(const TLSSessionContext &id) = 0;

        virtual void shutdown() = 0;

        virtual void setMTU(int size);

        virtual void start() = 0;

        virtual void update(const QByteArray &from_net, const QByteArray &from_app) = 0;

        virtual bool waitForResultsReady(int msecs) = 0;

        virtual Result result() const = 0;

        virtual QByteArray to_net() = 0;

        virtual int encoded() const = 0;

        virtual QByteArray to_app() = 0;

        virtual bool eof() const = 0;

        virtual bool clientHelloReceived() const = 0;

        virtual bool serverHelloReceived() const = 0;

        virtual QString hostName() const = 0;

        virtual bool certificateRequested() const = 0;

        virtual QList<CertificateInfoOrdered> issuerList() const = 0;

        virtual Validity peerCertificateValidity() const = 0;

        virtual CertificateChain peerCertificateChain() const = 0;

        virtual SessionInfo sessionInfo() const = 0;

        virtual QByteArray unprocessed() = 0;

Q_SIGNALS:
        void resultsReady();

        void dtlsTimeout();
};

class QCA_EXPORT SASLContext : public Provider::Context
{
        Q_OBJECT
public:
        class HostPort
        {
        public:
                QString addr;

                quint16 port;
        };

        enum Result
        {
                Success,   
                Error,     
                Params,    
                AuthCheck, 
                Continue   
        };

        SASLContext(Provider *p) : Provider::Context(p, "sasl") {}

        virtual void reset() = 0;

        virtual void setup(const QString &service, const QString &host, const HostPort *local, const HostPort *remote, const QString &ext_id, int ext_ssf) = 0;

        virtual void setConstraints(SASL::AuthFlags f, int minSSF, int maxSSF) = 0;

        virtual void startClient(const QStringList &mechlist, bool allowClientSendFirst) = 0;

        virtual void startServer(const QString &realm, bool disableServerSendLast) = 0;

        virtual void serverFirstStep(const QString &mech, const QByteArray *clientInit) = 0;

        virtual void nextStep(const QByteArray &from_net) = 0;

        virtual void tryAgain() = 0;

        virtual void update(const QByteArray &from_net, const QByteArray &from_app) = 0;

        virtual bool waitForResultsReady(int msecs) = 0;

        virtual Result result() const = 0;

        virtual QStringList mechlist() const = 0;

        virtual QString mech() const = 0;

        virtual bool haveClientInit() const = 0;

        virtual QByteArray stepData() const = 0;

        virtual QByteArray to_net() = 0;

        virtual int encoded() const = 0;

        virtual QByteArray to_app() = 0;

        virtual int ssf() const = 0;

        virtual SASL::AuthCondition authCondition() const = 0;

        virtual SASL::Params clientParams() const = 0;

        virtual void setClientParams(const QString *user, const QString *authzid, const SecureArray *pass, const QString *realm) = 0;

        virtual QStringList realmlist() const = 0;

        virtual QString username() const = 0;

        virtual QString authzid() const = 0;

Q_SIGNALS:
        void resultsReady();
};

class QCA_EXPORT MessageContext : public Provider::Context
{
        Q_OBJECT
public:
        enum Operation
        {
                Encrypt,       
                Decrypt,       
                Sign,          
                Verify,        
                SignAndEncrypt 
        };

        MessageContext(Provider *p, const QString &type) : Provider::Context(p, type) {}

        virtual bool canSignMultiple() const = 0;

        virtual SecureMessage::Type type() const = 0;

        virtual void reset() = 0;

        virtual void setupEncrypt(const SecureMessageKeyList &keys) = 0;

        virtual void setupSign(const SecureMessageKeyList &keys, SecureMessage::SignMode m, bool bundleSigner, bool smime) = 0;

        virtual void setupVerify(const QByteArray &detachedSig) = 0;

        virtual void start(SecureMessage::Format f, Operation op) = 0;

        virtual void update(const QByteArray &in) = 0;

        virtual QByteArray read() = 0;

        virtual int written() = 0;

        virtual void end() = 0;

        virtual bool finished() const = 0;

        virtual bool waitForFinished(int msecs) = 0;

        virtual bool success() const = 0;

        virtual SecureMessage::Error errorCode() const = 0;

        virtual QByteArray signature() const = 0;

        virtual QString hashName() const = 0;

        virtual SecureMessageSignatureList signers() const = 0;

        virtual QString diagnosticText() const;

Q_SIGNALS:
        void updated();
};

class QCA_EXPORT SMSContext : public BasicContext
{
        Q_OBJECT
public:
        SMSContext(Provider *p, const QString &type) : BasicContext(p, type) {}

        virtual void setTrustedCertificates(const CertificateCollection &trusted);

        virtual void setUntrustedCertificates(const CertificateCollection &untrusted);

        virtual void setPrivateKeys(const QList<SecureMessageKey> &keys);

        virtual MessageContext *createMessage() = 0;
};

}
#endif

#endif