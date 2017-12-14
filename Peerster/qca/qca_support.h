/*
 * qca_support.h - Qt Cryptographic Architecture
 * Copyright (C) 2003-2005  Justin Karneges <justin@affinix.com>
 * Copyright (C) 2004,2005, 2007  Brad Hards <bradh@frogmouth.net>
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

#ifndef QCA_SUPPORT_H
#define QCA_SUPPORT_H

#include <QByteArray>
#include <QString>
#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QStringList>
#include <QList>
#include <QMetaObject>
#include <QThread>
#include "qca_export.h"
#include "qca_tools.h"

namespace QCA {

QCA_EXPORT QByteArray methodReturnType(const QMetaObject *obj, const QByteArray &method, const QList<QByteArray> argTypes);

QCA_EXPORT bool invokeMethodWithVariants(QObject *obj, const QByteArray &method, const QVariantList &args, QVariant *ret, Qt::ConnectionType type = Qt::AutoConnection);

class QCA_EXPORT SyncThread : public QThread
{
        Q_OBJECT
public:
        SyncThread(QObject *parent = 0);

        ~SyncThread();

        void start();

        void stop();

        QVariant call(QObject *obj, const QByteArray &method, const QVariantList &args = QVariantList(), bool *ok = 0);

protected:
        virtual void atStart() = 0;

        virtual void atEnd() = 0;

        virtual void run();

private:
        Q_DISABLE_COPY(SyncThread)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT Synchronizer : public QObject
{
        Q_OBJECT
public:
        Synchronizer(QObject *parent);
        ~Synchronizer();

        bool waitForCondition(int msecs = -1);

        void conditionMet();

private:
        Q_DISABLE_COPY(Synchronizer)

        class Private;
        Private *d;
};

class QCA_EXPORT DirWatch : public QObject
{
        Q_OBJECT
public:
        explicit DirWatch(const QString &dir = QString(), QObject *parent = 0);
        ~DirWatch();

        QString dirName() const;

        void setDirName(const QString &dir);

Q_SIGNALS:
        void changed();

private:
        Q_DISABLE_COPY(DirWatch)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT FileWatch : public QObject
{
        Q_OBJECT
public:
        explicit FileWatch(const QString &file = QString(), QObject *parent = 0);
        ~FileWatch();

        QString fileName() const;

        void setFileName(const QString &file);

Q_SIGNALS:
        void changed();

private:
        Q_DISABLE_COPY(FileWatch)

        class Private;
        friend class Private;
        Private *d;
};

class ConsolePrivate;
class ConsoleReferencePrivate;
class ConsoleReference;

class QCA_EXPORT Console : public QObject
{
        Q_OBJECT
public:
        enum Type
        {
                Tty,         
                Stdio        
        };
        enum ChannelMode
        {
                Read,        
                ReadWrite    
        };

        enum TerminalMode
        {
                Default,     
                Interactive  
        };

        Console(Type type, ChannelMode cmode, TerminalMode tmode, QObject *parent = 0);
        ~Console();

        Type type() const;

        ChannelMode channelMode() const;

        TerminalMode terminalMode() const;

        static bool isStdinRedirected();

        static bool isStdoutRedirected();

        static Console *ttyInstance();

        static Console *stdioInstance();

        void release();

        QByteArray bytesLeftToRead();

        QByteArray bytesLeftToWrite();

private:
        Q_DISABLE_COPY(Console)

        friend class ConsolePrivate;
        ConsolePrivate *d;

        friend class ConsoleReference;
};

class QCA_EXPORT ConsoleReference : public QObject
{
        Q_OBJECT
public:
        enum SecurityMode
        {
                SecurityDisabled,
                SecurityEnabled
        };

        ConsoleReference(QObject *parent = 0);
        ~ConsoleReference();

        bool start(Console *console, SecurityMode mode = SecurityDisabled);

        void stop();

        Console *console() const;

        SecurityMode securityMode() const;

        QByteArray read(int bytes = -1);

        void write(const QByteArray &a);

        SecureArray readSecure(int bytes = -1);

        void writeSecure(const SecureArray &a);

        void closeOutput();

        int bytesAvailable() const;

        int bytesToWrite() const;

Q_SIGNALS:
        void readyRead();

        void bytesWritten(int bytes);

        void inputClosed();

        void outputClosed();

private:
        Q_DISABLE_COPY(ConsoleReference)

        friend class ConsoleReferencePrivate;
        ConsoleReferencePrivate *d;

        friend class Console;
};

class QCA_EXPORT ConsolePrompt : public QObject
{
        Q_OBJECT
public:
        ConsolePrompt(QObject *parent = 0);
        ~ConsolePrompt();

        void getHidden(const QString &promptStr);

        void getChar();

        void waitForFinished();

        SecureArray result() const;

        QChar resultChar() const;

Q_SIGNALS:
        void finished();

private:
        Q_DISABLE_COPY(ConsolePrompt)

        class Private;
        friend class Private;
        Private *d;
};

class AbstractLogDevice;

class QCA_EXPORT Logger : public QObject
{
        Q_OBJECT
public:
        enum Severity
        {
                Quiet = 0,       
                Emergency = 1,   
                Alert = 2,       
                Critical = 3,    
                Error = 4,       
                Warning = 5,     
                Notice = 6,      
                Information = 7, 
                Debug = 8        
        };

        inline Severity level() const { return m_logLevel; }

        void setLevel(Severity level);

        void logTextMessage(const QString &message, Severity = Information);

        void logBinaryMessage(const QByteArray &blob, Severity = Information);

        void registerLogDevice(AbstractLogDevice *logger);

        void unregisterLogDevice(const QString &loggerName);

        QStringList currentLogDevices() const;

private:
        Q_DISABLE_COPY(Logger)

        friend class Global;

        Logger();

        ~Logger();

        QStringList m_loggerNames;
        QList<AbstractLogDevice*> m_loggers;
        Severity m_logLevel;
};

class QCA_EXPORT AbstractLogDevice : public QObject
{
        Q_OBJECT
public:
        QString name() const;

        virtual void logTextMessage(const QString &message, Logger::Severity severity);

        virtual void logBinaryMessage(const QByteArray &blob, Logger::Severity severity);

protected:
        explicit AbstractLogDevice(const QString &name, QObject *parent = 0);

        virtual ~AbstractLogDevice() = 0;

private:
        Q_DISABLE_COPY(AbstractLogDevice)

        class Private;
        Private *d;

        QString m_name;
};

}

#endif