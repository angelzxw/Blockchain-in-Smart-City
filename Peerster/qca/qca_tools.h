/*
 * qca_tools.h - Qt Cryptographic Architecture
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

#ifndef QCA_TOOLS_H
#define QCA_TOOLS_H

#include <QSharedData>
#include <QSharedDataPointer>
#include <QMetaType>
#include "qca_export.h"

class QString;
class QByteArray;
class QTextStream;

QCA_EXPORT void *qca_secure_alloc(int bytes);

QCA_EXPORT void qca_secure_free(void *p);

QCA_EXPORT void *qca_secure_realloc(void *p, int bytes);

namespace QCA {

class QCA_EXPORT MemoryRegion
{
public:
        MemoryRegion();

        MemoryRegion(const char *str);

        MemoryRegion(const QByteArray &from);

        MemoryRegion(const MemoryRegion &from);
        ~MemoryRegion();

        MemoryRegion & operator=(const MemoryRegion &from);

        MemoryRegion & operator=(const QByteArray &from);

        bool isNull() const;

        bool isSecure() const;

        QByteArray toByteArray() const;

        bool isEmpty() const;

        int size() const;

        const char *data() const;

        const char *constData() const;

        const char & at(int index) const;

protected:
        MemoryRegion(bool secure);

        MemoryRegion(int size, bool secure);

        MemoryRegion(const QByteArray &from, bool secure);

        char *data();

        char & at(int index);

        bool resize(int size);

        void set(const QByteArray &from, bool secure);

        void setSecure(bool secure);

private:
        bool _secure;
        class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT SecureArray : public MemoryRegion
{
public:
        SecureArray();

        explicit SecureArray(int size, char ch = 0);

        SecureArray(const char *str);

        SecureArray(const QByteArray &a);

        SecureArray(const MemoryRegion &a);

        SecureArray(const SecureArray &from);

        ~SecureArray();

        SecureArray & operator=(const SecureArray &from);

        SecureArray & operator=(const QByteArray &a);

        void clear();

        char & operator[](int index);

        const char & operator[](int index) const;

        char *data();

        const char *data() const;

        const char *constData() const;

        char & at(int index);

        const char & at(int index) const;

        int size() const;

        bool isEmpty() const;

        bool resize(int size);

        void fill(char fillChar, int fillToPosition = -1);

        QByteArray toByteArray() const;

        SecureArray & append(const SecureArray &a);

        bool operator==(const MemoryRegion &other) const;
        
        inline bool operator!=(const MemoryRegion &other) const
        {
                return !(*this == other);
        }

        SecureArray & operator+=(const SecureArray &a);

protected:
        void set(const SecureArray &from);

        void set(const QByteArray &from);
};

QCA_EXPORT const SecureArray operator+(const SecureArray &a, const SecureArray &b);

class QCA_EXPORT BigInteger
{
public:
        BigInteger();

        BigInteger(int n);

        BigInteger(const char *c);

        BigInteger(const QString &s);

        BigInteger(const QCA::SecureArray &a);

        BigInteger(const BigInteger &from);

        ~BigInteger();

        BigInteger & operator=(const BigInteger &from);

        BigInteger & operator=(const QString &s);

        BigInteger & operator+=(const BigInteger &b);

        BigInteger & operator-=(const BigInteger &b);

        BigInteger & operator*=(const BigInteger &b);

        BigInteger & operator/=(const BigInteger &b);

        BigInteger & operator%=(const BigInteger &b);

        QCA::SecureArray toArray() const;

        void fromArray(const QCA::SecureArray &a);

        QString toString() const;

        bool fromString(const QString &s);

        int compare(const BigInteger &n) const;

        inline bool operator==(const BigInteger &other) const
        {
                return (compare(other) == 0);
        }

        inline bool operator!=(const BigInteger &other) const
        {
                return !(*this == other);
        }

        inline bool operator<=(const BigInteger &other) const
        {
                return (compare(other) <= 0);
        }

        inline bool operator>=(const BigInteger &other) const
        {
                return (compare(other) >= 0);
        }

        inline bool operator<(const BigInteger &other) const
        {
                return (compare(other) < 0);
        }

        inline bool operator>(const BigInteger &other) const
        {
                return (compare(other) > 0);
        }

private:
        class Private;
        QSharedDataPointer<Private> d;
};



QCA_EXPORT QTextStream &operator<<(QTextStream &stream, const BigInteger &b);


}

#endif