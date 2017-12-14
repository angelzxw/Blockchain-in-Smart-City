/*
 * qca_core.h - Qt Cryptographic Architecture
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

#ifndef QCA_CORE_H
#define QCA_CORE_H

#define QCA_VERSION 0x020003

#include <QString>
#include <QStringList>
#include <QList>
#include <QSharedData>
#include <QSharedDataPointer>
#include "qca_export.h"
#include "qca_support.h"
#include "qca_tools.h"

QCA_EXPORT int qcaVersion();

namespace QCA {

class Provider;
class Random;
class CertificateCollection;
class Global;
class KeyStore;
class KeyStoreEntry;
class KeyStoreInfo;
class KeyStoreManager;
class Logger;

typedef QList<Provider*> ProviderList;

enum MemoryMode
{
        Practical, 
        Locking, 
        LockingKeepPrivileges 
};

enum Direction
{
        Encode, 
        Decode  
};

QCA_EXPORT void init();

QCA_EXPORT void init(MemoryMode m, int prealloc);

QCA_EXPORT void deinit();

QCA_EXPORT bool haveSecureMemory();

QCA_EXPORT bool haveSecureRandom();

QCA_EXPORT bool isSupported(const char *features, const QString &provider = QString());

QCA_EXPORT bool isSupported(const QStringList &features, const QString &provider = QString());

QCA_EXPORT QStringList supportedFeatures();

QCA_EXPORT QStringList defaultFeatures();

QCA_EXPORT bool insertProvider(Provider *p, int priority = 0);

QCA_EXPORT void setProviderPriority(const QString &name, int priority);

QCA_EXPORT int providerPriority(const QString &name);

QCA_EXPORT ProviderList providers();

QCA_EXPORT Provider *findProvider(const QString &name);

QCA_EXPORT Provider *defaultProvider();

QCA_EXPORT void scanForPlugins();

QCA_EXPORT void unloadAllPlugins();

QCA_EXPORT QString pluginDiagnosticText();

QCA_EXPORT void clearPluginDiagnosticText();

QCA_EXPORT void appendPluginDiagnosticText(const QString &text);

QCA_EXPORT void setProperty(const QString &name, const QVariant &value);

QCA_EXPORT QVariant getProperty(const QString &name);

QCA_EXPORT void setProviderConfig(const QString &name, const QVariantMap &config);

QCA_EXPORT QVariantMap getProviderConfig(const QString &name);

QCA_EXPORT void saveProviderConfig(const QString &name);

QCA_EXPORT QString globalRandomProvider();

QCA_EXPORT void setGlobalRandomProvider(const QString &provider);

QCA_EXPORT Logger *logger();

#define QCA_logTextMessage(message, severity) \
        do { \
                register QCA::Logger::Severity s = severity; \
                register QCA::Logger *l = QCA::logger (); \
                if (s <= l->level ()) { \
                        l->logTextMessage (message, s); \
                } \
        } while (false)

#define QCA_logBinaryMessage(blob, severity) \
        do { \
                register QCA::Logger::Severity s = severity; \
                register QCA::Logger *l = QCA::logger (); \
                if (s <= l->level ()) { \
                        l->logBinaryMessage (blob, s); \
                } \
        } while (false)

QCA_EXPORT bool haveSystemStore();

QCA_EXPORT CertificateCollection systemStore();

QCA_EXPORT QString appName();

QCA_EXPORT void setAppName(const QString &name);

QCA_EXPORT QString arrayToHex(const QByteArray &array);

QCA_EXPORT QByteArray hexToArray(const QString &hexString);

class QCA_EXPORT Initializer
{
public:
        explicit Initializer(MemoryMode m = Practical, int prealloc = 64);
        ~Initializer();
};

class QCA_EXPORT KeyLength
{
public:
        KeyLength(int min, int max, int multiple)
                : _min( min ), _max(max), _multiple( multiple )
        { }

        int minimum() const { return _min; }

        int maximum() const { return _max; }

        int multiple() const { return _multiple; }

private:
        const int _min, _max, _multiple;
};

class QCA_EXPORT Provider
{
public:
        virtual ~Provider();

        class Context;

        virtual void init();

        virtual void deinit();

        virtual int version() const;

        virtual int qcaVersion() const = 0;

        virtual QString name() const = 0;

        virtual QStringList features() const = 0;

        virtual QString credit() const;

        virtual Context *createContext(const QString &type) = 0;

        virtual QVariantMap defaultConfig() const;

        virtual void configChanged(const QVariantMap &config);
};

class QCA_EXPORT Provider::Context : public QObject
{
        Q_OBJECT
public:
        virtual ~Context();

        Provider *provider() const;

        QString type() const;

        virtual Context *clone() const = 0;

        bool sameProvider(const Context *c) const;

protected:
        Context(Provider *parent, const QString &type);

        Context(const Context &from);

private:
        // disable assignment
        Context & operator=(const Context &from);

        Provider *_provider;
        QString _type;
};

class QCA_EXPORT BasicContext : public Provider::Context
{
        Q_OBJECT
public:
        ~BasicContext();

protected:
        BasicContext(Provider *parent, const QString &type);

        BasicContext(const BasicContext &from);

private:
        // disable assignment
        BasicContext & operator=(const BasicContext &from);
};

class QCA_EXPORT BufferedComputation
{
public:
        virtual ~BufferedComputation();

        virtual void clear() = 0;

        virtual void update(const MemoryRegion &a) = 0;

        virtual MemoryRegion final() = 0;

        MemoryRegion process(const MemoryRegion &a);
};

class QCA_EXPORT Filter
{
public:
        virtual ~Filter();

        virtual void clear() = 0;

        virtual MemoryRegion update(const MemoryRegion &a) = 0;

        virtual MemoryRegion final() = 0;

        virtual bool ok() const = 0;

        MemoryRegion process(const MemoryRegion &a);
};

class QCA_EXPORT Algorithm
{
public:
        Algorithm(const Algorithm &from);

        virtual ~Algorithm();

        Algorithm & operator=(const Algorithm &from);

        QString type() const;

        Provider *provider() const;

        // Note: The next five functions are not public!

        Provider::Context *context();

        const Provider::Context *context() const;

        void change(Provider::Context *c);

        void change(const QString &type, const QString &provider);

        Provider::Context *takeContext();

protected:
        Algorithm();

        Algorithm(const QString &type, const QString &provider);

private:
        class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT SymmetricKey : public SecureArray
{
public:
        SymmetricKey();

        SymmetricKey(int size);

        SymmetricKey(const SecureArray &a);

        SymmetricKey(const QByteArray &a);

        bool isWeakDESKey();
};

class QCA_EXPORT InitializationVector : public SecureArray
{
public:
        InitializationVector();

        InitializationVector(int size);

        InitializationVector(const SecureArray &a);

        InitializationVector(const QByteArray &a);
};

class QCA_EXPORT Event
{
public:
        enum Type
        {
                Password,   
                Token       
        };

        enum Source
        {
                KeyStore,   
                Data        
        };

        enum PasswordStyle
        {
                StylePassword,   
                StylePassphrase, 
                StylePIN         
        };

        Event();

        Event(const Event &from);

        ~Event();

        Event & operator=(const Event &from);

        bool isNull() const;

        Type type() const;

        Source source() const;

        PasswordStyle passwordStyle() const;

        KeyStoreInfo keyStoreInfo() const;

        KeyStoreEntry keyStoreEntry() const;

        QString fileName() const;

        void *ptr() const;

        void setPasswordKeyStore(PasswordStyle pstyle, const KeyStoreInfo &keyStoreInfo, const KeyStoreEntry &keyStoreEntry, void *ptr);

        void setPasswordData(PasswordStyle pstyle, const QString &fileName, void *ptr);

        void setToken(const KeyStoreInfo &keyStoreInfo, const KeyStoreEntry &keyStoreEntry, void *ptr);

private:
        class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT EventHandler : public QObject
{
        Q_OBJECT
public:
        EventHandler(QObject *parent = 0);
        ~EventHandler();

        void start();

        void submitPassword(int id, const SecureArray &password);

        void tokenOkay(int id);

        void reject(int id);

Q_SIGNALS:
        void eventReady(int id, const QCA::Event &context);

private:
        Q_DISABLE_COPY(EventHandler)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT PasswordAsker : public QObject
{
        Q_OBJECT
public:
        PasswordAsker(QObject *parent = 0);
        ~PasswordAsker();

        void ask(Event::PasswordStyle pstyle, const KeyStoreInfo &keyStoreInfo, const KeyStoreEntry &keyStoreEntry, void *ptr);

        void ask(Event::PasswordStyle pstyle, const QString &fileName, void *ptr);

        void cancel();

        void waitForResponse();

        bool accepted() const;

        SecureArray password() const;

Q_SIGNALS:
        void responseReady();

private:
        Q_DISABLE_COPY(PasswordAsker)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT TokenAsker : public QObject
{
        Q_OBJECT
public:
        TokenAsker(QObject *parent = 0);
        ~TokenAsker();

        void ask(const KeyStoreInfo &keyStoreInfo, const KeyStoreEntry &keyStoreEntry, void *ptr);

        void cancel();

        void waitForResponse();

        bool accepted() const;

Q_SIGNALS:
        void responseReady();

private:
        Q_DISABLE_COPY(TokenAsker)

        class Private;
        friend class Private;
        Private *d;
};

}

#endif