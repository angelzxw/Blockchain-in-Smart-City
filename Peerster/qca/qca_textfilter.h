/*
 * qca_textfilter.h - Qt Cryptographic Architecture
 * Copyright (C) 2003-2005  Justin Karneges <justin@affinix.com>
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

#ifndef QCA_TEXTFILTER_H
#define QCA_TEXTFILTER_H

#include "qca_core.h"

namespace QCA {

class QCA_EXPORT TextFilter : public Filter
{
public:
        TextFilter(Direction dir);

        void setup(Direction dir);

        Direction direction() const;

        MemoryRegion encode(const MemoryRegion &a);

        MemoryRegion decode(const MemoryRegion &a);

        QString arrayToString(const MemoryRegion &a);

        MemoryRegion stringToArray(const QString &s);

        QString encodeString(const QString &s);

        QString decodeString(const QString &s);

protected:
        Direction _dir;
};

class QCA_EXPORT Hex : public TextFilter
{
public:
        Hex(Direction dir = Encode);

        virtual void clear();

        virtual MemoryRegion update(const MemoryRegion &a);

        virtual MemoryRegion final();

        virtual bool ok() const;

private:
        Q_DISABLE_COPY(Hex)

        uchar val;
        bool partial;
        bool _ok;
};

class QCA_EXPORT Base64 : public TextFilter
{
public:
        Base64(Direction dir = Encode);

        bool lineBreaksEnabled() const;

        int lineBreaksColumn() const;

        void setLineBreaksEnabled(bool b);

        void setLineBreaksColumn(int column);

        virtual void clear();

        virtual MemoryRegion update(const MemoryRegion &a);

        virtual MemoryRegion final();

        virtual bool ok() const;

private:
        Q_DISABLE_COPY(Base64)

        QByteArray partial;
        bool _ok;
        int col;
        bool _lb_enabled;
        int _lb_column;

        class Private;
        Private *d;
};

}

#endif