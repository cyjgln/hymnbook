// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#include "SearchDialog.h"
#include "HymnManager.h"
#include "Hymn.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QShowEvent>
#include <QFrame>

SearchDialog::SearchDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void SearchDialog::setupUI()
{
    setWindowTitle(QStringLiteral("搜索诗歌"));
    setFixedSize(420, 520);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    // 标题
    auto *titleLabel = new QLabel(QStringLiteral("搜索诗歌"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName(QStringLiteral("dialogTitle"));
    titleLabel->setStyleSheet(QStringLiteral(
        "QLabel#dialogTitle {"
        "  color: #5a4a3a;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  padding: 0 0 10px 0;"
        "}"
    ));
    layout->addWidget(titleLabel);

    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText(QStringLiteral("输入歌名关键字..."));
    m_searchInput->setClearButtonEnabled(true);
    QFont inputFont = m_searchInput->font();
    inputFont.setPointSize(14);
    m_searchInput->setFont(inputFont);
    layout->addWidget(m_searchInput);

    m_resultList = new QListWidget(this);
    m_resultList->setFont(QFont(m_resultList->font().family(), 13));
    m_resultList->setSpacing(4);
    m_resultList->setFrameShape(QFrame::NoFrame);
    layout->addWidget(m_resultList, 1);

    // 实时搜索：输入变化即触发
    connect(m_searchInput, &QLineEdit::textChanged,
            this, &SearchDialog::performSearch);

    // 双击结果 → 跳转
    connect(m_resultList, &QListWidget::itemDoubleClicked,
            this, [this](QListWidgetItem *item) {
        QVariant data = item->data(Qt::UserRole);
        if (data.isValid()) {
            emit hymnSelected(data.toInt());
            accept();
        }
    });

    // 自动聚焦输入框
    m_searchInput->setFocus();
}

void SearchDialog::performSearch(const QString &keyword)
{
    m_resultList->clear();

    if (keyword.trimmed().isEmpty()) {
        return;
    }

    QList<Hymn> results = HymnManager::instance().search(keyword.trimmed());

    if (results.isEmpty()) {
        auto *item = new QListWidgetItem(QStringLiteral("无结果"));
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        item->setForeground(QColor(128, 128, 128));
        m_resultList->addItem(item);
        return;
    }

    // 查找每个结果在 hymnManager 中的索引
    QList<Hymn> allHymns = HymnManager::instance().allHymns();
    for (const Hymn &h : results) {
        int idx = -1;
        for (int i = 0; i < allHymns.size(); ++i) {
            if (allHymns[i].id == h.id) {
                idx = i;
                break;
            }
        }
        QString text = QStringLiteral("%1  %2")
                           .arg(h.pageNumber, 3, 10, QChar('0'))
                           .arg(h.title);
        auto *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, idx);
        m_resultList->addItem(item);
    }
}

void SearchDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    // 每次打开时清空搜索框和结果
    m_searchInput->clear();
    m_resultList->clear();
    m_searchInput->setFocus();
}

void SearchDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Enter 键在当前结果项上触发跳转
        auto *currentItem = m_resultList->currentItem();
        if (currentItem) {
            QVariant data = currentItem->data(Qt::UserRole);
            if (data.isValid() && data.toInt() >= 0) {
                emit hymnSelected(data.toInt());
                accept();
                return;
            }
        }
    }
    // Esc 关闭弹窗
    if (event->key() == Qt::Key_Escape) {
        reject();
        return;
    }
    QDialog::keyPressEvent(event);
}
