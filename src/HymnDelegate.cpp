#include "HymnDelegate.h"

#include <QPainter>
#include <QApplication>

HymnDelegate::HymnDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void HymnDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QRect r = option.rect.adjusted(4, 4, -4, -4);

    // --- 背景 ---
    QColor bgColor(0xff, 0xff, 0xff);         // white
    QColor borderColor(0xe0, 0xd5, 0xc8);     // light tan

    if (option.state & QStyle::State_Selected) {
        bgColor = QColor(0xf5, 0xef, 0xe8);       // warm cream
        borderColor = QColor(0xc9, 0xa9, 0x6e);   // gold
    } else if (option.state & QStyle::State_MouseOver) {
        bgColor = QColor(0xfa, 0xf6, 0xf0);       // lighter cream
        borderColor = QColor(0xc9, 0xa9, 0x6e);   // gold
    }

    // rounded rect background
    painter->setBrush(bgColor);
    painter->setPen(QPen(borderColor, 1));
    painter->drawRoundedRect(r, 8, 8);

    // selected: gold left border accent
    if (option.state & QStyle::State_Selected) {
        QRect accent = r.adjusted(0, 0, 0, 0);
        accent.setWidth(4);
        painter->setBrush(QColor(0xc9, 0xa9, 0x6e));
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(accent, 8, 8);
        painter->setBrush(bgColor);
        painter->setPen(QPen(borderColor, 1));
        painter->drawRoundedRect(r.adjusted(0, 0, -r.width() + 4, 0), 8, 8);
    }

    // --- 文字 ---
    QString text = index.data(Qt::DisplayRole).toString();
    if (text.isEmpty()) {
        painter->restore();
        return;
    }

    QRect textRect = r.adjusted(14, 8, -14, -8);
    QFont font = option.font;
    font.setPointSize(12);
    painter->setFont(font);
    painter->setPen(QColor(0x3a, 0x2a, 0x1a));  // dark brown

    // 手动按字符逐行绘制，实现可靠换行
    QFontMetrics fm(font);
    QStringList lines;
    QString currentLine;
    int maxWidth = textRect.width();

    for (int i = 0; i < text.size(); ++i) {
        QChar ch = text[i];
        // 检测是否该换行：在空格处分词（英文数字场景）
        if (ch == ' ' && !currentLine.isEmpty()) {
            QString testLine = currentLine + ch;
            int testWidth = fm.horizontalAdvance(testLine);
            if (testWidth > maxWidth) {
                lines.append(currentLine.trimmed());
                currentLine.clear();
            } else {
                currentLine += ch;
            }
        } else {
            currentLine += ch;
            int lineWidth = fm.horizontalAdvance(currentLine);
            if (lineWidth > maxWidth) {
                // 去掉最后一个字符换行
                currentLine.chop(1);
                if (currentLine.isEmpty()) {
                    // 单个字符也超宽，强制画一个
                    currentLine = ch;
                }
                lines.append(currentLine);
                currentLine = ch;
            }
        }
    }
    if (!currentLine.isEmpty())
        lines.append(currentLine);

    // 绘制每一行
    int lineHeight = fm.height();
    int y = textRect.y();
    for (int i = 0; i < lines.size(); ++i) {
        if (y + lineHeight > textRect.bottom() + 4)
            break;  // 超出区域不再绘制
        painter->drawText(textRect.x(), y, textRect.width(), lineHeight,
                          Qt::AlignLeft | Qt::AlignVCenter, lines[i]);
        y += lineHeight;
    }

    painter->restore();
}

QSize HymnDelegate::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    // 在 IconMode 下 sizeHint 被 gridSize 覆盖，这里返回默认值
    return QSize(400, 90);
}
