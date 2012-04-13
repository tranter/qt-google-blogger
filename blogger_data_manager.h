#ifndef BLOGGER_DATA_MANAGER_H
#define BLOGGER_DATA_MANAGER_H

#include <QObject>
#include <QVariantList>
//#include <QStringList>

#include <QNetworkReply>
#include <QNetworkAccessManager>

class blogger_data_manager : public QObject
{
    Q_OBJECT

public:
    blogger_data_manager();

    //Requests to Google API service.
    void getBlogsList(const QString& access_token);
    void getBlogPosts(const QString& access_token);
    void getPostComments(const QString& access_token, qlonglong idPost);
    void getBlogPages(const QString& access_token);
    void getBlogUsers(const QString& access_token);

    QVariantList& getBlogs() {return m_blogs;}
    QVariantList& getPosts() {return m_posts;}
    QVariantList& getPages() {return m_pages;}
    QVariantList& getComments() {return m_comments;}
    QVariantList& getUsers() {return m_users;}

    void setIdBlog(qlonglong id) {m_idBlog = id;}

signals:
    void sigErrorOccured(const QString& error);
    void sigBlogsListReady();
    void sigPagesListReady();
    void sigPostsListReady();
    void sigCommentsListReady();
    void sigUsersListReady();

private slots:
    void replyFinished(QNetworkReply*);

private:
    QVariantList m_blogs;
    QVariantList m_posts;
    QVariantList m_pages;
    QVariantList m_comments;
    QVariantList m_users;

    qlonglong m_idBlog;
    QNetworkAccessManager* m_pNetworkAccessManager;
};

#endif // BLOGGER_DATA_MANAGER_H
