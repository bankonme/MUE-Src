// Copyright (c) 2009-2015 Bitcoin developers
// Copyright (c) 2014-2015 MonetaryUnit developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef RECENTREQUESTSTABLEMODEL_H
#define RECENTREQUESTSTABLEMODEL_H

#include "walletmodel.h"

#include <QAbstractTableModel>
#include <QStringList>
#include <QDateTime>

class CWallet;

class RecentRequestEntry
{
public:
    RecentRequestEntry() : nVersion(RecentRequestEntry::CURRENT_VERSION), id(0) { }

    static const int CURRENT_VERSION = 1;
    int nVersion;
    int64_t id;
    QDateTime date;
    SendCoinsRecipient recipient;

    IMPLEMENT_SERIALIZE
    (
        RecentRequestEntry* pthis = const_cast<RecentRequestEntry*>(this);

        unsigned int nDate = date.toTime_t();

        READWRITE(pthis->nVersion);
        nVersion = pthis->nVersion;
        READWRITE(id);
        READWRITE(nDate);
        READWRITE(recipient);

        if (fRead)
            pthis->date = QDateTime::fromTime_t(nDate);
    )
};

class RecentRequestEntryLessThan
{
public:
    RecentRequestEntryLessThan(int nColumn, Qt::SortOrder fOrder):
        column(nColumn), order(fOrder) {}
    bool operator()(RecentRequestEntry &left, RecentRequestEntry &right) const;

private:
    int column;
    Qt::SortOrder order;
};

/** Model for list of recently generated payment requests / monetaryunit: URIs.
 * Part of wallet model.
 */
class RecentRequestsTableModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RecentRequestsTableModel(CWallet *wallet, WalletModel *parent);
    ~RecentRequestsTableModel();

    enum ColumnIndex {
        Date = 0,
        Label = 1,
        Message = 2,
        Amount = 3,
        NUMBER_OF_COLUMNS
    };

    /** @name Methods overridden from QAbstractTableModel
        @{*/
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /*@}*/

    const RecentRequestEntry &entry(int row) const { return list[row]; }
    void addNewRequest(const SendCoinsRecipient &recipient);
    void addNewRequest(const std::string &recipient);
    void addNewRequest(RecentRequestEntry &recipient);

public slots:
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

private:
    WalletModel *walletModel;
    QStringList columns;
    QList<RecentRequestEntry> list;
    int64_t nReceiveRequestsMaxId;
};

#endif
