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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUI();

    ZoomableGraphicsView *m_graphicsView;
    QGraphicsScene *m_scene;
    QLabel *m_titleLabel;
    QLabel *m_pageLabel;
    QLabel *m_zoomLabel;
    int m_currentIndex;
};

#endif // HYMNVIEWERPAGE_H
