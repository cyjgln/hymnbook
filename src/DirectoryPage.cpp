#include "DirectoryPage.h"
#include "HymnManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>
#include <QRegularExpression>

DirectoryPage::DirectoryPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();

    // 监听数据变更，自动刷新
    connect(&HymnManager::instance(), &HymnManager::dataChanged,
            this, &DirectoryPage::refresh);
}

void DirectoryPage::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    // 标题
    auto *titleLabel = new QLabel(QStringLiteral("诗歌本"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // 按钮区域
    auto *buttonBar = new QHBoxLayout();

    auto *addBtn = new QPushButton(QStringLiteral("新增歌谱"), this);
    connect(addBtn, &QPushButton::clicked, this, &DirectoryPage::addHymnRequested);
    buttonBar->addWidget(addBtn);

    auto *batchBtn = new QPushButton(QStringLiteral("批量导入"), this);
    connect(batchBtn, &QPushButton::clicked, this, &DirectoryPage::batchImport);
    buttonBar->addWidget(batchBtn);

    buttonBar->addStretch();

    auto *aboutBtn = new QPushButton(QStringLiteral("说明"), this);
    connect(aboutBtn, &QPushButton::clicked, this, &DirectoryPage::aboutRequested);
    buttonBar->addWidget(aboutBtn);
    layout->addLayout(buttonBar);

    // 诗歌列表
    m_listWidget = new QListWidget(this);
    m_listWidget->setFont(QFont(m_listWidget->font().family(), 14));
    m_listWidget->setSpacing(6);
    layout->addWidget(m_listWidget, 1);

    // 点击跳转
    connect(m_listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        int index = m_listWidget->row(item);
        QVariant data = item->data(Qt::UserRole);
        if (data.isValid()) {
            emit hymnSelected(data.toInt());  // 传递 hymnIndex
        }
    });

    refresh();
}

void DirectoryPage::batchImport()
{
    // 打开文件多选对话框
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        QStringLiteral("选择歌谱图片（批量导入）"),
        QString(),
        QStringLiteral("图片文件 (*.png *.jpg *.jpeg);;所有文件 (*)")
    );

    if (files.isEmpty())
        return;

    // 确定 images 目录（在 data 目录旁）
    QString dataDir = QFileInfo(HymnManager::instance().dataFilePath()).absolutePath();
    QString imagesDir = QDir(dataDir).absoluteFilePath(QStringLiteral("../images"));
    QDir().mkpath(imagesDir);

    QList<Hymn> batch;
    QStringList errors;

    for (const QString &filePath : files) {
        QFileInfo fi(filePath);
        QString ext = fi.suffix().toLower();

        // 校验文件格式
        if (ext != QStringLiteral("png") && ext != QStringLiteral("jpg") && ext != QStringLiteral("jpeg")) {
            errors.append(QStringLiteral("%1 （格式不支持）").arg(fi.fileName()));
            continue;
        }

        // 文件名（不含扩展名）作为歌名
        QString title = fi.completeBaseName();

        // 生成目标文件名
        QString safeName = title;
        safeName.replace(QRegularExpression(QStringLiteral("[\\\\/:*?\"<>|]")), QStringLiteral("_"));
        QString destName = QStringLiteral("%1_%2.%3")
                               .arg(safeName)
                               .arg(QDateTime::currentMSecsSinceEpoch())
                               .arg(ext);
        QString destPath = QDir(imagesDir).absoluteFilePath(destName);

        if (!QFile::copy(filePath, destPath)) {
            errors.append(QStringLiteral("%1 （图片复制失败）").arg(fi.fileName()));
            continue;
        }

        Hymn h;
        h.title = title;
        h.imagePath = QStringLiteral("images/") + destName;
        batch.append(h);
    }

    if (batch.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("导入结果"),
                             QStringLiteral("没有成功导入任何歌谱。"));
        return;
    }

    // 批量写入 HymnManager（只发射一次 dataChanged）
    HymnManager::instance().batchAddHymns(batch);
    HymnManager::instance().save();

    // 结果反馈
    QString msg = QStringLiteral("成功导入 %1 首歌谱。").arg(batch.size());
    if (!errors.isEmpty()) {
        msg += QStringLiteral("\n\n以下文件导入失败：\n");
        for (const QString &e : errors) {
            msg += QStringLiteral("  • ") + e + QStringLiteral("\n");
        }
    }
    QMessageBox::information(this, QStringLiteral("导入结果"), msg);
}

void DirectoryPage::refresh()
{
    m_listWidget->clear();

    const QList<Hymn> &hymns = HymnManager::instance().allHymns();
    for (int i = 0; i < hymns.size(); ++i) {
        const Hymn &h = hymns[i];
        QString text = QStringLiteral("%1  %2")
                           .arg(h.pageNumber, 3, 10, QChar('0'))
                           .arg(h.title);
        auto *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, i);  // 存储列表索引
        m_listWidget->addItem(item);
    }
}
