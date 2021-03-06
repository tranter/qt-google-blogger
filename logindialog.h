#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QString>
#include <QUrl>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();
    void setLoginUrl(const QString& url);
    QString accessToken() {return m_strAccessToken;}
    QString code() {return m_strCode;}

signals:
    void accessTokenObtained();
    void codeObtained();

private slots:
    void urlChanged(const QUrl& url);
    void loadStarted();
    void loadFinished(bool);


private:
    Ui::LoginDialog *ui;
    QString m_strAccessToken;
    QString m_strCode;
};

#endif // LOGINDIALOG_H
