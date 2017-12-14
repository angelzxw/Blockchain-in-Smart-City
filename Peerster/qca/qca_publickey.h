/*
 * qca_publickey.h - Qt Cryptographic Architecture
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

#ifndef QCA_PUBLICKEY_H
#define QCA_PUBLICKEY_H

#include <QObject>
#include "qca_core.h"

namespace QCA {

class PublicKey;
class PrivateKey;
class KeyGenerator;
class RSAPublicKey;
class RSAPrivateKey;
class DSAPublicKey;
class DSAPrivateKey;
class DHPublicKey;
class DHPrivateKey;

enum EncryptionAlgorithm
{
        EME_PKCS1v15,  
        EME_PKCS1_OAEP 
};

enum SignatureAlgorithm
{
        SignatureUnknown, 
        EMSA1_SHA1,       
        EMSA3_SHA1,       
        EMSA3_MD5,        
        EMSA3_MD2,        
        EMSA3_RIPEMD160,  
        EMSA3_Raw         
};

enum SignatureFormat
{
        DefaultFormat, 
        IEEE_1363,     
        DERSequence    
};

enum PBEAlgorithm
{
        PBEDefault,           
        PBES2_DES_SHA1,       
        PBES2_TripleDES_SHA1, 
        PBES2_AES128_SHA1,    
        PBES2_AES192_SHA1,    
        PBES2_AES256_SHA1     
};

enum ConvertResult
{
        ConvertGood,      
        ErrorDecode,      
        ErrorPassphrase,  
        ErrorFile         
};

enum DLGroupSet
{
        DSA_512,    
        DSA_768,    
        DSA_1024,   
        IETF_768,   
        IETF_1024,  
        IETF_1536,  
        IETF_2048,  
        IETF_3072,  
        IETF_4096,  
        IETF_6144,  
        IETF_8192  

};

QCA_EXPORT QByteArray emsa3Encode(const QString &hashName, const QByteArray &digest, int size = -1);

class QCA_EXPORT DLGroup
{
public:
        DLGroup();

        DLGroup(const BigInteger &p, const BigInteger &q, const BigInteger &g);

        DLGroup(const BigInteger &p, const BigInteger &g);

        DLGroup(const DLGroup &from);
        ~DLGroup();

        DLGroup & operator=(const DLGroup &from);

        static QList<DLGroupSet> supportedGroupSets(const QString &provider = QString());

        bool isNull() const;

        BigInteger p() const;

        BigInteger q() const;

        BigInteger g() const;

private:
        class Private;
        Private *d;
};

class QCA_EXPORT PKey : public Algorithm
{
public:
        enum Type {
                RSA, 
                DSA, 
                DH   
        };

        PKey();

        PKey(const PKey &from);

        ~PKey();

        PKey & operator=(const PKey &from);

        static QList<Type> supportedTypes(const QString &provider = QString());

        static QList<Type> supportedIOTypes(const QString &provider = QString());

        bool isNull() const;

        Type type() const;

        int bitSize() const;

        bool isRSA() const;

        bool isDSA() const;

        bool isDH() const;

        bool isPublic() const;  

        bool isPrivate() const;

        bool canExport() const;

        bool canKeyAgree() const;

        PublicKey toPublicKey() const;

        PrivateKey toPrivateKey() const;

        bool operator==(const PKey &a) const;

        bool operator!=(const PKey &a) const;

protected:
        PKey(const QString &type, const QString &provider);

        void set(const PKey &k);

        RSAPublicKey toRSAPublicKey() const;

        RSAPrivateKey toRSAPrivateKey() const;

        DSAPublicKey toDSAPublicKey() const;

        DSAPrivateKey toDSAPrivateKey() const;

        DHPublicKey toDHPublicKey() const;

        DHPrivateKey toDHPrivateKey() const;

private:
        void assignToPublic(PKey *dest) const;
        void assignToPrivate(PKey *dest) const;

        class Private;
        Private *d;
};

class QCA_EXPORT PublicKey : public PKey
{
public:
        PublicKey();

        PublicKey(const PrivateKey &k);

        PublicKey(const QString &fileName);

        PublicKey(const PublicKey &from);

        ~PublicKey();

        PublicKey & operator=(const PublicKey &from);

        RSAPublicKey toRSA() const;

        DSAPublicKey toDSA() const;

        DHPublicKey toDH() const;

        bool canEncrypt() const;

        bool canVerify() const;

        int maximumEncryptSize(EncryptionAlgorithm alg) const;

        SecureArray encrypt(const SecureArray &a, EncryptionAlgorithm alg);

        void startVerify(SignatureAlgorithm alg, SignatureFormat format = DefaultFormat);

        void update(const MemoryRegion &a);

        bool validSignature(const QByteArray &sig);

        bool verifyMessage(const MemoryRegion &a, const QByteArray &sig, SignatureAlgorithm alg, SignatureFormat format = DefaultFormat);

        QByteArray toDER() const;

        QString toPEM() const;

        bool toPEMFile(const QString &fileName) const;

        static PublicKey fromDER(const QByteArray &a, ConvertResult *result = 0, const QString &provider = QString());

        static PublicKey fromPEM(const QString &s, ConvertResult *result = 0, const QString &provider = QString());

        static PublicKey fromPEMFile(const QString &fileName, ConvertResult *result = 0, const QString &provider = QString());

protected:
        PublicKey(const QString &type, const QString &provider);

private:
        class Private;
        Private *d;
};

class QCA_EXPORT PrivateKey : public PKey
{
public:
        PrivateKey();

        explicit PrivateKey(const QString &fileName, const SecureArray &passphrase = SecureArray());

        PrivateKey(const PrivateKey &from);

        ~PrivateKey();

        PrivateKey & operator=(const PrivateKey &from);

        RSAPrivateKey toRSA() const;

        DSAPrivateKey toDSA() const;

        DHPrivateKey toDH() const;

        bool canDecrypt() const;

        bool canSign() const;

        bool decrypt(const SecureArray &in, SecureArray *out, EncryptionAlgorithm alg);

        void startSign(SignatureAlgorithm alg, SignatureFormat format = DefaultFormat);

        void update(const MemoryRegion &a);

        QByteArray signature();

        QByteArray signMessage(const MemoryRegion &a, SignatureAlgorithm alg, SignatureFormat format = DefaultFormat);

        SymmetricKey deriveKey(const PublicKey &theirs);

        static QList<PBEAlgorithm> supportedPBEAlgorithms(const QString &provider = QString());

        SecureArray toDER(const SecureArray &passphrase = SecureArray(), PBEAlgorithm pbe = PBEDefault) const;

        QString toPEM(const SecureArray &passphrase = SecureArray(), PBEAlgorithm pbe = PBEDefault) const;

        bool toPEMFile(const QString &fileName, const SecureArray &passphrase = SecureArray(), PBEAlgorithm pbe = PBEDefault) const;

        static PrivateKey fromDER(const SecureArray &a, const SecureArray &passphrase = SecureArray(), ConvertResult *result = 0, const QString &provider = QString());

        static PrivateKey fromPEM(const QString &s, const SecureArray &passphrase = SecureArray(), ConvertResult *result = 0, const QString &provider = QString());

        static PrivateKey fromPEMFile(const QString &fileName, const SecureArray &passphrase = SecureArray(), ConvertResult *result = 0, const QString &provider = QString());

protected:
        PrivateKey(const QString &type, const QString &provider);

private:
        class Private;
        Private *d;
};

class QCA_EXPORT KeyGenerator : public QObject
{
        Q_OBJECT
public:
        KeyGenerator(QObject *parent = 0);

        ~KeyGenerator();

        bool blockingEnabled() const;

        void setBlockingEnabled(bool b);

        bool isBusy() const;

        PrivateKey createRSA(int bits, int exp = 65537, const QString &provider = QString());

        PrivateKey createDSA(const DLGroup &domain, const QString &provider = QString());

        PrivateKey createDH(const DLGroup &domain, const QString &provider = QString());

        PrivateKey key() const;

        DLGroup createDLGroup(QCA::DLGroupSet set, const QString &provider = QString());

        DLGroup dlGroup() const;

Q_SIGNALS:
        void finished();

private:
        Q_DISABLE_COPY(KeyGenerator)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT RSAPublicKey : public PublicKey
{
public:
        RSAPublicKey();

        RSAPublicKey(const BigInteger &n, const BigInteger &e, const QString &provider = QString());

        RSAPublicKey(const RSAPrivateKey &k);

        BigInteger n() const;

        BigInteger e() const;
};

class QCA_EXPORT RSAPrivateKey : public PrivateKey
{
public:
        RSAPrivateKey();

        RSAPrivateKey(const BigInteger &n, const BigInteger &e, const BigInteger &p, const BigInteger &q, const BigInteger &d, const QString &provider = QString());

        BigInteger n() const;

        BigInteger e() const;

        BigInteger p() const;

        BigInteger q() const;

        BigInteger d() const;
};

class QCA_EXPORT DSAPublicKey : public PublicKey
{
public:
        DSAPublicKey();

        DSAPublicKey(const DLGroup &domain, const BigInteger &y, const QString &provider = QString());

        DSAPublicKey(const DSAPrivateKey &k);

        DLGroup domain() const;

        BigInteger y() const;
};

class QCA_EXPORT DSAPrivateKey : public PrivateKey
{
public:
        DSAPrivateKey();

        DSAPrivateKey(const DLGroup &domain, const BigInteger &y, const BigInteger &x, const QString &provider = QString());

        DLGroup domain() const;

        BigInteger y() const;

        BigInteger x() const;
};

class QCA_EXPORT DHPublicKey : public PublicKey
{
public:
        DHPublicKey();

        DHPublicKey(const DLGroup &domain, const BigInteger &y, const QString &provider = QString());

        DHPublicKey(const DHPrivateKey &k);

        DLGroup domain() const;

        BigInteger y() const;
};

class QCA_EXPORT DHPrivateKey : public PrivateKey
{
public:
        DHPrivateKey();

        DHPrivateKey(const DLGroup &domain, const BigInteger &y, const BigInteger &x, const QString &provider = QString());

        DLGroup domain() const;

        BigInteger y() const;

        BigInteger x() const;
};
}

#endif