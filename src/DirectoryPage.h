// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef DIRECTORYPAGE_H
#define DIRECTORYPAGE_H

#include <QWidget>
#include <QLabel>
#include "Hymn.h"

class QListWidget;
class QPushButton;
class HymnDelegate;

class DirectoryPage : public QWidget
{
    Q_OBJECT

public:
    explicit DirectoryPage(QWidget *parent = nullptr);
    ~DirectoryPage() override = default;

    /// 从 HymnManager 重新加载目录
    void refresh();

signals:
    void hymnSelected(int hymnIndex);
    void aboutRequested();
    void addHymnRequested();
    void settingsRequested();

private:
    void setupUI();
    void batchImport();

    // 批量删除
    void toggleSelectMode();
    void selectAll();
    void invertSelection();
    void deleteSelected();
    int selectedCount() const;
    void updateDeleteButtonText();

    QListWidget *m_listWidget;
    QLabel *m_emptyLabel;
    HymnDelegate *m_delegate = nullptr;

    // 批量删除 UI
    QPushButton *m_batchDeleteBtn = nullptr;
    QPushButton *m_selectAllBtn = nullptr;
    QPushButton *m_invertSelectBtn = nullptr;
    QPushButton *m_deleteSelectedBtn = nullptr;
    QPushButton *m_cancelSelectBtn = nullptr;
    QPushButton *m_addBtn = nullptr;
    QPushButton *m_batchBtn = nullptr;
    QPushButton *m_settingsBtn = nullptr;
    QPushButton *m_aboutBtn = nullptr;
    bool m_selectMode = false;
};

#endif // DIRECTORYPAGE_H
