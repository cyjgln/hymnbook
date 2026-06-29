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
    // 鼠标滚轮上下滚动歌谱
    QScrollBar *vbar = verticalScrollBar();
    if (vbar) {
        int delta = event->angleDelta().y();
        int numSteps = delta / 120;
        vbar->setValue(vbar->value() - numSteps * vbar->singleStep() * 3);
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
    // 定位到歌谱上端
    verticalScrollBar()->setValue(0);
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
