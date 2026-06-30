// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef HYMNVIEWERPAGE_H
#define HYMNVIEWERPAGE_H

#include <QWidget>
#include "Hymn.h"

class ZoomableGraphicsView;
class QGraphicsScene;
class QKeyEvent;
class QLabel;
class QPushButton;
class QShortcut;
class QTimer;
class HymnViewerPage : public QWidget
{
    Q_OBJECT

public:
    explicit HymnViewerPage(QWidget *parent = nullptr);
    ~HymnViewerPage() override = default;

    void loadHymn(int index);
    int currentIndex() const { return m_currentIndex; }

signals:
    void previousRequested();
    void nextRequested();
    void directoryRequested();
    void editHymnRequested(int hymnIndex);
    void searchRequested();

public slots:
    void zoomIn();
    void zoomOut();

public slots:
    void applyDisplayMode();
    void startAutoScroll();
    void stopAutoScroll();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void pauseAutoScroll();
    void resumeAutoScroll();
    void onAutoScrollTick();
    void recalcAutoScrollSpeed();
    void checkAutoScrollStart();
    void updateSpeedDisplay();

    ZoomableGraphicsView *m_graphicsView;
    QGraphicsScene *m_scene;
    QLabel *m_titleLabel;
    QLabel *m_pageLabel;
    QLabel *m_zoomLabel;
    QLabel *m_speedLabel = nullptr;
    QTimer *m_autoScrollTimer = nullptr;
    QPushButton *m_autoScrollBtn = nullptr;
    QPushButton *m_speedUpBtn = nullptr;
    QPushButton *m_speedDownBtn = nullptr;
    QShortcut *m_shortcutLeft = nullptr;
    QShortcut *m_shortcutRight = nullptr;
    QShortcut *m_shortcutZoomIn = nullptr;
    QShortcut *m_shortcutZoomOut = nullptr;
    QShortcut *m_shortcutSpace = nullptr;
    QShortcut *m_shortcutSpeedUp = nullptr;
    QShortcut *m_shortcutSpeedDown = nullptr;
    int m_currentIndex;
    int m_autoScrollSpeed = 3;
    int m_autoScrollPixelsPerTick = 0;
    bool m_autoScrollActive = false;
    bool m_autoScrollPaused = false;
};

#endif // HYMNVIEWERPAGE_H
