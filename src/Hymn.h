// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef HYMN_H
#define HYMN_H

#include <QString>
#include <QJsonObject>

struct Hymn
{
    int id = 0;
    QString title;
    QString imagePath;  // 相对于可执行文件所在目录的路径
    int pageNumber = 0;

    // 从 JSON 对象反序列化
    static Hymn fromJson(const QJsonObject &obj)
    {
        Hymn h;
        h.id = obj.value(QStringLiteral("id")).toInt();
        h.title = obj.value(QStringLiteral("title")).toString();
        h.imagePath = obj.value(QStringLiteral("imagePath")).toString();
        h.pageNumber = obj.value(QStringLiteral("pageNumber")).toInt();
        return h;
    }

    // 序列化为 JSON 对象
    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj[QStringLiteral("id")] = id;
        obj[QStringLiteral("title")] = title;
        obj[QStringLiteral("imagePath")] = imagePath;
        obj[QStringLiteral("pageNumber")] = pageNumber;
        return obj;
    }
};

#endif // HYMN_H
