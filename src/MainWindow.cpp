#include "MainWindow.h"
#include "DirectoryPage.h"
#include "HymnViewerPage.h"
#include "AboutPage.h"
#include "HymnManager.h"
#include "SearchDialog.h"
#include "EditDialog.h"

#include <QStackedWidget>
#include <QShortcut>
#include <QKeySequence>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupShortcuts();
    initData();

    // 默认显示目录页
    switchToPage(DirectoryPageIndex);
}

void MainWindow::initData()
{
    // 持久化数据路径：用户应用数据目录（Linux ~/.local/share/HymnBook/HymnBookApp/）
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString primaryPath = appDataDir + QStringLiteral("/data/hymns.json");

    // Priority 1: 持久化路径已存在 → 直接加载
    if (QFile::exists(primaryPath)) {
        if (HymnManager::instance().load(primaryPath)) {
            qDebug() << "数据加载成功:" << primaryPath;
            return;
        }
    }

    // Priority 2: 从旧位置（编译目录等）迁移到持久化路径
    QStringList legacyPaths = {
        QCoreApplication::applicationDirPath() + QStringLiteral("/data/hymns.json"),
        QDir::currentPath() + QStringLiteral("/data/hymns.json"),
    };

    for (const QString &legacyPath : legacyPaths) {
        if (legacyPath == primaryPath) continue;
        if (!QFile::exists(legacyPath)) continue;

        // 迁移数据文件
        QDir().mkpath(appDataDir + QStringLiteral("/data"));
        if (QFile::copy(legacyPath, primaryPath)) {
            // 迁移 images 目录
            QString legacyImages = QFileInfo(legacyPath).absolutePath()
                                   + QStringLiteral("/../images");
            QString newImages = appDataDir + QStringLiteral("/images");
            if (QDir(legacyImages).exists()) {
                QDir().mkpath(newImages);
                for (const QFileInfo &fi :
                     QDir(legacyImages).entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
                    QFile::copy(fi.absoluteFilePath(),
                                newImages + QStringLiteral("/") + fi.fileName());
                }
            }

            // 从新位置加载（m_filePath 指向持久化路径）
            if (HymnManager::instance().load(primaryPath)) {
                qDebug() << "旧数据已迁移:" << legacyPath << "→" << primaryPath;
                return;
            }
        }
    }

    // Priority 3: 真正没有数据 → 在持久化路径创建初始数据
    qWarning() << "未找到 hymns.json，将在 AppDataLocation 创建初始数据";
    QDir().mkpath(appDataDir + QStringLiteral("/data"));
    QDir().mkpath(appDataDir + QStringLiteral("/images"));

    HymnManager::instance().load(primaryPath);
    Hymn sample;
    sample.title = QStringLiteral("奇异恩典");
    HymnManager::instance().addHymn(sample);
    HymnManager::instance().save();
}

void MainWindow::setupUI()
{
    setWindowTitle(QStringLiteral("诗歌本"));
    resize(1200, 800);
    setMinimumSize(800, 600);

    m_stackedWidget = new QStackedWidget(this);

    m_directoryPage = new DirectoryPage(this);
    m_hymnViewerPage = new HymnViewerPage(this);
    m_aboutPage = new AboutPage(this);

    m_stackedWidget->addWidget(m_directoryPage);   // index 0
    m_stackedWidget->addWidget(m_hymnViewerPage);  // index 1
    m_stackedWidget->addWidget(m_aboutPage);       // index 2

    setCentralWidget(m_stackedWidget);

    // 搜索弹窗（延迟创建，首次使用时初始化）
    m_searchDialog = nullptr;

    // ---- 页面间信号连接 ----

    // 目录页 → 歌谱页
    connect(m_directoryPage, &DirectoryPage::hymnSelected,
            this, &MainWindow::switchToHymnViewer);

    // 目录页 → 说明页
    connect(m_directoryPage, &DirectoryPage::aboutRequested,
            this, &MainWindow::switchToAbout);

    // 歌谱页 → 返回目录
    connect(m_hymnViewerPage, &HymnViewerPage::directoryRequested,
            this, &MainWindow::switchToDirectory);

    // 歌谱页 → 上一首
    connect(m_hymnViewerPage, &HymnViewerPage::previousRequested,
            this, [this]() {
        int total = HymnManager::instance().count();
        if (total == 0) return;
        // HymnViewerPage 负责计算索引，MainWindow 只负责切换
        int idx = m_hymnViewerPage->currentIndex();
        if (idx > 0)
            switchToHymnViewer(idx - 1);
    });

    // 歌谱页 → 下一首
    connect(m_hymnViewerPage, &HymnViewerPage::nextRequested,
            this, [this]() {
        int total = HymnManager::instance().count();
        if (total == 0) return;
        int idx = m_hymnViewerPage->currentIndex();
        if (idx < total - 1)
            switchToHymnViewer(idx + 1);
    });

    // 说明页 → 返回目录
    connect(m_aboutPage, &AboutPage::directoryRequested,
            this, &MainWindow::switchToDirectory);

    // 歌谱页 → 搜索
    connect(m_hymnViewerPage, &HymnViewerPage::searchRequested,
            this, &MainWindow::showSearchDialog);

    // ---- 编辑功能信号连接 ----

    // 目录页 → 新增歌谱
    connect(m_directoryPage, &DirectoryPage::addHymnRequested, this, [this]() {
        EditDialog dlg(EditDialog::AddMode, Hymn(), this);
        connect(&dlg, &EditDialog::deleteRequested, this, [](int) {});
        if (dlg.exec() == QDialog::Accepted) {
            // 新增后跳转到最后一首（即刚新增的）
            int lastIdx = HymnManager::instance().count() - 1;
            switchToHymnViewer(lastIdx);
        }
    });

    // 歌谱页 → 编辑当前歌谱
    connect(m_hymnViewerPage, &HymnViewerPage::editHymnRequested, this, [this](int hymnIndex) {
        if (hymnIndex < 0 || hymnIndex >= HymnManager::instance().count())
            return;

        Hymn hymn = HymnManager::instance().hymnByIndex(hymnIndex);
        EditDialog dlg(EditDialog::EditMode, hymn, this);

        bool deleted = false;
        connect(&dlg, &EditDialog::deleteRequested, this, [this, &deleted](int hymnId) {
            HymnManager::instance().deleteHymn(hymnId);
            HymnManager::instance().save();
            deleted = true;
            switchToDirectory();
        });

        if (dlg.exec() == QDialog::Accepted && !deleted) {
            // 重新加载当前歌谱（修改后）
            m_hymnViewerPage->loadHymn(hymnIndex);
        }
    });
}

void MainWindow::setupShortcuts()
{
    // F11 全屏切换
    auto *fullscreenShortcut = new QShortcut(QKeySequence(Qt::Key_F11), this);
    connect(fullscreenShortcut, &QShortcut::activated, this, [this]() {
        if (isFullScreen())
            showNormal();
        else
            showFullScreen();
    });

    // Ctrl+F 搜索
    auto *searchShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    connect(searchShortcut, &QShortcut::activated, this, &MainWindow::showSearchDialog);

    // Esc 返回目录
    auto *escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, &QShortcut::activated, this, &MainWindow::switchToDirectory);
}

void MainWindow::switchToPage(Page page)
{
    m_stackedWidget->setCurrentIndex(static_cast<int>(page));
}

void MainWindow::switchToDirectory()
{
    switchToPage(DirectoryPageIndex);
}

void MainWindow::switchToHymnViewer(int hymnIndex)
{
    if (hymnIndex < 0 || hymnIndex >= HymnManager::instance().count())
        return;

    // 先切换到歌谱页，确保页面可见、视口尺寸正确
    switchToPage(HymnViewerPageIndex);
    m_hymnViewerPage->loadHymn(hymnIndex);
}

void MainWindow::switchToAbout()
{
    switchToPage(AboutPageIndex);
}

void MainWindow::showSearchDialog()
{
    if (!m_searchDialog) {
        m_searchDialog = new SearchDialog(this);
        connect(m_searchDialog, &SearchDialog::hymnSelected,
                this, &MainWindow::switchToHymnViewer);
    }
    m_searchDialog->show();
    m_searchDialog->raise();
    m_searchDialog->activateWindow();
}
