// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "parttypes.h"

#include <QMainWindow>
#include <QString>

class Face;
class FaceScene;
class PartBrowser;
class QGraphicsView;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &dataRoot, QWidget *parent = nullptr);

private slots:
    void onNew();
    void onQuit();
    void onAbout();
    void onPartSelected(PartType type, const QString &filename);
    void onCurrentTypeChanged(PartType type);

private:
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createCentralWidget();

    QString m_dataRoot;
    Face *m_face = nullptr;
    FaceScene *m_scene = nullptr;
    QGraphicsView *m_view = nullptr;
    PartBrowser *m_browser = nullptr;
    QLabel *m_statusHelp = nullptr;
    PartType m_currentType = PartType::Eye;
};

#endif // MAINWINDOW_H
