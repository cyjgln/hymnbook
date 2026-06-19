// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include "Hymn.h"

class QLineEdit;
class QLabel;

/// 新增/编辑歌谱弹窗
class EditDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { AddMode, EditMode };

    explicit EditDialog(Mode mode, const Hymn &hymn = Hymn(), QWidget *parent = nullptr);
    ~EditDialog() override = default;

    /// 获取编辑后的数据（在对话框 accepted 后调用）
    Hymn resultHymn() const { return m_result; }

signals:
    /// 删除请求（仅在 EditMode 下发射）
    void deleteRequested(int hymnId);

private:
    void setupUI();
    void browseImage();
    bool saveHymn();
    void confirmDelete();

    Mode m_mode;
    Hymn m_originalHymn;  // 编辑模式下的原始数据
    Hymn m_result;
    QString m_selectedImagePath;  // 用户选择的图片原始路径

    QLineEdit *m_titleInput;
    QLineEdit *m_imagePathInput;
    QLabel *m_previewLabel;
};

#endif // EDITDIALOG_H
