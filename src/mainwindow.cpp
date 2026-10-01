// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#include "mainwindow.h"
#include "commands.h"
#include "face.h"
#include "faceexport.h"
#include "facescene.h"
#include "partbrowser.h"
#include "xmlfaceformat.h"
#include "paths.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>
#include <QUndoStack>
#include <QWheelEvent>
#include <QWidget>

static constexpr qreal kScaleStep = 1.02;
static constexpr qreal kRotateStep = 1.0;

static QIcon themedIcon(const QString &themeName, QStyle::StandardPixmap fallback)
{
    QIcon icon = QIcon::fromTheme(themeName);
    if (icon.isNull())
        icon = QApplication::style()->standardIcon(fallback);
    return icon;
}



MainWindow::MainWindow(const QString &dataRoot, QWidget *parent)
    : QMainWindow(parent), m_dataRoot(dataRoot)
{
    setWindowTitle(tr("FaceBuilder"));
    resize(900, 650);
    m_face = new Face(this);
    m_undoStack = new QUndoStack(this);
    m_format = new XmlFaceFormat(m_dataRoot);

    createMenus();
    createToolBar();
    createCentralWidget();
    createStatusBar();

    // Match original: load pirate example on startup if present.
    const QString pirate = examplesDir() + QStringLiteral("/pirate.xml");
    if (QFile::exists(pirate))
        loadFromPath(pirate);
}

QString MainWindow::examplesDir() const
{
    return findExamplesDir(m_dataRoot);
}

QIcon MainWindow::loadToolIcon(const QString &name) const
{
    const QStringList candidates = {
        QCoreApplication::applicationDirPath() + QStringLiteral("/../resources/icons/") + name,
        QCoreApplication::applicationDirPath() + QStringLiteral("/../share/facebuilder/icons/") + name,
        QStringLiteral("resources/icons/") + name,
        m_dataRoot + QStringLiteral("/../resources/icons/") + name,
    };
    for (const QString &p : candidates) {
        if (QFile::exists(p))
            return QIcon(p);
    }
    return {};
}

void MainWindow::setCurrentFile(const QString &path)
{
    m_currentFile = path;
    m_saveAct->setEnabled(!path.isEmpty());
    if (path.isEmpty())
        setWindowTitle(tr("FaceBuilder"));
    else
        setWindowTitle(tr("FaceBuilder — %1").arg(QFileInfo(path).fileName()));
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    auto *newAct = fileMenu->addAction(tr("&New"), this, &MainWindow::onNew);
    newAct->setShortcut(QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, &MainWindow::onOpen)->setShortcut(QKeySequence::Open);
    m_saveAct = fileMenu->addAction(tr("&Save"), this, &MainWindow::onSave);
    m_saveAct->setShortcut(QKeySequence::Save);
    m_saveAct->setEnabled(false);
    fileMenu->addAction(tr("Save &As..."), this, &MainWindow::onSaveAs)
        ->setShortcut(QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Export &PNG..."), this, &MainWindow::onExportPng);
    fileMenu->addAction(tr("Export SV&G..."), this, &MainWindow::onExportSvg);
    fileMenu->addSeparator();
    auto *quitAct = fileMenu->addAction(tr("&Quit"), this, &MainWindow::onQuit);
    quitAct->setShortcut(QKeySequence::Quit);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    m_undoAct = m_undoStack->createUndoAction(this, tr("&Undo"));
    m_undoAct->setShortcut(QKeySequence::Undo);
    editMenu->addAction(m_undoAct);
    m_redoAct = m_undoStack->createRedoAction(this, tr("&Redo"));
    m_redoAct->setShortcut(QKeySequence::Redo);
    editMenu->addAction(m_redoAct);
    editMenu->addSeparator();
    editMenu->addAction(tr("&Copy"))->setEnabled(false);
    editMenu->addAction(tr("&Paste"))->setEnabled(false);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&Center Face"), this, &MainWindow::onCenterFace);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About FaceBuilder"), this, &MainWindow::onAbout);
}

void MainWindow::createToolBar()
{
    QToolBar *tb = addToolBar(tr("Main"));
    tb->setMovable(false);
    tb->setIconSize(QSize(24, 24));

    {
        auto *a = tb->addAction(themedIcon(QStringLiteral("document-new"), QStyle::SP_FileIcon),
                                tr("New"), this, &MainWindow::onNew);
        a->setToolTip(tr("New"));
    }
    {
        auto *a = tb->addAction(themedIcon(QStringLiteral("document-open"), QStyle::SP_DirOpenIcon),
                                tr("Open"), this, &MainWindow::onOpen);
        a->setToolTip(tr("Open"));
    }
    {
        auto *a = tb->addAction(themedIcon(QStringLiteral("document-save"), QStyle::SP_DialogSaveButton),
                                tr("Save"), this, &MainWindow::onSave);
        a->setToolTip(tr("Save"));
    }
    {
        auto *a = tb->addAction(themedIcon(QStringLiteral("document-save-as"), QStyle::SP_DialogSaveButton),
                                tr("Save As"), this, &MainWindow::onSaveAs);
        a->setToolTip(tr("Save As"));
    }
    tb->addSeparator();
    tb->addAction(m_undoAct);
    tb->addAction(m_redoAct);
    tb->addSeparator();
    tb->addAction(tr("Copy"))->setEnabled(false);
    tb->addAction(tr("Paste"))->setEnabled(false);
    tb->addSeparator();

    tb->addAction(loadToolIcon(QStringLiteral("icon_size_minus.png")),
                  tr("Scale -"), this, &MainWindow::onScaleMinus)
        ->setToolTip(tr("Scale down"));
    tb->addAction(loadToolIcon(QStringLiteral("icon_size_plus.png")),
                  tr("Scale +"), this, &MainWindow::onScalePlus)
        ->setToolTip(tr("Scale up"));
    tb->addAction(loadToolIcon(QStringLiteral("icon_center_horizontal.png")),
                  tr("Center H"), this, &MainWindow::onCenterHorizontal)
        ->setToolTip(tr("Center horizontal"));
    tb->addAction(loadToolIcon(QStringLiteral("icon_center_vertical.png")),
                  tr("Center V"), this, &MainWindow::onCenterVertical)
        ->setToolTip(tr("Center vertical"));
    tb->addAction(loadToolIcon(QStringLiteral("icon_rotate_left.png")),
                  tr("Rotate Left"), this, &MainWindow::onRotateLeft)
        ->setToolTip(tr("Rotate left"));
    tb->addAction(loadToolIcon(QStringLiteral("icon_rotate_right.png")),
                  tr("Rotate Right"), this, &MainWindow::onRotateRight)
        ->setToolTip(tr("Rotate right"));
    tb->addAction(tr("Reset"), this, &MainWindow::onResetProperties)
        ->setToolTip(tr("Reset scale and rotation"));
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
    m_view->setFocusPolicy(Qt::StrongFocus);
    m_view->installEventFilter(this);

    m_browser = new PartBrowser(m_dataRoot, central);
    m_browser->setMinimumWidth(220);
    connect(m_browser, &PartBrowser::partSelected, this, &MainWindow::onPartSelected);
    connect(m_browser, &PartBrowser::currentTypeChanged, this, &MainWindow::onBrowserTypeChanged);
    connect(m_scene, &FaceScene::currentTypeChanged, this, &MainWindow::onSceneTypeChanged);
    connect(m_scene, &FaceScene::partMoved, this, &MainWindow::onPartMoved);

    layout->addWidget(m_view, 1);
    layout->addWidget(m_browser);
    setCentralWidget(central);
    m_view->setFocus();
}

void MainWindow::createStatusBar()
{
    m_statusHelp = new QLabel(
        tr("PgUp, PgDown: scale   |   Home, End: rotate   |   "
           "Cursor keys: move   |   Shift+drag: lock horizontal"));
    m_statusHelp->setMargin(4);
    statusBar()->addWidget(m_statusHelp, 1);
}

bool MainWindow::loadFromPath(const QString &path)
{
    QString err;
    if (!m_format->load(m_face, path, &err)) {
        QMessageBox::warning(this, tr("Open failed"), err);
        return false;
    }
    m_undoStack->clear();
    setCurrentFile(path);
    return true;
}

bool MainWindow::saveToPath(const QString &path)
{
    QString err;
    if (!m_format->save(m_face, path, &err)) {
        QMessageBox::warning(this, tr("Save failed"), err);
        return false;
    }
    setCurrentFile(path);
    return true;
}

void MainWindow::onNew()
{
    m_undoStack->clear();
    m_scene->clearFace();
    setCurrentFile({});
}

void MainWindow::onOpen()
{
    const QString start = m_currentFile.isEmpty() ? examplesDir() : QFileInfo(m_currentFile).path();
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Open Face"), start,
        tr("FaceBuilder XML (*.xml);;All files (*)"));
    if (path.isEmpty())
        return;
    loadFromPath(path);
}

void MainWindow::onSave()
{
    if (m_currentFile.isEmpty()) {
        onSaveAs();
        return;
    }
    saveToPath(m_currentFile);
}

void MainWindow::onSaveAs()
{
    const QString start = m_currentFile.isEmpty()
        ? examplesDir() + QStringLiteral("/untitled.xml")
        : m_currentFile;
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Save Face"), start,
        tr("FaceBuilder XML (*.xml);;All files (*)"));
    if (path.isEmpty())
        return;
    saveToPath(path);
}

void MainWindow::onExportPng()
{
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Export PNG"), QStringLiteral("face.png"),
        tr("PNG images (*.png);;All files (*)"));
    if (path.isEmpty())
        return;
    QString err;
    if (!FaceExport::toPng(m_scene, path, &err))
        QMessageBox::warning(this, tr("Export failed"), err);
}

void MainWindow::onExportSvg()
{
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Export SVG"), QStringLiteral("face.svg"),
        tr("SVG images (*.svg);;All files (*)"));
    if (path.isEmpty())
        return;
    QString err;
    if (!FaceExport::toSvg(m_face, path, &err))
        QMessageBox::warning(this, tr("Export failed"), err);
}

void MainWindow::onCenterFace()
{
    const QPointF head = m_face->part(PartType::Head).offset();
    if (qFuzzyIsNull(head.x()) && qFuzzyIsNull(head.y()))
        return;
    m_undoStack->push(new CenterFaceCommand(m_face));
}

void MainWindow::onQuit() { QApplication::quit(); }

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
    const QString old = m_face->part(type).filename();
    if (old == filename) return;
    m_undoStack->push(new SetPartFilenameCommand(m_face, type, old, filename));
    m_scene->setCurrentType(type);
    m_currentType = type;
}

void MainWindow::onBrowserTypeChanged(PartType type)
{
    m_currentType = type;
    m_scene->setCurrentType(type);
}

void MainWindow::onSceneTypeChanged(PartType type)
{
    m_currentType = type;
    m_browser->setCurrentType(type);
}

void MainWindow::onPartMoved(PartType type, const QPointF &oldOffset, const QPointF &newOffset)
{
    m_undoStack->push(new SetPartOffsetCommand(m_face, type, oldOffset, newOffset));
}

void MainWindow::onScaleMinus()
{
    const qreal oldS = m_face->part(m_currentType).scale();
    m_undoStack->push(new SetPartScaleCommand(m_face, m_currentType, oldS, oldS / kScaleStep));
}

void MainWindow::onScalePlus()
{
    const qreal oldS = m_face->part(m_currentType).scale();
    m_undoStack->push(new SetPartScaleCommand(m_face, m_currentType, oldS, oldS * kScaleStep));
}

void MainWindow::onCenterHorizontal()
{
    const QPointF oldO = m_face->part(m_currentType).offset();
    const QPointF newO(0, oldO.y());
    if (oldO == newO) return;
    m_undoStack->push(new SetPartOffsetCommand(m_face, m_currentType, oldO, newO));
}

void MainWindow::onCenterVertical()
{
    const QPointF oldO = m_face->part(m_currentType).offset();
    const QPointF newO(oldO.x(), 0);
    if (oldO == newO) return;
    m_undoStack->push(new SetPartOffsetCommand(m_face, m_currentType, oldO, newO));
}

void MainWindow::onRotateLeft()
{
    const qreal oldR = m_face->part(m_currentType).rotation();
    m_undoStack->push(new SetPartRotationCommand(m_face, m_currentType, oldR, oldR - kRotateStep));
}

void MainWindow::onRotateRight()
{
    const qreal oldR = m_face->part(m_currentType).rotation();
    m_undoStack->push(new SetPartRotationCommand(m_face, m_currentType, oldR, oldR + kRotateStep));
}

void MainWindow::onResetProperties()
{
    const auto &p = m_face->part(m_currentType);
    if (qFuzzyCompare(p.scale(), 1.0) && qFuzzyIsNull(p.rotation())) return;
    m_undoStack->push(new ResetPartTransformCommand(m_face, m_currentType, p.scale(), p.rotation()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_PageUp: {
        const qreal oldS = m_face->part(m_currentType).scale();
        m_undoStack->push(new SetPartScaleCommand(m_face, m_currentType, oldS, oldS * kScaleStep));
        event->accept(); return;
    }
    case Qt::Key_PageDown: {
        const qreal oldS = m_face->part(m_currentType).scale();
        m_undoStack->push(new SetPartScaleCommand(m_face, m_currentType, oldS, oldS / kScaleStep));
        event->accept(); return;
    }
    case Qt::Key_Home: {
        const qreal oldR = m_face->part(m_currentType).rotation();
        m_undoStack->push(new SetPartRotationCommand(m_face, m_currentType, oldR, oldR + kRotateStep));
        event->accept(); return;
    }
    case Qt::Key_End: {
        const qreal oldR = m_face->part(m_currentType).rotation();
        m_undoStack->push(new SetPartRotationCommand(m_face, m_currentType, oldR, oldR - kRotateStep));
        event->accept(); return;
    }
    case Qt::Key_Left: case Qt::Key_Right: case Qt::Key_Up: case Qt::Key_Down: {
        QPointF oldO = m_face->part(m_currentType).offset();
        QPointF newO = oldO;
        if (event->key() == Qt::Key_Left) newO.rx() -= 1;
        else if (event->key() == Qt::Key_Right) newO.rx() += 1;
        else if (event->key() == Qt::Key_Up) newO.ry() -= 1;
        else newO.ry() += 1;
        m_undoStack->push(new SetPartOffsetCommand(m_face, m_currentType, oldO, newO));
        event->accept(); return;
    }
    default: break;
    }
    QMainWindow::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_view && event->type() == QEvent::Wheel) {
        auto *we = static_cast<QWheelEvent *>(event);
        const qreal oldS = m_face->part(m_currentType).scale();
        qreal newS = oldS;
        if (we->angleDelta().y() > 0) newS = oldS * kScaleStep;
        else if (we->angleDelta().y() < 0) newS = oldS / kScaleStep;
        if (!qFuzzyCompare(oldS, newS))
            m_undoStack->push(new SetPartScaleCommand(m_face, m_currentType, oldS, newS));
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}
