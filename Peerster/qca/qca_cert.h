/*
 * qca_cert.h - Qt Cryptographic Architecture
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

#ifndef QCA_CERT_H
#define QCA_CERT_H

#include <QMap>
#include <QDateTime>
#include "qca_core.h"
#include "qca_publickey.h"

namespace QCA {

class CertContext;
class CSRContext;
class CRLContext;
class Certificate;
class CRL;
class CertificateCollection;
class CertificateChain;


enum CertificateRequestFormat
{
        PKCS10, 
        SPKAC   
};

enum CertificateInfoTypeKnown
{
        CommonName,             
        Email,                  
        EmailLegacy,            
        Organization,           
        OrganizationalUnit,     
        Locality,               
        IncorporationLocality,  
        State,                  
        IncorporationState,     
        Country,                
        IncorporationCountry,   
        URI,                    
        DNS,                    
        IPAddress,              
        XMPP                    
};

class QCA_EXPORT CertificateInfoType
{
public:
        enum Section
        {
                DN,              
                AlternativeName  
        };

        CertificateInfoType();

        CertificateInfoType(CertificateInfoTypeKnown known);

        CertificateInfoType(const QString &id, Section section);

        CertificateInfoType(const CertificateInfoType &from);

        ~CertificateInfoType();

        CertificateInfoType & operator=(const CertificateInfoType &from);

        Section section() const;

        CertificateInfoTypeKnown known() const;

        QString id() const;

        bool operator<(const CertificateInfoType &other) const;

        bool operator==(const CertificateInfoType &other) const;

        inline bool operator!=(const CertificateInfoType &other) const
        {
                return !(*this == other);
        }

private:
        class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT CertificateInfoPair
{
public:
        CertificateInfoPair();

        CertificateInfoPair(const CertificateInfoType &type, const QString &value);

        CertificateInfoPair(const CertificateInfoPair &from);

        ~CertificateInfoPair();

        CertificateInfoPair & operator=(const CertificateInfoPair &from);

        CertificateInfoType type() const;

        QString value() const;

        bool operator==(const CertificateInfoPair &other) const;

        inline bool operator!=(const CertificateInfoPair &other) const
        {
                return !(*this == other);
        }

private:
        class Private;
        QSharedDataPointer<Private> d;
};


enum ConstraintTypeKnown
{
        // KeyUsage
        DigitalSignature,    
        NonRepudiation,      
        KeyEncipherment,     
        DataEncipherment,    
        KeyAgreement,        
        KeyCertificateSign,  
        CRLSign,             
        EncipherOnly,        
        DecipherOnly,        

        // ExtKeyUsage
        ServerAuth,       
        ClientAuth,       
        CodeSigning,      
        EmailProtection,  
        IPSecEndSystem,   
        IPSecTunnel,      
        IPSecUser,        
        TimeStamping,     
        OCSPSigning       
};

class QCA_EXPORT ConstraintType
{
public:
        enum Section
        {
                KeyUsage,          
                ExtendedKeyUsage   
        };

        ConstraintType();

        ConstraintType(ConstraintTypeKnown known);

        ConstraintType(const QString &id, Section section);

        ConstraintType(const ConstraintType &from);

        ~ConstraintType();

        ConstraintType & operator=(const ConstraintType &from);

        Section section() const;

        ConstraintTypeKnown known() const;

        QString id() const;

        bool operator<(const ConstraintType &other) const;

        bool operator==(const ConstraintType &other) const;

        inline bool operator!=(const ConstraintType &other) const
        {
                return !(*this == other);
        }

private:
        class Private;
        QSharedDataPointer<Private> d;
};

enum UsageMode
{
        UsageAny             = 0x00, 
        UsageTLSServer       = 0x01, 
        UsageTLSClient       = 0x02, 
        UsageCodeSigning     = 0x04, 
        UsageEmailProtection = 0x08, 
        UsageTimeStamping    = 0x10, 
        UsageCRLSigning      = 0x20  
};

enum Validity
{
        ValidityGood,            
        ErrorRejected,           
        ErrorUntrusted,          
        ErrorSignatureFailed,    
        ErrorInvalidCA,          
        ErrorInvalidPurpose,     
        ErrorSelfSigned,         
        ErrorRevoked,            
        ErrorPathLengthExceeded, 
        ErrorExpired,            
        ErrorExpiredCA,          
        ErrorValidityUnknown = 64  
};

enum ValidateFlags
{
        ValidateAll     = 0x00,  // Verify all conditions
        ValidateRevoked = 0x01,  // Verify the certificate was not revoked
        ValidateExpired = 0x02,  // Verify the certificate has not expired
        ValidatePolicy  = 0x04   // Verify the certificate can be used for a specified purpose
};

typedef QMultiMap<CertificateInfoType, QString> CertificateInfo;

class CertificateInfoOrdered : public QList<CertificateInfoPair>
{
public:
        inline QString toString() const;

        inline CertificateInfoOrdered dnOnly() const;
};

QCA_EXPORT QString orderedToDNString(const CertificateInfoOrdered &in);

QCA_EXPORT CertificateInfoOrdered orderedDNOnly(const CertificateInfoOrdered &in);

inline QString CertificateInfoOrdered::toString() const
{
        return orderedToDNString(*this);
}

inline CertificateInfoOrdered CertificateInfoOrdered::dnOnly() const
{
        return orderedDNOnly(*this);
}

typedef QList<ConstraintType> Constraints;

QCA_EXPORT QStringList makeFriendlyNames(const QList<Certificate> &list);

class QCA_EXPORT CertificateOptions
{
public:
        CertificateOptions(CertificateRequestFormat format = PKCS10);

        CertificateOptions(const CertificateOptions &from);
        ~CertificateOptions();

        CertificateOptions & operator=(const CertificateOptions &from);

        CertificateRequestFormat format() const;

        void setFormat(CertificateRequestFormat f);

        bool isValid() const;

        QString challenge() const;

        CertificateInfo info() const;

        CertificateInfoOrdered infoOrdered() const;

        Constraints constraints() const;

        QStringList policies() const;

        QStringList crlLocations() const;

        QStringList issuerLocations() const;

        QStringList ocspLocations() const;

        bool isCA() const;

        int pathLimit() const;

        BigInteger serialNumber() const;

        QDateTime notValidBefore() const;

        QDateTime notValidAfter() const;

        void setChallenge(const QString &s);

        void setInfo(const CertificateInfo &info);

        void setInfoOrdered(const CertificateInfoOrdered &info);

        void setConstraints(const Constraints &constraints);

        void setPolicies(const QStringList &policies);

        void setCRLLocations(const QStringList &locations);

        void setIssuerLocations(const QStringList &locations);

        void setOCSPLocations(const QStringList &locations);

        void setAsCA(int pathLimit = 8); // value from Botan

        void setAsUser();

        void setSerialNumber(const BigInteger &i);

        void setValidityPeriod(const QDateTime &start, const QDateTime &end);

private:
        class Private;
        Private *d;
};

class QCA_EXPORT Certificate : public Algorithm
{
public:
        Certificate();

        Certificate(const QString &fileName);

        Certificate(const CertificateOptions &opts, const PrivateKey &key, const QString &provider = QString());

        Certificate(const Certificate &from);

        ~Certificate();

        Certificate & operator=(const Certificate &from);

        bool isNull() const;

        QDateTime notValidBefore() const;

        QDateTime notValidAfter() const;

        CertificateInfo subjectInfo() const;

        CertificateInfoOrdered subjectInfoOrdered() const;

        CertificateInfo issuerInfo() const;

        CertificateInfoOrdered issuerInfoOrdered() const;

        Constraints constraints() const;

        QStringList policies() const;

        QStringList crlLocations() const;

        QStringList issuerLocations() const;

        QStringList ocspLocations() const;

        QString commonName() const;

        BigInteger serialNumber() const;

        PublicKey subjectPublicKey() const;

        bool isCA() const;

        bool isSelfSigned() const;

        bool isIssuerOf(const Certificate &other) const;

        int pathLimit() const;

        SignatureAlgorithm signatureAlgorithm() const;

        QByteArray subjectKeyId() const;

        QByteArray issuerKeyId() const;

        Validity validate(const CertificateCollection &trusted, const CertificateCollection &untrusted, UsageMode u = UsageAny, ValidateFlags vf = ValidateAll) const;

        QByteArray toDER() const;

        QString toPEM() const;

        bool toPEMFile(const QString &fileName) const;

        static Certificate fromDER(const QByteArray &a, ConvertResult *result = 0, const QString &provider = QString());

        static Certificate fromPEM(const QString &s, ConvertResult *result = 0, const QString &provider = QString());

        static Certificate fromPEMFile(const QString &fileName, ConvertResult *result = 0, const QString &provider = QString());

        bool matchesHostName(const QString &host) const;

        bool operator==(const Certificate &a) const;

        inline bool operator!=(const Certificate &other) const
        {
                return !(*this == other);
        }

        void change(CertContext *c);

private:
        class Private;
        friend class Private;
        QSharedDataPointer<Private> d;

        friend class CertificateChain;
        Validity chain_validate(const CertificateChain &chain, const CertificateCollection &trusted, const QList<CRL> &untrusted_crls, UsageMode u, ValidateFlags vf) const;
        CertificateChain chain_complete(const CertificateChain &chain, const QList<Certificate> &issuers, Validity *result) const;
};

class CertificateChain : public QList<Certificate>
{
public:
        inline CertificateChain() {}

        inline CertificateChain(const Certificate &primary) { append(primary); }

        inline const Certificate & primary() const { return first(); }

        inline Validity validate(const CertificateCollection &trusted, const QList<CRL> &untrusted_crls = QList<CRL>(), UsageMode u = UsageAny, ValidateFlags vf = ValidateAll) const;

        inline CertificateChain complete(const QList<Certificate> &issuers = QList<Certificate>(), Validity *result = 0) const;
};

inline Validity CertificateChain::validate(const CertificateCollection &trusted, const QList<CRL> &untrusted_crls, UsageMode u, ValidateFlags vf) const
{
        if(isEmpty())
                return ErrorValidityUnknown;
        return first().chain_validate(*this, trusted, untrusted_crls, u, vf);
}

inline CertificateChain CertificateChain::complete(const QList<Certificate> &issuers, Validity *result) const
{
        if(isEmpty())
                return CertificateChain();
        return first().chain_complete(*this, issuers, result);
}

class QCA_EXPORT CertificateRequest : public Algorithm
{
public:
        CertificateRequest();

        CertificateRequest(const QString &fileName);

        CertificateRequest(const CertificateOptions &opts, const PrivateKey &key, const QString &provider = QString());

        CertificateRequest(const CertificateRequest &from);

        ~CertificateRequest();

        CertificateRequest & operator=(const CertificateRequest &from);

        bool isNull() const;

        static bool canUseFormat(CertificateRequestFormat f, const QString &provider = QString());

        CertificateRequestFormat format() const;

        CertificateInfo subjectInfo() const;

        CertificateInfoOrdered subjectInfoOrdered() const;

        Constraints constraints() const;

        QStringList policies() const;

        PublicKey subjectPublicKey() const;

        bool isCA() const;

        int pathLimit() const;

        QString challenge() const;

        SignatureAlgorithm signatureAlgorithm() const;

        bool operator==(const CertificateRequest &csr) const;

        inline bool operator!=(const CertificateRequest &other) const
        {
                return !(*this == other);
        }

        QByteArray toDER() const;

        QString toPEM() const;

        bool toPEMFile(const QString &fileName) const;

        static CertificateRequest fromDER(const QByteArray &a, ConvertResult *result = 0, const QString &provider = QString());

        static CertificateRequest fromPEM(const QString &s, ConvertResult *result = 0, const QString &provider = QString());

        static CertificateRequest fromPEMFile(const QString &fileName, ConvertResult *result = 0, const QString &provider = QString());

        QString toString() const;

        static CertificateRequest fromString(const QString &s, ConvertResult *result = 0, const QString &provider = QString());

        void change(CSRContext *c);

private:
        class Private;
        friend class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT CRLEntry
{
public:
        enum Reason
        {
                Unspecified,        
                KeyCompromise,      
                CACompromise,       
                AffiliationChanged,
                Superseded,         
                CessationOfOperation,
                CertificateHold,    
                RemoveFromCRL,      
                PrivilegeWithdrawn,
                AACompromise        
        };

        CRLEntry();

        explicit CRLEntry(const Certificate &c, Reason r = Unspecified);

        CRLEntry(const BigInteger serial, const QDateTime &time, Reason r = Unspecified);

        CRLEntry(const CRLEntry &from);

        ~CRLEntry();

        CRLEntry & operator=(const CRLEntry &from);

        BigInteger serialNumber() const;

        QDateTime time() const;

        bool isNull() const;

        Reason reason() const;

        bool operator<(const CRLEntry &a) const;

        bool operator==(const CRLEntry &a) const;

        inline bool operator!=(const CRLEntry &other) const
        {
                return !(*this == other);
        }

private:
        BigInteger _serial;
        QDateTime _time;
        Reason _reason;

        class Private;
        Private *d;
};

class QCA_EXPORT CRL : public Algorithm
{
public:
        CRL();

        CRL(const CRL &from);

        ~CRL();

        CRL & operator=(const CRL &from);

        bool isNull() const;

        CertificateInfo issuerInfo() const;

        CertificateInfoOrdered issuerInfoOrdered() const;

        int number() const;

        QDateTime thisUpdate() const;

        QDateTime nextUpdate() const;

        QList<CRLEntry> revoked() const;

        SignatureAlgorithm signatureAlgorithm() const;

        QByteArray issuerKeyId() const;

        bool operator==(const CRL &a) const;

        inline bool operator!=(const CRL &other) const
        {
                return !(*this == other);
        }

        QByteArray toDER() const;

        QString toPEM() const;

        bool toPEMFile(const QString &fileName) const;

        static CRL fromDER(const QByteArray &a, ConvertResult *result = 0, const QString &provider = QString());

        static CRL fromPEM(const QString &s, ConvertResult *result = 0, const QString &provider = QString());

        static CRL fromPEMFile(const QString &fileName, ConvertResult *result = 0, const QString &provider = QString());

        void change(CRLContext *c);

private:
        class Private;
        friend class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT CertificateCollection
{
public:
        CertificateCollection();

        CertificateCollection(const CertificateCollection &from);

        ~CertificateCollection();

        CertificateCollection & operator=(const CertificateCollection &from);

        void addCertificate(const Certificate &cert);

        void addCRL(const CRL &crl);

        QList<Certificate> certificates() const;

        QList<CRL> crls() const;

        void append(const CertificateCollection &other);

        CertificateCollection operator+(const CertificateCollection &other) const;

        CertificateCollection & operator+=(const CertificateCollection &other);

        static bool canUsePKCS7(const QString &provider = QString());

        bool toFlatTextFile(const QString &fileName);

        bool toPKCS7File(const QString &fileName, const QString &provider = QString());

        static CertificateCollection fromFlatTextFile(const QString &fileName, ConvertResult *result = 0, const QString &provider = QString());

        static CertificateCollection fromPKCS7File(const QString &fileName, ConvertResult *result = 0, const QString &provider = QString());

private:
        class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT CertificateAuthority : public Algorithm
{
public:
        CertificateAuthority(const Certificate &cert, const PrivateKey &key, const QString &provider);

        CertificateAuthority(const CertificateAuthority &from);

        ~CertificateAuthority();

        CertificateAuthority & operator=(const CertificateAuthority &from);

        Certificate certificate() const;

        Certificate signRequest(const CertificateRequest &req, const QDateTime &notValidAfter) const;

        Certificate createCertificate(const PublicKey &key, const CertificateOptions &opts) const;

        CRL createCRL(const QDateTime &nextUpdate) const;

        CRL updateCRL(const CRL &crl, const QList<CRLEntry> &entries, const QDateTime &nextUpdate) const;

private:
        class Private;
        Private *d;
};

class QCA_EXPORT KeyBundle
{
public:
        KeyBundle();

        explicit KeyBundle(const QString &fileName, const SecureArray &passphrase = SecureArray());

        KeyBundle(const KeyBundle &from);

        ~KeyBundle();

        KeyBundle & operator=(const KeyBundle &from);

        bool isNull() const;

        QString name() const;

        CertificateChain certificateChain() const;

        PrivateKey privateKey() const;

        void setName(const QString &s);

        void setCertificateChainAndKey(const CertificateChain &c, const PrivateKey &key);

        QByteArray toArray(const SecureArray &passphrase, const QString &provider = QString()) const;

        bool toFile(const QString &fileName, const SecureArray &passphrase, const QString &provider = QString()) const;

        static KeyBundle fromArray(const QByteArray &a, const SecureArray &passphrase = SecureArray(), ConvertResult *result = 0, const QString &provider = QString());

        static KeyBundle fromFile(const QString &fileName, const SecureArray &passphrase = SecureArray(), ConvertResult *result = 0, const QString &provider = QString());

private:
        class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT PGPKey : public Algorithm
{
public:
        PGPKey();

        PGPKey(const QString &fileName);

        PGPKey(const PGPKey &from);

        ~PGPKey();

        PGPKey & operator=(const PGPKey &from);

        bool isNull() const;

        QString keyId() const;

        QString primaryUserId() const;

        QStringList userIds() const;

        bool isSecret() const;

        QDateTime creationDate() const;

        QDateTime expirationDate() const;

        QString fingerprint() const;

        bool inKeyring() const;

        bool isTrusted() const;

        QByteArray toArray() const;

        QString toString() const;

        bool toFile(const QString &fileName) const;

        static PGPKey fromArray(const QByteArray &a, ConvertResult *result = 0, const QString &provider = QString());

        static PGPKey fromString(const QString &s, ConvertResult *result = 0, const QString &provider = QString());

        static PGPKey fromFile(const QString &fileName, ConvertResult *result = 0, const QString &provider = QString());

private:
        class Private;
        Private *d;
};

class QCA_EXPORT KeyLoader : public QObject
{
        Q_OBJECT
public:
        KeyLoader(QObject *parent = 0);
        ~KeyLoader();

        void loadPrivateKeyFromPEMFile(const QString &fileName);

        void loadPrivateKeyFromPEM(const QString &s);

        void loadPrivateKeyFromDER(const SecureArray &a);

        void loadKeyBundleFromFile(const QString &fileName);

        void loadKeyBundleFromArray(const QByteArray &a);

        ConvertResult convertResult() const;

        PrivateKey privateKey() const;

        KeyBundle keyBundle() const;

Q_SIGNALS:
        void finished();

private:
        Q_DISABLE_COPY(KeyLoader)

        class Private;
        friend class Private;
        Private *d;
};

}

#endif