// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

class QLineEdit;
class QListWidget;
class QKeyEvent;
class QShowEvent;

/// 搜索弹窗：Ctrl+F 触发，实时模糊搜索歌名
class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    ~SearchDialog() override = default;

signals:
    /// 用户选中搜索结果，传递 hymnIndex（在 HymnManager 中的索引）
    void hymnSelected(int hymnIndex);

private:
    void setupUI();
    void performSearch(const QString &keyword);

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void showEvent(QShowEvent *event) override;

    QLineEdit *m_searchInput;
    QListWidget *m_resultList;
};

#endif // SEARCHDIALOG_H
