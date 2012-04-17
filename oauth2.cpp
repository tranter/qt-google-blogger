#include "oauth2.h"
#include <QApplication>
#include "logindialog.h"
#include <QSettings>
#include <QMessageBox>

OAuth2::OAuth2(QWidget* parent)
{
    m_strEndPoint = "https://accounts.google.com/o/oauth2/auth";
    m_strScope = "https://www.googleapis.com/auth/blogger";
    m_strClientID = "YOUR_CLIENT_ID_HERE";
    m_strRedirectURI = "YOUR_REDIRECT_URI_HERE";
    m_strResponseType = "token";

    m_strCompanyName = "ICS"; //You company here
    m_strAppName = "QtBlogger"; //Your application name here

    m_pLoginDialog = NULL;
    m_pParent = parent;
}

void OAuth2::setScope(const QString& scope)
{
    m_strScope = scope;
}

void OAuth2::setClientID(const QString& clientID)
{
    m_strClientID = clientID;
}

void OAuth2::setRedirectURI(const QString& redirectURI)
{
    m_strRedirectURI = redirectURI;
}

void OAuth2::setCompanyName(const QString& companyName)
{
    m_strCompanyName = companyName;
}

void OAuth2::setAppName(const QString& appName)
{
    m_strAppName = appName;
}

QString OAuth2::loginUrl()
{
    QString str = QString("%1?client_id=%2&redirect_uri=%3&response_type=%4&scope=%5").arg(m_strEndPoint).arg(m_strClientID).
            arg(m_strRedirectURI).arg(m_strResponseType).arg(m_strScope);
    return str;
}

QString OAuth2::accessToken()
{
    return m_strAccessToken;
}

bool OAuth2::isAuthorized()
{
    return m_strAccessToken != "";
}

void OAuth2::startLogin(bool bForce)
{
    QSettings settings(m_strCompanyName, m_strAppName);
    QString str = settings.value("access_token", "").toString();

    if(m_strClientID == "YOUR_CLIENT_ID_HERE" || m_strRedirectURI == "YOUR_REDIRECT_URI_HERE")
    {
        QMessageBox::warning(m_pParent, "Warning",
                             "To work with application you need to register your own application in <b>Google</b>.\n"
                             "Learn more from <a href='http://code.google.com/p/qt-google-blogger/wiki/HowToRegisterYourApplicationInGoogle'>here</a>");
        return;
    }

    if(str.isEmpty() || bForce)
    {
        if (m_pLoginDialog != NULL) {
            delete m_pLoginDialog;
        }
        m_pLoginDialog = new LoginDialog(m_pParent);
        connect(m_pLoginDialog, SIGNAL(accessTokenObtained()), this, SLOT(accessTokenObtained()));
        m_pLoginDialog->setLoginUrl(loginUrl());
        m_pLoginDialog->show();
    }
    else
    {
        m_strAccessToken = str;
        emit loginDone();
    }
}

void OAuth2::accessTokenObtained()
{
    QSettings settings(m_strCompanyName, m_strAppName);
    m_strAccessToken = m_pLoginDialog->accessToken();
    settings.setValue("access_token", m_strAccessToken);
    emit loginDone();
    if (m_pLoginDialog != NULL) {
        m_pLoginDialog->deleteLater();
        m_pLoginDialog = NULL;
    }
}
