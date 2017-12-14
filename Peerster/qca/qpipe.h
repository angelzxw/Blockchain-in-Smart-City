/*
 * Copyright (C) 2003-2007  Justin Karneges <justin@affinix.com>
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

#ifndef QPIPE_H
#define QPIPE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef QPIPE_NO_SECURE
# define QPIPE_SECURE
#endif

#ifdef QPIPE_SECURE
# include "QtCrypto"
#else
# define QCA_EXPORT
#endif

// defs adapted qprocess_p.h
#ifdef Q_OS_WIN
#include <windows.h>
typedef HANDLE Q_PIPE_ID;
#define INVALID_Q_PIPE_ID INVALID_HANDLE_VALUE
#else
typedef int Q_PIPE_ID;
#define INVALID_Q_PIPE_ID -1
#endif

#endif

// Note: for Windows console, I/O must be in UTF-8.  Reads are guaranteed to
//   to completely decode (no partial characters).  Likewise, writes must
//   not contain partial characters.

namespace QCA {


class QCA_EXPORT QPipeDevice : public QObject
{
        Q_OBJECT
public:
        enum Type
        {
                Read, 
                Write 
        };

        QPipeDevice(QObject *parent = 0);
        ~QPipeDevice();

        Type type() const;

        bool isValid() const;

        Q_PIPE_ID id() const;

        int idAsInt() const;

        void take(Q_PIPE_ID id, Type t);

        void enable();

        void close();

        void release();

        bool setInheritable(bool enabled);

        int bytesAvailable() const;

        int read(char *data, int maxsize);

        int write(const char *data, int size);

        int writeResult(int *written) const;

Q_SIGNALS:
        void notify();

private:
        Q_DISABLE_COPY(QPipeDevice)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT QPipeEnd : public QObject
{
        Q_OBJECT
public:

        enum Error
        {
                ErrorEOF,    
                ErrorBroken  
        };

        QPipeEnd(QObject *parent = 0);

        ~QPipeEnd();

        void reset();

        QPipeDevice::Type type() const;

        bool isValid() const;

        Q_PIPE_ID id() const;

        int idAsInt() const;

        void take(Q_PIPE_ID id, QPipeDevice::Type t);

#ifdef QPIPE_SECURE

        void setSecurityEnabled(bool secure);
#endif

        void enable();

        void close();

        void release();

        bool setInheritable(bool enabled);

        void finalize();

        void finalizeAndRelease();

        int bytesAvailable() const;

        int bytesToWrite() const;

        QByteArray read(int bytes = -1);

        void write(const QByteArray &a);

#ifdef QPIPE_SECURE

        SecureArray readSecure(int bytes = -1);

        void writeSecure(const SecureArray &a);
#endif

        QByteArray takeBytesToWrite();

#ifdef QPIPE_SECURE

        SecureArray takeBytesToWriteSecure();
#endif

Q_SIGNALS:
        void readyRead();

        void bytesWritten(int bytes);

        void closed();

        void error(QCA::QPipeEnd::Error e);

private:
        Q_DISABLE_COPY(QPipeEnd)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT QPipe
{
public:
        QPipe(QObject *parent = 0);

        ~QPipe();

        void reset();

#ifdef QPIPE_SECURE

        bool create(bool secure = false);
#else

        bool create();
#endif

        QPipeEnd & readEnd() { return i; }

        QPipeEnd & writeEnd() { return o; }

private:
        Q_DISABLE_COPY(QPipe)

        QPipeEnd i, o;
};

}

#endif