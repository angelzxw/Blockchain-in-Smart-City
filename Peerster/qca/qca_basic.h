/*
 * qca_basic.h - Qt Cryptographic Architecture
 * Copyright (C) 2003-2007  Justin Karneges <justin@affinix.com>
 * Copyright (C) 2004-2007  Brad Hards <bradh@frogmouth.net>
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

#ifndef QCA_BASIC_H
#define QCA_BASIC_H

#include "qca_core.h"

namespace QCA {

class QCA_EXPORT Random : public Algorithm
{
public:
        Random(const QString &provider = QString());

        Random(const Random &from);

        ~Random();

        Random & operator=(const Random &from);

        uchar nextByte();

        SecureArray nextBytes(int size);

        static uchar randomChar();

        static int randomInt();

        static SecureArray randomArray(int size);

private:
        class Private;
        Private *d;
};

class QCA_EXPORT Hash : public Algorithm, public BufferedComputation
{
public:
        explicit Hash(const QString &type, const QString &provider = QString());

        Hash(const Hash &from);

        ~Hash();

        Hash & operator=(const Hash &from);

        static QStringList supportedTypes(const QString &provider = QString());

        QString type() const;

        virtual void clear();

        virtual void update(const MemoryRegion &a);

        void update(const QByteArray &a);

        void update(const char *data, int len = -1);

        void update(QIODevice *file);

        virtual MemoryRegion final();

        MemoryRegion hash(const MemoryRegion &array);

        QString hashToString(const MemoryRegion &array);

private:
        class Private;
        Private *d;
};

class QCA_EXPORT Cipher : public Algorithm, public Filter
{
public:
        enum Mode
        {
                CBC, 
                CFB, 
                ECB, 
                OFB  
        };

        enum Padding
        {
                DefaultPadding, 
                NoPadding,      
                PKCS7           
        };

        Cipher(const QString &type, Mode mode, Padding pad = DefaultPadding,
                Direction dir = Encode, const SymmetricKey &key = SymmetricKey(), 
                const InitializationVector &iv = InitializationVector(),
                const QString &provider = QString());

        Cipher(const Cipher &from);

        ~Cipher();

        Cipher & operator=(const Cipher &from);

        static QStringList supportedTypes(const QString &provider = QString());

        QString type() const;

        Mode mode() const;

        Padding padding() const;

        Direction direction() const;

        KeyLength keyLength() const;

        bool validKeyLength(int n) const;

        int blockSize() const;

        virtual void clear();

        virtual MemoryRegion update(const MemoryRegion &a);

        virtual MemoryRegion final();

        virtual bool ok() const;

        void setup(Direction dir, const SymmetricKey &key, const InitializationVector &iv = InitializationVector());

        static QString withAlgorithms(const QString &cipherType, Mode modeType, Padding paddingType);

private:
        class Private;
        Private *d;
};

class QCA_EXPORT MessageAuthenticationCode : public Algorithm, public BufferedComputation
{
public:
        MessageAuthenticationCode(const QString &type, const SymmetricKey &key, const QString &provider = QString());

        MessageAuthenticationCode(const MessageAuthenticationCode &from);

        ~MessageAuthenticationCode();

        MessageAuthenticationCode & operator=(const MessageAuthenticationCode &from);

        static QStringList supportedTypes(const QString &provider = QString());

        QString type() const;

        KeyLength keyLength() const;

        bool validKeyLength(int n) const;

        virtual void clear();

        virtual void update(const MemoryRegion &array);

        virtual MemoryRegion final();

        void setup(const SymmetricKey &key);

private:
        class Private;
        Private *d;
};

class QCA_EXPORT KeyDerivationFunction : public Algorithm
{
public:
        KeyDerivationFunction(const KeyDerivationFunction &from);

        ~KeyDerivationFunction();

        KeyDerivationFunction & operator=(const KeyDerivationFunction &from);

        SymmetricKey makeKey(const SecureArray &secret, const InitializationVector &salt, unsigned int keyLength, unsigned int iterationCount);

        static QString withAlgorithm(const QString &kdfType, const QString &algType);

protected:
        KeyDerivationFunction(const QString &type, const QString &provider);

private:
        class Private;
        Private *d;
};

class QCA_EXPORT PBKDF1 : public KeyDerivationFunction
{
public:
        explicit PBKDF1(const QString &algorithm = "sha1", const QString &provider = QString()) : KeyDerivationFunction(withAlgorithm("pbkdf1", algorithm), provider) {}
};

class QCA_EXPORT PBKDF2 : public KeyDerivationFunction
{
public:
        explicit PBKDF2(const QString &algorithm = "sha1", const QString &provider = QString()) : KeyDerivationFunction(withAlgorithm("pbkdf2", algorithm), provider) {}
};

}

#endif