#ifndef HYMNDELEGATE_H
#define HYMNDELEGATE_H

#include <QStyledItemDelegate>

class HymnDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit HymnDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};

#endif // HYMNDELEGATE_H
