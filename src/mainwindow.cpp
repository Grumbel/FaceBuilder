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
#include <QClipboard>
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
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPixmap>
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

    createActions();
    createMenus();
    createToolBar();
    createCentralWidget();
    createStatusBar();

    // Application icon (installed or from resources next to data)
    {
        const QStringList iconCands = {
            m_dataRoot + QStringLiteral("/logo.png"),
            QCoreApplication::applicationDirPath() + QStringLiteral("/../share/icons/hicolor/256x256/apps/facebuilder.png"),
            QCoreApplication::applicationDirPath() + QStringLiteral("/../resources/icons/facebuilder.png"),
            QStringLiteral("resources/icons/facebuilder.png"),
        };
        for (const QString &p : iconCands) {
            if (QFile::exists(p)) {
                setWindowIcon(QIcon(p));
                break;
            }
        }
    }

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

void MainWindow::showIconInMenu(QAction *action)
{
    if (action)
        action->setIconVisibleInMenu(true);
}

void MainWindow::createActions()
{
    m_newAct = new QAction(themedIcon(QStringLiteral("document-new"), QStyle::SP_FileIcon),
                           tr("&New"), this);
    m_newAct->setShortcut(QKeySequence::New);
    m_newAct->setToolTip(tr("New face"));
    connect(m_newAct, &QAction::triggered, this, &MainWindow::onNew);

    m_openAct = new QAction(themedIcon(QStringLiteral("document-open"), QStyle::SP_DirOpenIcon),
                            tr("&Open..."), this);
    m_openAct->setShortcut(QKeySequence::Open);
    m_openAct->setToolTip(tr("Open face XML"));
    connect(m_openAct, &QAction::triggered, this, &MainWindow::onOpen);

    m_saveAct = new QAction(themedIcon(QStringLiteral("document-save"), QStyle::SP_DialogSaveButton),
                            tr("&Save"), this);
    m_saveAct->setShortcut(QKeySequence::Save);
    m_saveAct->setToolTip(tr("Save"));
    m_saveAct->setEnabled(false);
    connect(m_saveAct, &QAction::triggered, this, &MainWindow::onSave);

    m_saveAsAct = new QAction(themedIcon(QStringLiteral("document-save-as"), QStyle::SP_DialogSaveButton),
                              tr("Save &As..."), this);
    m_saveAsAct->setShortcut(QKeySequence::SaveAs);
    m_saveAsAct->setToolTip(tr("Save As"));
    connect(m_saveAsAct, &QAction::triggered, this, &MainWindow::onSaveAs);

    m_exportPngAct = new QAction(themedIcon(QStringLiteral("image-x-generic"), QStyle::SP_DesktopIcon),
                                 tr("Export &PNG..."), this);
    m_exportPngAct->setToolTip(tr("Export as PNG"));
    connect(m_exportPngAct, &QAction::triggered, this, &MainWindow::onExportPng);

    m_exportSvgAct = new QAction(themedIcon(QStringLiteral("image-x-generic"), QStyle::SP_FileDialogListView),
                                 tr("Export SV&G..."), this);
    m_exportSvgAct->setToolTip(tr("Export as SVG"));
    connect(m_exportSvgAct, &QAction::triggered, this, &MainWindow::onExportSvg);

    m_quitAct = new QAction(themedIcon(QStringLiteral("application-exit"), QStyle::SP_DialogCloseButton),
                            tr("&Quit"), this);
    m_quitAct->setShortcut(QKeySequence::Quit);
    m_quitAct->setToolTip(tr("Quit"));
    connect(m_quitAct, &QAction::triggered, this, &MainWindow::onQuit);

    m_undoAct = m_undoStack->createUndoAction(this, tr("&Undo"));
    m_undoAct->setIcon(themedIcon(QStringLiteral("edit-undo"), QStyle::SP_ArrowBack));
    m_undoAct->setShortcut(QKeySequence::Undo);
    m_undoAct->setToolTip(tr("Undo"));

    m_redoAct = m_undoStack->createRedoAction(this, tr("&Redo"));
    m_redoAct->setIcon(themedIcon(QStringLiteral("edit-redo"), QStyle::SP_ArrowForward));
    m_redoAct->setShortcut(QKeySequence::Redo);
    m_redoAct->setToolTip(tr("Redo"));

    m_copyAct = new QAction(themedIcon(QStringLiteral("edit-copy"), QStyle::SP_FileDialogDetailedView),
                            tr("&Copy"), this);
    m_copyAct->setShortcut(QKeySequence::Copy);
    m_copyAct->setToolTip(tr("Copy face XML"));
    connect(m_copyAct, &QAction::triggered, this, &MainWindow::onCopy);

    m_pasteAct = new QAction(themedIcon(QStringLiteral("edit-paste"), QStyle::SP_FileDialogContentsView),
                             tr("&Paste"), this);
    m_pasteAct->setShortcut(QKeySequence::Paste);
    m_pasteAct->setToolTip(tr("Paste face XML"));
    connect(m_pasteAct, &QAction::triggered, this, &MainWindow::onPaste);

    m_centerFaceAct = new QAction(themedIcon(QStringLiteral("zoom-fit-best"), QStyle::SP_TitleBarMaxButton),
                                  tr("&Center Face"), this);
    m_centerFaceAct->setToolTip(tr("Center face on head"));
    connect(m_centerFaceAct, &QAction::triggered, this, &MainWindow::onCenterFace);

    m_aboutAct = new QAction(themedIcon(QStringLiteral("help-about"), QStyle::SP_MessageBoxInformation),
                             tr("&About FaceBuilder"), this);
    m_aboutAct->setToolTip(tr("About FaceBuilder"));
    connect(m_aboutAct, &QAction::triggered, this, &MainWindow::onAbout);

    m_scaleMinusAct = new QAction(loadToolIcon(QStringLiteral("icon_size_minus.png")),
                                  tr("Scale -"), this);
    m_scaleMinusAct->setToolTip(tr("Scale down"));
    connect(m_scaleMinusAct, &QAction::triggered, this, &MainWindow::onScaleMinus);

    m_scalePlusAct = new QAction(loadToolIcon(QStringLiteral("icon_size_plus.png")),
                                 tr("Scale +"), this);
    m_scalePlusAct->setToolTip(tr("Scale up"));
    connect(m_scalePlusAct, &QAction::triggered, this, &MainWindow::onScalePlus);

    m_centerHAct = new QAction(loadToolIcon(QStringLiteral("icon_center_horizontal.png")),
                               tr("Center Horizontal"), this);
    m_centerHAct->setToolTip(tr("Center horizontal"));
    connect(m_centerHAct, &QAction::triggered, this, &MainWindow::onCenterHorizontal);

    m_centerVAct = new QAction(loadToolIcon(QStringLiteral("icon_center_vertical.png")),
                               tr("Center Vertical"), this);
    m_centerVAct->setToolTip(tr("Center vertical"));
    connect(m_centerVAct, &QAction::triggered, this, &MainWindow::onCenterVertical);

    m_rotateLeftAct = new QAction(loadToolIcon(QStringLiteral("icon_rotate_left.png")),
                                  tr("Rotate Left"), this);
    m_rotateLeftAct->setToolTip(tr("Rotate left"));
    connect(m_rotateLeftAct, &QAction::triggered, this, &MainWindow::onRotateLeft);

    m_rotateRightAct = new QAction(loadToolIcon(QStringLiteral("icon_rotate_right.png")),
                                   tr("Rotate Right"), this);
    m_rotateRightAct->setToolTip(tr("Rotate right"));
    connect(m_rotateRightAct, &QAction::triggered, this, &MainWindow::onRotateRight);

    m_resetAct = new QAction(themedIcon(QStringLiteral("edit-clear"), QStyle::SP_BrowserReload),
                             tr("Reset"), this);
    m_resetAct->setToolTip(tr("Reset scale and rotation"));
    connect(m_resetAct, &QAction::triggered, this, &MainWindow::onResetProperties);

    // Menus should display icons wherever the style allows.
    for (QAction *a : {
             m_newAct, m_openAct, m_saveAct, m_saveAsAct, m_exportPngAct, m_exportSvgAct,
             m_quitAct, m_undoAct, m_redoAct, m_copyAct, m_pasteAct, m_centerFaceAct,
             m_aboutAct, m_scaleMinusAct, m_scalePlusAct, m_centerHAct, m_centerVAct,
             m_rotateLeftAct, m_rotateRightAct, m_resetAct}) {
        showIconInMenu(a);
    }
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_newAct);
    fileMenu->addAction(m_openAct);
    fileMenu->addAction(m_saveAct);
    fileMenu->addAction(m_saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exportPngAct);
    fileMenu->addAction(m_exportSvgAct);
    fileMenu->addSeparator();
    fileMenu->addAction(m_quitAct);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(m_undoAct);
    editMenu->addAction(m_redoAct);
    editMenu->addSeparator();
    editMenu->addAction(m_copyAct);
    editMenu->addAction(m_pasteAct);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(m_centerFaceAct);
    viewMenu->addSeparator();
    viewMenu->addAction(m_scaleMinusAct);
    viewMenu->addAction(m_scalePlusAct);
    viewMenu->addAction(m_centerHAct);
    viewMenu->addAction(m_centerVAct);
    viewMenu->addAction(m_rotateLeftAct);
    viewMenu->addAction(m_rotateRightAct);
    viewMenu->addAction(m_resetAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(m_aboutAct);
}

void MainWindow::createToolBar()
{
    QToolBar *tb = addToolBar(tr("Main"));
    tb->setMovable(false);
    tb->setIconSize(QSize(24, 24));
    tb->setToolButtonStyle(Qt::ToolButtonIconOnly);

    tb->addAction(m_newAct);
    tb->addAction(m_openAct);
    tb->addAction(m_saveAct);
    tb->addAction(m_saveAsAct);
    tb->addSeparator();
    tb->addAction(m_undoAct);
    tb->addAction(m_redoAct);
    tb->addSeparator();
    tb->addAction(m_copyAct);
    tb->addAction(m_pasteAct);
    tb->addSeparator();
    tb->addAction(m_scaleMinusAct);
    tb->addAction(m_scalePlusAct);
    tb->addAction(m_centerHAct);
    tb->addAction(m_centerVAct);
    tb->addAction(m_rotateLeftAct);
    tb->addAction(m_rotateRightAct);
    tb->addAction(m_resetAct);
    tb->addSeparator();
    tb->addAction(m_centerFaceAct);
    tb->addAction(m_exportPngAct);
    tb->addAction(m_exportSvgAct);
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
    QMessageBox box(this);
    box.setWindowTitle(tr("About FaceBuilder"));
    box.setTextFormat(Qt::RichText);
    box.setText(tr("<h3>FaceBuilder 0.2.0</h3>"
                   "<p>A face-composition toy.</p>"
                   "<p>C++/Qt6 rewrite of the original Ruby + GnomeCanvas application "
                   "by Ingo Ruhnke.</p>"
                   "<p>License: GPLv3+</p>"));
    const QString logo = m_dataRoot + QStringLiteral("/logo.png");
    if (QFile::exists(logo))
        box.setIconPixmap(QPixmap(logo).scaledToWidth(200, Qt::SmoothTransformation));
    box.exec();
}

void MainWindow::onCopy()
{
    const QString xml = m_format->saveToString(m_face);
    QApplication::clipboard()->setText(xml);
    statusBar()->showMessage(tr("Face XML copied to clipboard"), 2000);
}

void MainWindow::onPaste()
{
    const QString xml = QApplication::clipboard()->text();
    if (xml.trimmed().isEmpty() || !xml.contains(QStringLiteral("<face"))) {
        QMessageBox::information(this, tr("Paste"),
                                 tr("Clipboard does not contain FaceBuilder XML."));
        return;
    }
    QString err;
    if (!m_format->loadFromString(m_face, xml, &err)) {
        QMessageBox::warning(this, tr("Paste failed"), err);
        return;
    }
    m_undoStack->clear();
    setCurrentFile({});
    statusBar()->showMessage(tr("Face pasted from clipboard"), 2000);
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
