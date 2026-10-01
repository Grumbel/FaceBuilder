// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "parttypes.h"

#include <QMainWindow>
#include <QPointF>
#include <QString>

class Face;
class FaceScene;
class PartBrowser;
class XmlFaceFormat;
class CanvasControls;
class QAction;
class QGraphicsView;
class QLabel;
class QUndoStack;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &dataRoot, QWidget *parent = nullptr);

    /** Load a face XML (used at startup and from CLI). */
    bool loadFaceFile(const QString &path);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onExportPng();
    void onExportSvg();
    void onQuit();
    void onAbout();
    void onCopy();
    void onPaste();
    void onPartSelected(PartType type, const QString &filename);
    void onBrowserTypeChanged(PartType type);
    void onSceneTypeChanged(PartType type);
    void onPartMoved(PartType type, const QPointF &oldOffset, const QPointF &newOffset);
    void onScaleMinus();
    void onScalePlus();
    void onCenterHorizontal();
    void onCenterVertical();
    void onRotateLeft();
    void onRotateRight();
    void onResetProperties();
    void onCenterFace();
    void onReload();
    void onNextPartFile();
    void onPrevPartFile();
    void onNextCategory();
    void onPrevCategory();
    void onToggleCanvasControls(bool on);
    void onCanvasCycle(PartType type, int delta);

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createCentralWidget();
    QIcon loadToolIcon(const QString &name) const;
    void setCurrentFile(const QString &path);
    bool saveToPath(const QString &path);
    bool loadFromPath(const QString &path);
    QString examplesDir() const;

    /** Ensure action icon is shown in menus (some styles hide them by default). */
    static void showIconInMenu(QAction *action);

    QString m_dataRoot;
    QString m_currentFile;
    Face *m_face = nullptr;
    FaceScene *m_scene = nullptr;
    QGraphicsView *m_view = nullptr;
    PartBrowser *m_browser = nullptr;
    XmlFaceFormat *m_format = nullptr;
    CanvasControls *m_canvasControls = nullptr;
    QLabel *m_statusHelp = nullptr;
    QUndoStack *m_undoStack = nullptr;
    PartType m_currentType = PartType::Eye;

    QAction *m_newAct = nullptr;
    QAction *m_openAct = nullptr;
    QAction *m_saveAct = nullptr;
    QAction *m_saveAsAct = nullptr;
    QAction *m_exportPngAct = nullptr;
    QAction *m_exportSvgAct = nullptr;
    QAction *m_quitAct = nullptr;
    QAction *m_undoAct = nullptr;
    QAction *m_redoAct = nullptr;
    QAction *m_copyAct = nullptr;
    QAction *m_pasteAct = nullptr;
    QAction *m_centerFaceAct = nullptr;
    QAction *m_aboutAct = nullptr;
    QAction *m_scaleMinusAct = nullptr;
    QAction *m_scalePlusAct = nullptr;
    QAction *m_centerHAct = nullptr;
    QAction *m_centerVAct = nullptr;
    QAction *m_rotateLeftAct = nullptr;
    QAction *m_rotateRightAct = nullptr;
    QAction *m_resetAct = nullptr;
    QAction *m_reloadAct = nullptr;
    QAction *m_showControlsAct = nullptr;
};

#endif
