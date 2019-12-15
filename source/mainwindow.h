#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QCryptographicHash>
#include <QMouseEvent>
#include <QNetworkCookieJar>
#include <QRegularExpression>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_released();
    void on_pushButton_2_released();

    void on_passInput_returnPressed() {
        on_pushButton_2_released();
    }

    void on_userInput_returnPressed() {
        on_pushButton_2_released();
    }

    void on_minimizeButton_released() {
        showMinimized();
        startX = startY = -1;
    }

    void on_exitButton_released() {
        close();
    }

    void fetchNewsFinish(QNetworkReply* rep);
    void getLauncherTokenFinish(QNetworkReply* rep);
    void getTokenFinish(QNetworkReply* rep);
    void signInFinish(QNetworkReply* rep);
    void accountInfoFinish(QNetworkReply* rep);

    void on_progressBar_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    bool authenticating = false;
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*) {
        startX = startY = -1;
    }
    void updateSuccessful();
    int startX = -1;
    int startY = -1;

    QString token;
    QNetworkAccessManager* mgr;
    QNetworkCookieJar* cookieJar;

    friend class game;
};

#endif // MAINWINDOW_H
