#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(startLogin()));



    m_pForm = new Form(this);
    setCentralWidget(m_pForm);

    connect(ui->actionBack, SIGNAL(triggered()), m_pForm, SLOT(goBack()));
    connect(ui->actionForward, SIGNAL(triggered()), m_pForm, SLOT(goForward()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startLogin()
{
    m_pForm->startLogin(true);
}

void MainWindow::startLoginDefault()
{
    m_pForm->startLogin(false);
}

void MainWindow::on_actionRefresh_triggered()
{
    m_pForm->refreshBlogs();
}
