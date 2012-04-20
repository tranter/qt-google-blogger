#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "oauth2.h"
#include "blogger_data_manager.h"

//class blogger_data_manager;
class QSettings;

namespace Ui {
    class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();


    void startLogin(bool bForce); //Show login dialog even if there is gotten already access_token.
    void refreshBlogs();

public slots:
    void goBack();
    void goForward();

private slots:
    void onLoginDone();
    void onErrorOccured(const QString& error);
    void onUserEmailReady();
    void onBlogsListReady();
    void onPostsListReady();
    void onCommentsListReady();
    void onPagesListReady();

    void showBlogPosts(int pos);
    void showPostContent(int pos);
    void showPageContent(int pos);

private:
    Ui::Form *ui;
    OAuth2* m_pOAuth2;
    blogger_data_manager m_manager;
    QSettings* m_pSettings;
    QString m_strCompanyName;
    QString m_strAppName;

    QDateTime convertToTime(const QString& str);

    void saveSettings();
};

#endif // FORM_H
