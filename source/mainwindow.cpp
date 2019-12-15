#include "mainwindow.h"
#include "ui_mainwindow.h"
#define WIN32_LEAN_AND_MEAN
#include <SDKDDKVer.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <thread>
#include <sstream>
#include "game.h"
#include "slswindow.h"
#pragma comment(lib, "user32.lib")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setVisible(false);
    mgr = new QNetworkAccessManager(this);
    cookieJar = new QNetworkCookieJar(mgr);
    mgr->setCookieJar(cookieJar);

    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(fetchNewsFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    mgr->get(QNetworkRequest(QUrl("http://tera.enmasse.com/news/categories/windows")));

    HINSTANCE hInstance = GetModuleHandle(NULL);
    registerClass(hInstance);

    HWND hWnd = CreateWindowW(szWindowClass, L"TERA Launcher", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if(hWnd) {
        HACCEL hAccelTable = LoadAccelerators(hInstance, L"EME.LauncherWnd");
        std::thread mThread(messageThread, hAccelTable);
        mThread.detach();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// News
void MainWindow::fetchNewsFinish(QNetworkReply* rep) {
    mgr->disconnect();
    updateSuccessful();
    QString read = rep->readAll();
    ui->newsWindow->setHtml("<html><head></head><body>");
    QRegularExpressionMatchIterator matches = QRegularExpression("<article class=\"blog_post\" id=\"(.+?)\">([A-z0-9\\<\\>\\/\\\"\\=\\-\\!\\,\\.\\;\\'\\:\\_\\(\\)\\–\\s]+?)<\\/article>").globalMatch(read);
    while(matches.hasNext()) {
        QString matched = matches.next().captured(2);
        QRegularExpressionMatch linkMatch = QRegularExpression("<a href=\"(.+?)\">(.+?)</a>").match(matched);
        QRegularExpressionMatch postedAt = QRegularExpression("<time datetime=\"(.+?)\" class=\"posted_at\">(.+?)</time>").match(matched);
        QRegularExpressionMatch postedBy = QRegularExpression("by (.*)").match(matched);
        QRegularExpressionMatch content = QRegularExpression("<div class=\"content\">([A-z0-9\\<\\>\\/\\\"\\=\\-\\!\\,\\.\\;\\'\\:\\_\\(\\)\\–\\s]+?)</div>").match(matched);
        ui->newsWindow->insertHtml("<b><font style=\"font-size: 18px;\"><a href=\"http://tera.enmasse.com" + linkMatch.captured(1) + "\"><font color=\"#FFFFFF\">" + linkMatch.captured(2) + "</font></a></b><br><font color=\"#CCCCCC\" style=\"font-size: 0.8em;\">" + postedAt.captured(2) + " by " + postedBy.captured(1) + "</font></font><br><br><font style=\"color: #CCCCCC; font-size: 12px;\">" + content.captured(1) + "</font>" + (matches.hasNext() ? "<br><hr><br>" : ""));
    }
    ui->newsWindow->insertHtml("</body></html>");
}

/* -- LAUNCHER STUFF -- */

void MainWindow::updateSuccessful() {
    ui->progressBar->setValue(ui->progressBar->maximum());
    ui->progressPercent->setText("Please log in");
    ui->pushButton_2->setEnabled(true);
}

void MainWindow::on_pushButton_released() {
    launchGame();
}

void MainWindow::on_pushButton_2_released()
{
    if(!authenticating) {
        authenticating = true;
        connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(getTokenFinish(QNetworkReply*)));
        connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
        mgr->get(QNetworkRequest(QUrl("https://account.enmasse.com")));
    }
}

// Get launcher token.
void MainWindow::getLauncherTokenFinish(QNetworkReply* rep) {
    token = QRegularExpression("meta content=\"(.+?)\" name=\"csrf-token").match(QString(rep->readAll())).captured(1);

    QNetworkRequest request(QUrl("https://account.enmasse.com/launcher/1/account_server_info?attach_auth_ticket=1"));

    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("Accept-Language", "en-us,en");
    request.setRawHeader("Accept-Charset", "iso-8859-1,*,utf-8");
    request.setRawHeader("Host", "account.enmasse.com");
    request.setRawHeader("Origin", "https://account.enmasse.com");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1046.0 Safari/535.21");
    request.setRawHeader("Referer", "https://account.enmasse.com/launcher/1");
    request.setRawHeader("X-CSRF-Token", token.toLocal8Bit());
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");

    mgr->disconnect();

    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(accountInfoFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));

    mgr->get(request);
}


// Get account information
void MainWindow::accountInfoFinish(QNetworkReply* rep) {
    QByteArray bts = rep->readAll();
    if(QString(bts) != "{\"result-message\":\"Game account not found\",\"result-code\":404}") {
        ui->userInput->setVisible(false);
        ui->passInput->setVisible(false);
        ui->pushButton_2->setVisible(false);
        ui->pushButton->setEnabled(true);
        ui->pushButton->setVisible(true);
        gameString = QString(bts).toStdString();
        ui->progressPercent->setText("Ready to launch");
    } else {
        ui->progressPercent->setText("Invalid credentials");
    }

    authenticating = false;
}

void MainWindow::signInFinish(QNetworkReply*) {
    mgr->disconnect();
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(getLauncherTokenFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    mgr->get(QNetworkRequest(QUrl("https://account.enmasse.com/launcher/1")));
}

// Get authenticity_token and sign in using it.
void MainWindow::getTokenFinish(QNetworkReply* rep) {
    token = QRegularExpression("meta content=\"(.+?)\" name=\"csrf-token").match(QString(rep->readAll())).captured(1);

    QNetworkRequest request(QUrl("https://account.enmasse.com/launcher/1/authenticate"));

    request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("Accept-Language", "en-us,en");
    request.setRawHeader("Accept-Charset", "iso-8859-1,*,utf-8");
    request.setRawHeader("Host", "account.enmasse.com");
    request.setRawHeader("Origin", "https://account.enmasse.com");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/535.21 (KHTML, like Gecko) Chrome/19.0.1046.0 Safari/535.21");
    request.setRawHeader("Referer", "https://account.enmasse.com/");

    mgr->disconnect();

    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(signInFinish(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));

    QByteArray postData = QString("authenticity_token=" + token + "&user[client_time]=&user[io_black_box]=.&user[email]=" + ui->userInput->text().trimmed() + "&user[password]=" + ui->passInput->text().trimmed()).toLocal8Bit();
    mgr->post(request, postData);
}

/* -- UI STUFF -- */

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if(event->y() <= ui->minimizeButton->pos().y() + ui->minimizeButton->size().height() && event->x() < ui->minimizeButton->pos().x()) {
        startX = event->x();
        startY = event->y();
    } else {
        startX = startY = -1;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent*) {
    if(startX != -1 && startY != -1) {
        move(cursor().pos().x() - startX, cursor().pos().y() - startY);
    }
}

void MainWindow::on_progressBar_valueChanged(int value)
{
    ui->progressPercent->setText(QString::number(((double)value / (double)ui->progressBar->maximum()) * 100) + "%");
}
