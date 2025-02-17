// Copyright (c) 2009-2015 Bitcoin developers
// Copyright (c) 2014-2015 MonetaryUnit developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef QRCODEDIALOG_H
#define QRCODEDIALOG_H

#include "walletmodel.h"

#include <QDialog>
#include <QImage>
#include <QLabel>

namespace Ui {
    class ReceiveRequestDialog;
}
class OptionsModel;
QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

/* Label widget for QR code. This image can be dragged, dropped, copied and saved
 * to disk.
 */
class QRImageWidget : public QLabel
{
    Q_OBJECT

public:
    explicit QRImageWidget(QWidget *parent = 0);
    QImage exportImage();

public slots:
    void saveImage();
    void copyImage();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);

private:
    QMenu *contextMenu;
};

class ReceiveRequestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiveRequestDialog(QWidget *parent = 0);
    ~ReceiveRequestDialog();

    void setModel(OptionsModel *model);
    void setInfo(const SendCoinsRecipient &info);

private slots:
    void on_btnCopyURI_clicked();
    void on_btnCopyAddress_clicked();

    void update();

private:
    Ui::ReceiveRequestDialog *ui;
    OptionsModel *model;
    SendCoinsRecipient info;
};

#endif // QRCODEDIALOG_H
