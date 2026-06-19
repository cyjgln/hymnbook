// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


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

#include "DirectoryPage.h"
#include "HymnManager.h"
#include "HymnDelegate.h"

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
#include <QSpacerItem>
#include <QListView>

DirectoryPage::DirectoryPage(QWidget *parent)
    : QWidget(parent), m_emptyLabel(nullptr)
{
    setupUI();

    // 监听数据变更，自动刷新
    connect(&HymnManager::instance(), &HymnManager::dataChanged,
            this, &DirectoryPage::refresh);
}

void DirectoryPage::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    // 标题
    auto *titleLabel = new QLabel(QStringLiteral("诗歌本"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName(QStringLiteral("pageTitle"));
    titleLabel->setStyleSheet(QStringLiteral(
        "QLabel#pageTitle {"
        "  color: #5a4a3a;"
        "  font-size: 28px;"
        "  font-weight: bold;"
        "  padding: 8px 0 16px 0;"
        "}"
    ));
    layout->addWidget(titleLabel);

    // 按钮区域
    auto *buttonBar = new QHBoxLayout();

    auto *addBtn = new QPushButton(QStringLiteral("＋ 新增歌谱"), this);
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &DirectoryPage::addHymnRequested);
    buttonBar->addWidget(addBtn);

    auto *batchBtn = new QPushButton(QStringLiteral("批量导入"), this);
    batchBtn->setObjectName(QStringLiteral("secondaryBtn"));
    batchBtn->setCursor(Qt::PointingHandCursor);
    connect(batchBtn, &QPushButton::clicked, this, &DirectoryPage::batchImport);
    buttonBar->addWidget(batchBtn);

    buttonBar->addStretch();

    auto *aboutBtn = new QPushButton(QStringLiteral("说明"), this);
    aboutBtn->setObjectName(QStringLiteral("secondaryBtn"));
    aboutBtn->setCursor(Qt::PointingHandCursor);
    connect(aboutBtn, &QPushButton::clicked, this, &DirectoryPage::aboutRequested);
    buttonBar->addWidget(aboutBtn);
    layout->addLayout(buttonBar);

    // 分隔线
    layout->addSpacing(4);
    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet(QStringLiteral("QFrame { color: #e0d5c8; }"));
    layout->addWidget(separator);
    layout->addSpacing(8);

    // 列表容器（略有不同的背景色，与按钮栏区分）
    auto *listContainer = new QFrame(this);
    listContainer->setObjectName(QStringLiteral("listContainer"));
    listContainer->setStyleSheet(QStringLiteral(
        "QFrame#listContainer {"
        "  background-color: #ede4d8;"
        "  border-radius: 8px;"
        "  padding: 8px;"
        "}"
    ));
    auto *containerLayout = new QVBoxLayout(listContainer);
    containerLayout->setContentsMargins(8, 8, 8, 8);

    // 空状态提示（放在容器内）
    m_emptyLabel = new QLabel(this);
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setObjectName(QStringLiteral("emptyHint"));
    m_emptyLabel->setStyleSheet(QStringLiteral(
        "QLabel#emptyHint {"
        "  color: #b8ad9e;"
        "  font-size: 16px;"
        "  padding: 60px 20px;"
        "}"
    ));
    m_emptyLabel->setText(QStringLiteral("暂无诗歌\n点击「新增歌谱」或「批量导入」添加"));
    containerLayout->addWidget(m_emptyLabel);

    // 诗歌列表（网格布局，自动多列）
    m_listWidget = new QListWidget(this);
    m_listWidget->setViewMode(QListView::IconMode);
    m_listWidget->setIconSize(QSize(0, 0));
    m_listWidget->setGridSize(QSize(420, 100));
    m_listWidget->setSpacing(10);
    m_listWidget->setResizeMode(QListView::Adjust);
    m_listWidget->setWordWrap(true);
    m_listWidget->setMovement(QListView::Static);
    m_listWidget->setUniformItemSizes(true);
    m_listWidget->setFrameShape(QFrame::NoFrame);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 自定义委托：可靠文字换行（跨平台）
    m_listWidget->setItemDelegate(new HymnDelegate(this));
    containerLayout->addWidget(m_listWidget, 1);

    layout->addWidget(listContainer, 1);

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

    // 切换空状态提示
    bool empty = hymns.isEmpty();
    m_emptyLabel->setVisible(empty);
    m_listWidget->setVisible(!empty);

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
