// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef DIRECTORYPAGE_H
#define DIRECTORYPAGE_H

#include <QWidget>
#include <QLabel>
#include "Hymn.h"

class QListWidget;

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

private:
    void setupUI();
    void batchImport();

    QListWidget *m_listWidget;
    QLabel *m_emptyLabel;
};

#endif // DIRECTORYPAGE_H
