#include "DirectoryPage.h"
#include "HymnManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QFont>

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
