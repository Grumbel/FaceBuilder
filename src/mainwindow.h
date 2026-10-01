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
class QAction;
class QGraphicsView;
class QLabel;
class QUndoStack;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &dataRoot, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onNew();
    void onQuit();
    void onAbout();
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

private:
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createCentralWidget();
    QIcon loadToolIcon(const QString &name) const;

    QString m_dataRoot;
    Face *m_face = nullptr;
    FaceScene *m_scene = nullptr;
    QGraphicsView *m_view = nullptr;
    PartBrowser *m_browser = nullptr;
    QLabel *m_statusHelp = nullptr;
    QUndoStack *m_undoStack = nullptr;
    PartType m_currentType = PartType::Eye;
    QAction *m_undoAct = nullptr;
    QAction *m_redoAct = nullptr;
};

#endif
