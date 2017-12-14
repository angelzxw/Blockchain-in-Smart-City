/*
 * qca_keystore.h - Qt Cryptographic Architecture
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

#ifndef QCA_KEYSTORE_H
#define QCA_KEYSTORE_H

#include "qca_core.h"
#include "qca_cert.h"

namespace QCA {

class KeyStoreTracker;
class KeyStoreManagerPrivate;
class KeyStorePrivate;

class QCA_EXPORT KeyStoreEntry : public Algorithm
{
public:
        enum Type
        {
                TypeKeyBundle,
                TypeCertificate,
                TypeCRL,
                TypePGPSecretKey,
                TypePGPPublicKey
        };

        KeyStoreEntry();

        KeyStoreEntry(const QString &serialized);

        KeyStoreEntry(const KeyStoreEntry &from);

        ~KeyStoreEntry();

        KeyStoreEntry & operator=(const KeyStoreEntry &from);

        bool isNull() const;

        bool isAvailable() const;

        bool isAccessible() const;

        Type type() const;

        QString name() const;

        QString id() const;

        QString storeName() const;

        QString storeId() const;

        QString toString() const;

        static KeyStoreEntry fromString(const QString &serialized);

        KeyBundle keyBundle() const;

        Certificate certificate() const;

        CRL crl() const;

        PGPKey pgpSecretKey() const;

        PGPKey pgpPublicKey() const;

        bool ensureAvailable();

        bool ensureAccess();

private:
        class Private;
        Private *d;

        friend class KeyStoreTracker;
};

class QCA_EXPORT KeyStoreEntryWatcher : public QObject
{
        Q_OBJECT
public:
        explicit KeyStoreEntryWatcher(const KeyStoreEntry &e, QObject *parent = 0);

        ~KeyStoreEntryWatcher();

        KeyStoreEntry entry() const;

Q_SIGNALS:
        void available();

        void unavailable();

private:
        Q_DISABLE_COPY(KeyStoreEntryWatcher)

        class Private;
        friend class Private;
        Private *d;
};

class QCA_EXPORT KeyStore : public QObject, public Algorithm
{
        Q_OBJECT
public:
        enum Type
        {
                System,      
                User,        
                Application, 
                SmartCard,   
                PGPKeyring   
        };

        KeyStore(const QString &id, KeyStoreManager *keyStoreManager);

        ~KeyStore();

        bool isValid() const;

        Type type() const;

        QString name() const;

        QString id() const;

        bool isReadOnly() const;

        void startAsynchronousMode();

        QList<KeyStoreEntry> entryList() const;

        bool holdsTrustedCertificates() const;

        bool holdsIdentities() const;

        bool holdsPGPPublicKeys() const;

        QString writeEntry(const KeyBundle &kb);

        QString writeEntry(const Certificate &cert);

        QString writeEntry(const CRL &crl);

        QString writeEntry(const PGPKey &key);

        bool removeEntry(const QString &id);

Q_SIGNALS:
        void updated();

        void unavailable();

        void entryWritten(const QString &entryId);

        void entryRemoved(bool success);

private:
        Q_DISABLE_COPY(KeyStore)

        friend class KeyStorePrivate;
        KeyStorePrivate *d;

        friend class KeyStoreManagerPrivate;
};

class QCA_EXPORT KeyStoreInfo
{
public:
        KeyStoreInfo();

        KeyStoreInfo(KeyStore::Type type, const QString &id, const QString &name);

        KeyStoreInfo(const KeyStoreInfo &from);

        ~KeyStoreInfo();

        KeyStoreInfo & operator=(const KeyStoreInfo &from);

        bool isNull() const;

        KeyStore::Type type() const;

        QString id() const;

        QString name() const;

private:
        class Private;
        QSharedDataPointer<Private> d;
};

class QCA_EXPORT KeyStoreManager : public QObject
{
        Q_OBJECT
public:
        KeyStoreManager(QObject *parent = 0);
        ~KeyStoreManager();

        static void start();

        static void start(const QString &provider);

        bool isBusy() const;

        void waitForBusyFinished();

        QStringList keyStores() const;

        static QString diagnosticText();

        static void clearDiagnosticText();

        void sync();

Q_SIGNALS:
        void busyStarted();

        void busyFinished();

        void keyStoreAvailable(const QString &id);

private:
        Q_DISABLE_COPY(KeyStoreManager)

        friend class KeyStoreManagerPrivate;
        KeyStoreManagerPrivate *d;

        friend class Global;
        friend class KeyStorePrivate;

        static void scan();
        static void shutdown();
};

}

#endif