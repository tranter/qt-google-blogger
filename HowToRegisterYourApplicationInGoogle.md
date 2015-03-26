# Introduction #

The qt-google-blogger example uses Google Blogger API. To use API you need to register your own application on Google. Do not worry: the procedure is very simple.


# Details #

You need to login to Google, so first you need to create simple Google account. Then you can visit the page

https://code.google.com/apis/console

there you can create your application. You need to check access to Blogger API.

Right now you need to register your application as a web server application.

**IMPORTANT!** For Redirect\_URI you should select any valid web server with any address on them.

Then  you can see credentials of your application. You need to copy and paste **Client\_ID** and **Redirect\_URI** and **Client\_secret** to the file **oauth2.cpp**.
```
OAuth2::OAuth2(QWidget* parent)
{
    m_strEndPoint = "https://accounts.google.com/o/oauth2/auth";
    m_strScope = "https://www.googleapis.com/auth/blogger";

    m_strClientID = "YOUR_CLIENT_ID_HERE";
    m_strClientSecret = "YOUR_CLIENT_SECRET_HERE";
    m_strRedirectURI = "YOUR_REDIRECT_URI_HERE";

    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    m_pLoginDialog = NULL;
    m_pParent = parent;
    m_pSettings = NULL;
}
```

Additionally you can specify your company name in the file **form.cpp**.
```
Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    m_pOAuth2 = new OAuth2(this);
    m_strCompanyName = "YOU_COMPANY_NAME_HERE";
    m_strAppName = "QtBlogger";

    // Load settings
    m_pSettings = new QSettings(m_strCompanyName,m_strAppName);
    m_pOAuth2->setAccessToken(m_pSettings->value("access_token").toString());
    m_pOAuth2->setRefreshToken(m_pSettings->value("refresh_token").toString());
    m_pOAuth2->setSettings(m_pSettings);
```

After that you can compile and run **qt-google-blogger**.