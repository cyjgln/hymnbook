// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef ZOOMABLEGRAPHICSVIEW_H
#define ZOOMABLEGRAPHICSVIEW_H

#include <QGraphicsView>

class QLabel;

/// 支持滚轮缩放 + 拖动的图片显示控件
class ZoomableGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ZoomableGraphicsView(QWidget *parent = nullptr);

    double zoomFactor() const { return m_zoomFactor; }

public slots:
    void zoomIn();
    void zoomOut();
    void zoomReset();

signals:
    void zoomChanged(double factor);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    void applyZoom(double factor);

    double m_zoomFactor = 1.0;
    double m_minZoom = 0.1;
    double m_maxZoom = 10.0;
    double m_zoomStep = 1.15;
};

#endif // ZOOMABLEGRAPHICSVIEW_H
