// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef HYMNDELEGATE_H
#define HYMNDELEGATE_H

#include <QStyledItemDelegate>

class HymnDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit HymnDelegate(QObject *parent = nullptr);

    void setSelectMode(bool selectMode);
    bool isSelectMode() const;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

private:
    bool m_selectMode = false;
};

#endif // HYMNDELEGATE_H
