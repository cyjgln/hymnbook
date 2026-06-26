// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class DirectoryPage;
class HymnViewerPage;
class AboutPage;
class SettingsPage;
class SearchDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

    enum Page {
        DirectoryPageIndex = 0,
        HymnViewerPageIndex,
        AboutPageIndex,
        SettingsPageIndex
    };

    void switchToPage(Page page);
    void switchToDirectory();
    void switchToHymnViewer(int hymnIndex);
    void switchToAbout();
    void switchToSettings();
    void showSearchDialog();

private:
    void setupUI();
    void setupShortcuts();
    void initData();

    QStackedWidget *m_stackedWidget;
    DirectoryPage *m_directoryPage;
    HymnViewerPage *m_hymnViewerPage;
    AboutPage *m_aboutPage;
    SettingsPage *m_settingsPage;
    SearchDialog *m_searchDialog;
};

#endif // MAINWINDOW_H
