// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


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

    m_addBtn = new QPushButton(QStringLiteral("＋ 新增歌谱"), this);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addBtn, &QPushButton::clicked, this, &DirectoryPage::addHymnRequested);
    buttonBar->addWidget(m_addBtn);

    m_batchBtn = new QPushButton(QStringLiteral("批量导入"), this);
    m_batchBtn->setObjectName(QStringLiteral("secondaryBtn"));
    m_batchBtn->setCursor(Qt::PointingHandCursor);
    connect(m_batchBtn, &QPushButton::clicked, this, &DirectoryPage::batchImport);
    buttonBar->addWidget(m_batchBtn);

    m_batchDeleteBtn = new QPushButton(QStringLiteral("🗑 批量删除"), this);
    m_batchDeleteBtn->setObjectName(QStringLiteral("secondaryBtn"));
    m_batchDeleteBtn->setCursor(Qt::PointingHandCursor);
    connect(m_batchDeleteBtn, &QPushButton::clicked, this, &DirectoryPage::toggleSelectMode);
    buttonBar->addWidget(m_batchDeleteBtn);

    buttonBar->addStretch();

    // 选择模式按钮（初始隐藏）
    m_selectAllBtn = new QPushButton(QStringLiteral("全选"), this);
    m_selectAllBtn->setCursor(Qt::PointingHandCursor);
    m_selectAllBtn->setVisible(false);
    connect(m_selectAllBtn, &QPushButton::clicked, this, &DirectoryPage::selectAll);
    buttonBar->addWidget(m_selectAllBtn);

    m_invertSelectBtn = new QPushButton(QStringLiteral("反选"), this);
    m_invertSelectBtn->setObjectName(QStringLiteral("secondaryBtn"));
    m_invertSelectBtn->setCursor(Qt::PointingHandCursor);
    m_invertSelectBtn->setVisible(false);
    connect(m_invertSelectBtn, &QPushButton::clicked, this, &DirectoryPage::invertSelection);
    buttonBar->addWidget(m_invertSelectBtn);

    buttonBar->addStretch();

    m_deleteSelectedBtn = new QPushButton(QStringLiteral("删除选中"), this);
    m_deleteSelectedBtn->setCursor(Qt::PointingHandCursor);
    m_deleteSelectedBtn->setObjectName(QStringLiteral("dangerBtn"));
    m_deleteSelectedBtn->setVisible(false);
    connect(m_deleteSelectedBtn, &QPushButton::clicked, this, &DirectoryPage::deleteSelected);
    buttonBar->addWidget(m_deleteSelectedBtn);

    m_cancelSelectBtn = new QPushButton(QStringLiteral("取消"), this);
    m_cancelSelectBtn->setObjectName(QStringLiteral("secondaryBtn"));
    m_cancelSelectBtn->setCursor(Qt::PointingHandCursor);
    m_cancelSelectBtn->setVisible(false);
    connect(m_cancelSelectBtn, &QPushButton::clicked, this, &DirectoryPage::toggleSelectMode);
    buttonBar->addWidget(m_cancelSelectBtn);

    // 设置按钮
    m_settingsBtn = new QPushButton(QStringLiteral("设置"), this);
    m_settingsBtn->setObjectName(QStringLiteral("secondaryBtn"));
    m_settingsBtn->setCursor(Qt::PointingHandCursor);
    connect(m_settingsBtn, &QPushButton::clicked, this, &DirectoryPage::settingsRequested);
    buttonBar->addWidget(m_settingsBtn);

    // 说明按钮
    m_aboutBtn = new QPushButton(QStringLiteral("说明"), this);
    m_aboutBtn->setObjectName(QStringLiteral("secondaryBtn"));
    m_aboutBtn->setCursor(Qt::PointingHandCursor);
    connect(m_aboutBtn, &QPushButton::clicked, this, &DirectoryPage::aboutRequested);
    buttonBar->addWidget(m_aboutBtn);

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
    m_listWidget->setGridSize(QSize(420, 60));
    m_listWidget->setSpacing(10);
    m_listWidget->setResizeMode(QListView::Adjust);
    m_listWidget->setWordWrap(true);
    m_listWidget->setMovement(QListView::Static);
    m_listWidget->setUniformItemSizes(true);
    m_listWidget->setFrameShape(QFrame::NoFrame);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 自定义委托
    m_delegate = new HymnDelegate(this);
    m_listWidget->setItemDelegate(m_delegate);
    containerLayout->addWidget(m_listWidget, 1);

    layout->addWidget(listContainer, 1);

    // 点击跳转
    connect(m_listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        // 选择模式下不触发导航（复选框由 HymnDelegate::editorEvent 处理）
        if (m_selectMode)
            return;
        int index = m_listWidget->row(item);
        QVariant data = item->data(Qt::UserRole);
        if (data.isValid()) {
            emit hymnSelected(data.toInt());
        }
    });

    refresh();
}

void DirectoryPage::batchImport()
{
    // 选择模式下禁止批量导入
    if (m_selectMode) return;

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

// ---- 批量删除 ----

void DirectoryPage::toggleSelectMode()
{
    m_selectMode = !m_selectMode;

    m_delegate->setSelectMode(m_selectMode);

    // 进入选择模式时，清除所有已有选中状态
    if (m_selectMode) {
        for (int i = 0; i < m_listWidget->count(); ++i) {
            m_listWidget->item(i)->setData(Qt::CheckStateRole, Qt::Unchecked);
        }
    }

    // 切换按钮可见性
    m_addBtn->setVisible(!m_selectMode);
    m_batchBtn->setVisible(!m_selectMode);
    m_batchDeleteBtn->setVisible(!m_selectMode);
    m_settingsBtn->setVisible(!m_selectMode);
    m_aboutBtn->setVisible(!m_selectMode);

    m_selectAllBtn->setVisible(m_selectMode);
    m_invertSelectBtn->setVisible(m_selectMode);
    m_deleteSelectedBtn->setVisible(m_selectMode);
    m_cancelSelectBtn->setVisible(m_selectMode);

    // 进入选择模式时禁用列表点击导航（editorEvent 已拦截点击，这里同时禁用选择高亮）
    m_listWidget->setSelectionMode(m_selectMode
        ? QAbstractItemView::NoSelection
        : QAbstractItemView::SingleSelection);

    updateDeleteButtonText();
    m_listWidget->viewport()->update();
}

void DirectoryPage::selectAll()
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        m_listWidget->item(i)->setData(Qt::CheckStateRole, Qt::Checked);
    }
    updateDeleteButtonText();
    m_listWidget->viewport()->update();
}

void DirectoryPage::invertSelection()
{
    for (int i = 0; i < m_listWidget->count(); ++i) {
        auto *item = m_listWidget->item(i);
        Qt::CheckState state = static_cast<Qt::CheckState>(
            item->data(Qt::CheckStateRole).toInt());
        item->setData(Qt::CheckStateRole,
                      (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked);
    }
    updateDeleteButtonText();
    m_listWidget->viewport()->update();
}

int DirectoryPage::selectedCount() const
{
    int count = 0;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        if (m_listWidget->item(i)->data(Qt::CheckStateRole).toInt() == Qt::Checked)
            ++count;
    }
    return count;
}

void DirectoryPage::updateDeleteButtonText()
{
    int n = selectedCount();
    m_deleteSelectedBtn->setText(
        n > 0
            ? QStringLiteral("删除选中 (%1)").arg(n)
            : QStringLiteral("删除选中"));
}

void DirectoryPage::deleteSelected()
{
    int n = selectedCount();
    if (n == 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("请先勾选要删除的歌谱。"));
        return;
    }

    // 确认对话框
    auto result = QMessageBox::question(
        this,
        QStringLiteral("确认批量删除"),
        QStringLiteral("确定要删除选中的 %1 首歌谱吗？\n此操作不可撤销。").arg(n),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (result != QMessageBox::Yes)
        return;

    // 收集选中歌谱的 ID
    QList<int> ids;
    QStringList titles;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        auto *item = m_listWidget->item(i);
        if (item->data(Qt::CheckStateRole).toInt() == Qt::Checked) {
            int hymnIndex = item->data(Qt::UserRole).toInt();
            Hymn h = HymnManager::instance().hymnByIndex(hymnIndex);
            ids.append(h.id);
            titles.append(h.title);
        }
    }

    // 执行删除
    HymnManager &mgr = HymnManager::instance();
    mgr.deleteHymns(ids);
    mgr.save();

    // 退出选择模式
    toggleSelectMode();
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
        item->setData(Qt::CheckStateRole, Qt::Unchecked);  // 初始化未勾选
        m_listWidget->addItem(item);
    }

    // 如果处于选择模式，刷新后检查是否还有数据
    if (m_selectMode && hymns.isEmpty()) {
        toggleSelectMode();  // 自动退出选择模式
    }
}
