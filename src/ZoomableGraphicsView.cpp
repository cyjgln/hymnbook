// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#include "ZoomableGraphicsView.h"

#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <cmath>

ZoomableGraphicsView::ZoomableGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFrameShape(QFrame::NoFrame);
    setInteractive(true);
    // 优化大图片性能
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
}

void ZoomableGraphicsView::zoomIn()
{
    applyZoom(m_zoomFactor * m_zoomStep);
}

void ZoomableGraphicsView::zoomOut()
{
    applyZoom(m_zoomFactor / m_zoomStep);
}

void ZoomableGraphicsView::zoomReset()
{
    m_zoomFactor = 1.0;
    resetTransform();
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    // 计算实际比例
    if (!sceneRect().isEmpty() && viewport()->width() > 0) {
        double scaleX = viewport()->width() / sceneRect().width();
        double scaleY = viewport()->height() / sceneRect().height();
        m_zoomFactor = std::min(scaleX, scaleY);
    }
    emit zoomChanged(m_zoomFactor);
}

void ZoomableGraphicsView::wheelEvent(QWheelEvent *event)
{
    // 鼠标滚轮缩放
    double delta = event->angleDelta().y();
    if (delta > 0) {
        zoomIn();
    } else if (delta < 0) {
        zoomOut();
    }
    event->accept();
}

void ZoomableGraphicsView::zoomFitToWidth()
{
    m_zoomFactor = 1.0;
    resetTransform();
    QRectF r = sceneRect();
    if (r.isEmpty() || viewport()->width() == 0)
        return;
    double margin = 12;
    double factor = (viewport()->width() - 2 * margin) / r.width();
    factor = std::clamp(factor, m_minZoom, m_maxZoom);
    m_zoomFactor = factor;
    QGraphicsView::scale(factor, factor);
    centerOn(r.center());
    emit zoomChanged(m_zoomFactor);
}

void ZoomableGraphicsView::zoomFitToHeight()
{
    m_zoomFactor = 1.0;
    resetTransform();
    QRectF r = sceneRect();
    if (r.isEmpty() || viewport()->height() == 0)
        return;
    double margin = 12;
    double scaleH = (viewport()->height() - 2 * margin) / r.height();
    double scaleW = (viewport()->width() - 2 * margin) / r.width();
    double factor = scaleH;  // 优先撑高
    if (r.width() * factor > viewport()->width() - 2 * margin)
        factor = scaleW;     // 宽度不能超出
    factor = std::clamp(factor, m_minZoom, m_maxZoom);
    m_zoomFactor = factor;
    QGraphicsView::scale(factor, factor);
    centerOn(r.center());
    emit zoomChanged(m_zoomFactor);
}

void ZoomableGraphicsView::applyZoom(double newFactor)
{
    // 限制缩放范围
    newFactor = std::clamp(newFactor, m_minZoom, m_maxZoom);

    if (std::abs(newFactor - m_zoomFactor) < 0.001)
        return;

    double ratio = newFactor / m_zoomFactor;
    m_zoomFactor = newFactor;
    scale(ratio, ratio);
    emit zoomChanged(m_zoomFactor);
}
