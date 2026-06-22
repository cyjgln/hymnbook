// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef HYMNMANAGER_H
#define HYMNMANAGER_H

#include <QObject>
#include <QList>
#include "Hymn.h"

class QString;

class HymnManager : public QObject
{
    Q_OBJECT

public:
    static HymnManager &instance();

    // 加载 / 保存
    bool load(const QString &filePath);
    bool save();

    // 查询
    QList<Hymn> allHymns() const;
    Hymn hymnById(int id) const;
    Hymn hymnByIndex(int index) const;  // 按列表顺序索引
    int count() const;

    // 搜索（按歌名模糊匹配，大小写不敏感）
    QList<Hymn> search(const QString &keyword) const;

    // 新增
    int addHymn(const Hymn &hymn);
    // 批量新增（只发射一次 dataChanged）
    void batchAddHymns(const QList<Hymn> &hymns);
    // 修改
    bool updateHymn(const Hymn &hymn);
    // 删除
    bool deleteHymn(int id);
    // 批量删除（只发射一次 dataChanged）
    void deleteHymns(const QList<int> &ids);

    // 数据文件路径
    QString dataFilePath() const { return m_filePath; }

signals:
    void dataChanged();

private:
    HymnManager() = default;
    ~HymnManager() override = default;
    HymnManager(const HymnManager &) = delete;
    HymnManager &operator=(const HymnManager &) = delete;

    void regeneratePageNumbers();

    QString m_filePath;
    QList<Hymn> m_hymns;
    int m_nextId = 1;
};

#endif // HYMNMANAGER_H
