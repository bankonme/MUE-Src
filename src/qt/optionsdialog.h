// Copyright (c) 2009-2015 Bitcoin Developers
// Copyright (c) 2014-2015 MonetaryUnit Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class MonitoredDataMapper;
class OptionsModel;
class QValidatedLineEdit;

namespace Ui {
class OptionsDialog;
}

/** Preferences dialog. */
class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent);
    ~OptionsDialog();

    void setModel(OptionsModel *model);
    void setMapper();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    /* enable OK button */
    void enableOkButton();
    /* disable OK button */
    void disableOkButton();
    /* set OK button state (enabled / disabled) */
    void setOkButtonState(bool fState);
    void on_resetButton_clicked();
    void on_okButton_clicked();
    void on_cancelButton_clicked();

    void showRestartWarning(bool fPersistent = false);
    void clearStatusLabel();
    void updateDisplayUnit();
    void doProxyIpChecks(QValidatedLineEdit *pUiProxyIp, int nProxyPort);

signals:
    void proxyIpChecks(QValidatedLineEdit *pUiProxyIp, int nProxyPort);

private:
    Ui::OptionsDialog *ui;
    OptionsModel *model;
    MonitoredDataMapper *mapper;
    bool fProxyIpValid;
};

#endif // OPTIONSDIALOG_H
