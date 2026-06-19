#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class DirectoryPage;
class HymnViewerPage;
class AboutPage;
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
        AboutPageIndex
    };

    void switchToPage(Page page);
    void switchToDirectory();
    void switchToHymnViewer(int hymnIndex);
    void switchToAbout();
    void showSearchDialog();

private:
    void setupUI();
    void setupShortcuts();
    void initData();

    QStackedWidget *m_stackedWidget;
    DirectoryPage *m_directoryPage;
    HymnViewerPage *m_hymnViewerPage;
    AboutPage *m_aboutPage;
    SearchDialog *m_searchDialog;
};

#endif // MAINWINDOW_H
