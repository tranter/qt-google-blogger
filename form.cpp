#include <QMessageBox>
#include <QDateTime>
#include <QWebHistory>
#include <QSettings>

#include "form.h"
#include "ui_form.h"
#include "blogger_data_manager.h"

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

    connect(m_pOAuth2, SIGNAL(loginDone()), this, SLOT(onLoginDone()));
    connect(m_pOAuth2, SIGNAL(sigErrorOccured(QString)),this,SLOT(onErrorOccured(QString)));
    connect(&m_manager, SIGNAL(sigErrorOccured(QString)),this,SLOT(onErrorOccured(QString)));
    connect(&m_manager, SIGNAL(sigUserEmailReady()),this,SLOT(onUserEmailReady()));
    connect(&m_manager, SIGNAL(sigBlogsListReady()),this,SLOT(onBlogsListReady()));
    connect(&m_manager, SIGNAL(sigPostsListReady()),this,SLOT(onPostsListReady()));
    connect(&m_manager, SIGNAL(sigCommentsListReady()),this,SLOT(onCommentsListReady()));
    connect(&m_manager, SIGNAL(sigPagesListReady()),this,SLOT(onPagesListReady()));

    connect(ui->blogsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(showBlogPosts(int)));
    connect(ui->postsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(showPostContent(int)));
    connect(ui->pagesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(showPageContent(int)));

    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
}

Form::~Form()
{
    saveSettings();
    delete m_pSettings;
    delete ui;
}

void Form::startLogin(bool bForce)
{
    //Now we allow to start logging in when m_oauth2.isAuthorized(). User can log in using another Google account.
    m_pOAuth2->startLogin(bForce); //this is a parent widget for a login dialog.
}
/*! \brief Refresh blogs list
 *
 */
void Form::refreshBlogs()
{
    onLoginDone();
}
void Form::onUserEmailReady()
{
    ui->userEmail->setText(m_manager.userEmail());
    //Now trying to get JSON string for bookshelves of the user.
    //Clear all widgets
    ui->webViewPost->setHtml("");
    ui->webViewComments->setHtml("");
    ui->webViewPage->setHtml("");
    ui->blogsListWidget->clear();
    ui->postsListWidget->clear();
    ui->pagesListWidget->clear();

    m_manager.getBlogsList(m_pOAuth2->accessToken());
    m_manager.getBlogUsers(m_pOAuth2->accessToken());
}

/*! \brief Start function for initialization
 *
 */
void Form::onLoginDone()
{
    m_manager.getUserEmail(m_pOAuth2->accessToken());
}

void Form::onErrorOccured(const QString& error)
{
    if(error.indexOf("Invalid Credentials") != -1)
    {
        startLogin(true);
    }
    else
    {
        QMessageBox::warning(this, tr("Some error on server occured"), error);
    }
}
/*! \brief Handler for signal sigBlogsListReady
 *
 */
void Form::onBlogsListReady()
{
    QVariantList& blogs = m_manager.getBlogs();
    QStringList list;
    for (int i=0; i<blogs.count(); i++) {
        list << blogs[i].toMap()["name"].toString();
    }
    if (list.size() > 0) {
        ui->blogsListWidget->addItems(list);
        ui->blogsListWidget->setCurrentRow(0);
    }
}
/*! \brief Begin show current blog
 *
 *  \param pos Index of current blog in blogsList and index in m_blogsList object too.
 */
void Form::showBlogPosts(int pos)
{
    if (pos < 0) {
        return;
    }
    //Save current blog ID
    m_manager.setIdBlog(m_manager.getBlogs()[pos].toMap()["id"].toLongLong());
    //Refresh posts and pages data
    m_manager.getBlogPosts(m_pOAuth2->accessToken());
    m_manager.getBlogPages(m_pOAuth2->accessToken());
}
/*! \brief List of all posts for current blog is ready
 *
 */
void Form::onPostsListReady()
{
    QVariantList& posts = m_manager.getPosts();
    QStringList list;
    QString sTitle;
    for (int i=0; i<posts.count(); i++) {
        sTitle = posts[i].toMap()["title"].toString();
        if (sTitle.isEmpty()) {
            sTitle = tr("No title");
        }
        list << sTitle;
    }
    ui->webViewPost->setHtml("");
    ui->webViewComments->setHtml("");

    ui->postsListWidget->clear();
    if (list.size() > 0) {
        ui->postsListWidget->addItems(list);
        ui->postsListWidget->setCurrentRow(0);
    }
}
/*! \brief List of all pages for current blog is ready
 *
 */
void Form::onPagesListReady()
{
    QVariantList& pages = m_manager.getPages();
    QStringList list;
    QString sTitle;
    for (int i=0; i<pages.count(); i++) {
        sTitle = pages[i].toMap()["title"].toString();
        if (sTitle.isEmpty()) {
            sTitle = tr("No title");
        }
        list << sTitle;
    }
    ui->webViewPage->setHtml("");

    ui->pagesListWidget->clear();
    if (list.size() > 0) {
        ui->pagesListWidget->addItems(list);
        ui->pagesListWidget->setCurrentRow(0);
    }
}
/*! \brief Show post content for selected row in posts list
 *
 *  \param pos Index of post in lists to show
 *  Show content of selected post that already downloaded and begin download
 *  respective comments list for this post.
 */
void Form::showPostContent(int pos)
{
    if (pos < 0) {
        return;
    }
    QVariantList& posts = m_manager.getPosts();
    QString sContent;

    qlonglong idPost = posts[pos].toMap()["id"].toLongLong();
    sContent = posts[pos].toMap()["content"].toString();
    if (sContent.isEmpty()) {
        sContent = tr("No content");
    }
    ui->webViewPost->setHtml(sContent);
    ui->webViewComments->setHtml("");

    //Begin download comments list for current post
    m_manager.getPostComments(m_pOAuth2->accessToken(),idPost);
}
/*! \brief Comments list is ready. Need to show it.
 *
 *  Collect all comments and show them in webView widget.
 */
void Form::onCommentsListReady()
{
    QVariantList& comments = m_manager.getComments();
    if (comments.size() == 0) {
        return;
    }
    QString sComment;
    sComment = " - <font color=\"blue\">";
    for (int i=0; i<comments.count(); i++) {
        sComment += "Comment from <b>";
        sComment += comments[i].toMap()["author"].toMap()["displayName"].toString();
        QDateTime tUpdated = convertToTime(comments[i].toMap()["updated"].toString());
        sComment += "</b> at " + tUpdated.toString();
        sComment += ": <font color=\"red\">";
        sComment += comments[i].toMap()["content"].toString();
        sComment += ": </font> ";
    }
    sComment += "</font>";

    ui->webViewComments->setHtml(sComment);
}
/*! \brief Show selected blog's page
 *
 *  \param pos Index of selected page
 *  Do not show video references and link's pages of blog.
 *  Reference to URL don't resolve the problem: video and links not shown too.
 */
void Form::showPageContent(int pos)
{
    if (pos < 0) {
        return;
    }
    QVariantList& pages = m_manager.getPages();
    QString sContent;

    sContent = pages[pos].toMap()["content"].toString();
    if (sContent.isEmpty()) {
        sContent = tr("No content");
    }
    ui->webViewPage->setHtml(sContent);
}

QDateTime Form::convertToTime(const QString& str)
{
    QDateTime ret;

    int pos = str.indexOf("T");
    QString sYear = str.mid(0,4);
    QString sMonth = str.mid(5,2);
    QString sDay = str.mid(8,2);

    QString sHour = str.mid(pos+1,2);
    QString sMin = str.mid(pos+4,2);
    QString sSec = str.mid(pos+7,2);

    ret.setDate(QDate(sYear.toInt(),sMonth.toInt(),sDay.toInt()));
    ret.setTime(QTime(sHour.toInt(),sMin.toInt(),sSec.toInt()));

    return ret;
}

void Form::goBack()
{
    if(ui->tabWidget->currentIndex() == 0)
    {
        QWebHistory* history = ui->webViewPost->history();
        if(history->canGoBack())
        {
            if(history->backItem().url().toString() == "about:blank")
            {
                int index = ui->postsListWidget->currentRow();
                if(index != -1)
                {
                    showPostContent(index);
                }
            }
            else
            {
                ui->webViewPost->back();
            }
        }
    }
    else if(ui->tabWidget->currentIndex() == 1)
    {
        QWebHistory* history = ui->webViewComments->history();
        if(history->canGoBack())
        {
            if(history->backItem().url().toString() == "about:blank")
            {
                onCommentsListReady();
            }
            else
            {
                ui->webViewComments->back();
            }
        }
    }
    else if(ui->tabWidget->currentIndex() == 2)
    {
        QWebHistory* history = ui->webViewPage->history();
        if(history->canGoBack())
        {
            if(history->backItem().url().toString() == "about:blank")
            {
                int index = ui->pagesListWidget->currentRow();
                if(index != -1)
                {
                    showPageContent(index);
                }
            }
            else
            {
                ui->webViewPage->back();
            }
        }
    }
}

void Form::goForward()
{
    if(ui->tabWidget->currentIndex() == 0)
    {
        ui->webViewPost->forward();
    }
    else if(ui->tabWidget->currentIndex() == 1)
    {
        ui->webViewComments->forward();
    }
    else if(ui->tabWidget->currentIndex() == 2)
    {
        ui->webViewPage->forward();
    }
}

void Form::saveSettings()
{
    m_pSettings->setValue("access_token",m_pOAuth2->accessToken());
    m_pSettings->setValue("refresh_token",m_pOAuth2->getRefreshToken());
}

