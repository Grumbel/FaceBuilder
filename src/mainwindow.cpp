// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "mainwindow.h"
#include "face.h"
#include "facescene.h"
#include "partbrowser.h"

#include <QAction>
#include <QApplication>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QWidget>

MainWindow::MainWindow(const QString &dataRoot, QWidget *parent)
    : QMainWindow(parent)
    , m_dataRoot(dataRoot)
{
    setWindowTitle(tr("FaceBuilder"));
    resize(900, 650);

    m_face = new Face(this);

    createMenus();
    createToolBar();
    createCentralWidget();
    createStatusBar();
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *newAct = fileMenu->addAction(tr("&New"), this, &MainWindow::onNew);
    newAct->setShortcut(QKeySequence::New);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Open..."))->setEnabled(false);
    fileMenu->addAction(tr("&Save"))->setEnabled(false);
    fileMenu->addAction(tr("Save &As..."))->setEnabled(false);
    fileMenu->addSeparator();
    QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &MainWindow::onQuit);
    quitAct->setShortcut(QKeySequence::Quit);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Undo"))->setEnabled(false);
    editMenu->addAction(tr("&Redo"))->setEnabled(false);
    editMenu->addSeparator();
    editMenu->addAction(tr("&Copy"))->setEnabled(false);
    editMenu->addAction(tr("&Paste"))->setEnabled(false);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("Reset Zoom"))->setEnabled(false);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About FaceBuilder"), this, &MainWindow::onAbout);
}

void MainWindow::createToolBar()
{
    QToolBar *tb = addToolBar(tr("Main"));
    tb->setMovable(false);
    tb->setIconSize(QSize(24, 24));

    auto addPlaceholder = [tb](const QString &tip) {
        QAction *a = tb->addAction(tip);
        a->setEnabled(false);
        a->setToolTip(tip);
        return a;
    };

    addPlaceholder(tr("New"));
    addPlaceholder(tr("Open"));
    addPlaceholder(tr("Save"));
    addPlaceholder(tr("Save As"));
    tb->addSeparator();
    addPlaceholder(tr("Undo"));
    addPlaceholder(tr("Redo"));
    tb->addSeparator();
    addPlaceholder(tr("Copy"));
    addPlaceholder(tr("Paste"));
    tb->addSeparator();
    addPlaceholder(tr("Scale -"));
    addPlaceholder(tr("Scale +"));
    addPlaceholder(tr("Center Horizontal"));
    addPlaceholder(tr("Center Vertical"));
    addPlaceholder(tr("Rotate Left"));
    addPlaceholder(tr("Rotate Right"));
    addPlaceholder(tr("Reset Properties"));
}

void MainWindow::createCentralWidget()
{
    auto *central = new QWidget(this);
    auto *layout = new QHBoxLayout(central);
    layout->setContentsMargins(4, 4, 4, 4);

    m_scene = new FaceScene(m_face, this);
    m_view = new QGraphicsView(m_scene, central);
    m_view->setRenderHint(QPainter::Antialiasing, true);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform, true);
    m_view->setBackgroundBrush(Qt::white);
    m_view->setMinimumSize(512, 512);
    m_view->setSceneRect(-256, -256, 512, 512);
    m_view->centerOn(0, 0);

    m_browser = new PartBrowser(m_dataRoot, central);
    m_browser->setMinimumWidth(220);
    connect(m_browser, &PartBrowser::partSelected,
            this, &MainWindow::onPartSelected);
    connect(m_browser, &PartBrowser::currentTypeChanged,
            this, &MainWindow::onCurrentTypeChanged);

    layout->addWidget(m_view, /*stretch=*/1);
    layout->addWidget(m_browser);

    setCentralWidget(central);
}

void MainWindow::createStatusBar()
{
    m_statusHelp = new QLabel(
        tr("PgUp, PgDown: scale   |   Home, End: rotate   |   "
           "Cursor keys: move   |   Cursor+Shift: move vertical"));
    m_statusHelp->setMargin(4);
    statusBar()->addWidget(m_statusHelp, /*stretch=*/1);
}

void MainWindow::onNew()
{
    m_scene->clearFace();
}

void MainWindow::onQuit()
{
    QApplication::quit();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("About FaceBuilder"),
        tr("<h3>FaceBuilder 0.2.0</h3>"
           "<p>A face-composition toy.</p>"
           "<p>C++/Qt6 rewrite of the original Ruby + GnomeCanvas application "
           "by Ingo Ruhnke.</p>"
           "<p>License: GPLv3+</p>"));
}

void MainWindow::onPartSelected(PartType type, const QString &filename)
{
    m_face->setPartFilename(type, filename);
}

void MainWindow::onCurrentTypeChanged(PartType type)
{
    m_currentType = type;
}
