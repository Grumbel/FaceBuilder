// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "mainwindow.h"
#include "facescene.h"

#include <QAction>
#include <QApplication>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(const QString &dataRoot, QWidget *parent)
    : QMainWindow(parent)
    , m_dataRoot(dataRoot)
{
    setWindowTitle(tr("FaceBuilder"));
    resize(900, 650);

    createActions();
    createMenus();
    createToolBar();
    createCentralWidget();
    createStatusBar();
}

void MainWindow::createActions()
{
    // Actions are created and owned by the menus/toolbars that use them.
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *newAct = fileMenu->addAction(tr("&New"), this, &MainWindow::onNew);
    newAct->setShortcut(QKeySequence::New);
    fileMenu->addSeparator();
    // Open / Save placeholders for later milestones
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

    // Placeholders matching the original toolbar order.
    // Icons will be wired in a later commit once resources are set up.
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

    m_scene = new FaceScene(this);
    m_view = new QGraphicsView(m_scene, central);
    m_view->setRenderHint(QPainter::Antialiasing, true);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform, true);
    m_view->setBackgroundBrush(Qt::white);
    m_view->setMinimumSize(512, 512);
    m_view->setSceneRect(-256, -256, 512, 512);
    m_view->centerOn(0, 0);

    // Placeholder for the part browser (right side) — filled in M2.
    auto *browserPlaceholder = new QLabel(tr("Part browser\n(coming in M2)"), central);
    browserPlaceholder->setAlignment(Qt::AlignCenter);
    browserPlaceholder->setMinimumWidth(220);
    browserPlaceholder->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    layout->addWidget(m_view, /*stretch=*/1);
    layout->addWidget(browserPlaceholder);

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
    // M1: just clear the scene; real face reset comes later.
    m_scene->clear();
    m_scene->addGuideFrame();
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
