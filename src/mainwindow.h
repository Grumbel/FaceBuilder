// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class FaceScene;
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

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createCentralWidget();

    QString m_dataRoot;
    FaceScene *m_scene = nullptr;
    QGraphicsView *m_view = nullptr;
    QLabel *m_statusHelp = nullptr;
};

#endif // MAINWINDOW_H
