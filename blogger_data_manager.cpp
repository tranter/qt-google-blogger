#include <QJson/Parser>

#include "blogger_data_manager.h"

blogger_data_manager::blogger_data_manager()
{
    m_idBlog = -1;
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}
/*! \brief Get blogs list for particular blogger
 *
 *  \param access_token Authorization token
 */
void blogger_data_manager::getBlogsList(const QString& access_token)
{
    QString s = QString("https://www.googleapis.com/blogger/v2/users/self/blogs?access_token=%1").arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(s)));
}

void blogger_data_manager::getBlogPosts(const QString& access_token)
{
    if (m_idBlog < 0) {
        return;
    }
    QString s = QString("https://www.googleapis.com/blogger/v2/blogs/%1/posts?access_token=%2").arg(m_idBlog).arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(s)));
}

void blogger_data_manager::getPostComments(const QString& access_token, qlonglong idPost)
{
    if (m_idBlog < 0) {
        return;
    }
    QString s = QString("https://www.googleapis.com/blogger/v2/blogs/%1/posts/%2/comments?access_token=%3").arg(m_idBlog).arg(idPost).arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(s)));
}

void blogger_data_manager::getBlogPages(const QString& access_token)
{
    if (m_idBlog < 0) {
        return;
    }
    QString s = QString("https://www.googleapis.com/blogger/v2/blogs/%1/pages?access_token=%2").arg(m_idBlog).arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(s)));
}

void blogger_data_manager::getBlogUsers(const QString& access_token)
{
    QString s = QString("https://www.googleapis.com/blogger/v2/users/self?access_token=%1").arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(s)));
}

void blogger_data_manager::getUserEmail(const QString& access_token)
{
    QString query = QString("https://www.googleapis.com/oauth2/v1/userinfo"
                            "?access_token=%1")
            .arg(access_token);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(query)));
}

void blogger_data_manager::replyFinished(QNetworkReply *reply)
{
    QString json = reply->readAll();
    QString url = reply->url().toString();

    if (json.length() == 0) {
        return;
    }

    QJson::Parser parser;

    bool ok;

    // json is a QString containing the data to convert
    QVariant result = parser.parse (json.toLatin1(), &ok);
    if (!ok) {
        emit sigErrorOccured(QString("Cannot convert to QJson object: %1").arg(json));
        return;
    }

    if (result.toMap().contains("error")) {
        if (result.toMap()["error"].toMap()["message"].toString()=="Not Found") {
            emit sigErrorOccured("Empty blogs list: probably you don't create any one.");
        } else {
            emit sigErrorOccured(result.toMap()["error"].toMap()["message"].toString());
        }
        return;
    }

    if (result.toMap()["kind"] == "blogger#blogList") {
        m_blogs = result.toMap()["items"].toList();
        emit sigBlogsListReady();
        return;
    } else if (result.toMap()["kind"] == "blogger#postList") {
        m_posts = result.toMap()["items"].toList();
        emit sigPostsListReady();
        return;
    } else if (result.toMap()["kind"] == "blogger#commentList") {
        m_comments = result.toMap()["items"].toList();
        emit sigCommentsListReady();
        return;
    } else if (result.toMap()["kind"] == "blogger#pageList") {
        m_pages = result.toMap()["items"].toList();
        emit sigPagesListReady();
        return;
    } else if (result.toMap()["kind"] == "blogger#userList") {
        m_users = result.toMap()["items"].toList();
        emit sigUsersListReady();
        return;
    } else if (url.contains("userinfo")) {
        m_strUserEmail = result.toMap()["email"].toString();
        emit sigUserEmailReady();
        return;
    }
}
