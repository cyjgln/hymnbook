// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#include "HymnViewerPage.h"
#include "ZoomableGraphicsView.h"
#include "HymnManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsScene>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>
#include <QToolBar>
#include <QFont>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QResizeEvent>
#include <QShortcut>
#include <QTimer>
#include <QScrollBar>
#include <QMouseEvent>
#include <cmath>

HymnViewerPage::HymnViewerPage(QWidget *parent)
    : QWidget(parent), m_currentIndex(0)
{
    setFocusPolicy(Qt::StrongFocus);

    m_autoScrollTimer = new QTimer(this);
    m_autoScrollTimer->setInterval(50);  // 20 fps
    connect(m_autoScrollTimer, &QTimer::timeout, this, &HymnViewerPage::onAutoScrollTick);

    setupUI();
}

void HymnViewerPage::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 顶部导航栏
    auto *toolbar = new QToolBar(this);
    toolbar->setMovable(false);

    auto *backBtn = new QPushButton(QStringLiteral("← 返回目录"), this);
    backBtn->setObjectName(QStringLiteral("secondaryBtn"));
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &HymnViewerPage::directoryRequested);
    toolbar->addWidget(backBtn);

    auto *prevBtn = new QPushButton(QStringLiteral("上一首"), this);
    connect(prevBtn, &QPushButton::clicked, this, &HymnViewerPage::previousRequested);
    toolbar->addWidget(prevBtn);

    auto *nextBtn = new QPushButton(QStringLiteral("下一首"), this);
    connect(nextBtn, &QPushButton::clicked, this, &HymnViewerPage::nextRequested);
    toolbar->addWidget(nextBtn);

    toolbar->addSeparator();

    auto *searchBtn = new QPushButton(QStringLiteral("搜索"), this);
    connect(searchBtn, &QPushButton::clicked, this, &HymnViewerPage::searchRequested);
    toolbar->addWidget(searchBtn);

    toolbar->addSeparator();

    auto *editBtn = new QPushButton(QStringLiteral("编辑"), this);
    connect(editBtn, &QPushButton::clicked, this, [this]() {
        emit editHymnRequested(m_currentIndex);
    });
    toolbar->addWidget(editBtn);

    toolbar->addSeparator();

    // 最大化（横向铺满）
    auto *maximizeBtn = new QPushButton(QStringLiteral("最大化"), this);
    maximizeBtn->setCursor(Qt::PointingHandCursor);
    connect(maximizeBtn, &QPushButton::clicked, this, [this]() {
        m_graphicsView->zoomFitToWidth();
    });
    toolbar->addWidget(maximizeBtn);

    // 缩放按钮
    auto *zoomInBtn = new QPushButton(QStringLiteral("放大"), this);
    connect(zoomInBtn, &QPushButton::clicked, this, &HymnViewerPage::zoomIn);
    toolbar->addWidget(zoomInBtn);

    auto *zoomOutBtn = new QPushButton(QStringLiteral("缩小"), this);
    connect(zoomOutBtn, &QPushButton::clicked, this, &HymnViewerPage::zoomOut);
    toolbar->addWidget(zoomOutBtn);

    m_zoomLabel = new QLabel(QStringLiteral("100%"), this);
    m_zoomLabel->setMinimumWidth(50);
    m_zoomLabel->setAlignment(Qt::AlignCenter);
    toolbar->addWidget(m_zoomLabel);

    toolbar->addSeparator();

    // 自动滚动按钮
    m_autoScrollBtn = new QPushButton(QStringLiteral("▶ 自动滚动"), this);
    m_autoScrollBtn->setCheckable(true);
    m_autoScrollBtn->setCursor(Qt::PointingHandCursor);
    connect(m_autoScrollBtn, &QPushButton::clicked, this, [this]() {
        if (m_autoScrollActive) {
            stopAutoScroll();
        } else {
            startAutoScroll();
        }
    });
    toolbar->addWidget(m_autoScrollBtn);

    // 滚动速度调节（上下三角图标按钮）
    auto *speedLabelTitle = new QLabel(QStringLiteral("速度:"), this);
    toolbar->addWidget(speedLabelTitle);

    // 用 QPainter 绘制三角图标，避免 Unicode 字符依赖系统字体导致显示异常
    auto makeTriangleIcon = [](bool pointingUp) -> QIcon {
        QPixmap pix(16, 16);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QColor(QStringLiteral("#5a4a3a")));
        painter.setPen(Qt::NoPen);
        QPolygonF triangle;
        if (pointingUp) {
            triangle << QPointF(8, 2) << QPointF(2, 13) << QPointF(14, 13);
        } else {
            triangle << QPointF(2, 3) << QPointF(14, 3) << QPointF(8, 14);
        }
        painter.drawPolygon(triangle);
        painter.end();
        return QIcon(pix);
    };

    m_speedDownBtn = new QPushButton(this);
    m_speedDownBtn->setIcon(makeTriangleIcon(false));  // 倒三角 = 减速
    m_speedDownBtn->setIconSize(QSize(14, 14));
    m_speedDownBtn->setFixedWidth(32);
    m_speedDownBtn->setCursor(Qt::PointingHandCursor);
    m_speedDownBtn->setToolTip(QStringLiteral("降低滚动速度 (Ctrl+↓)"));
    connect(m_speedDownBtn, &QPushButton::clicked, this, [this]() {
        if (m_autoScrollSpeed > 1) {
            m_autoScrollSpeed--;
            updateSpeedDisplay();
            if (m_autoScrollActive && !m_autoScrollPaused)
                recalcAutoScrollSpeed();
        }
    });
    toolbar->addWidget(m_speedDownBtn);

    m_speedLabel = new QLabel(QString::number(m_autoScrollSpeed), this);
    m_speedLabel->setMinimumWidth(20);
    m_speedLabel->setAlignment(Qt::AlignCenter);
    m_speedLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 14px;"));
    toolbar->addWidget(m_speedLabel);

    m_speedUpBtn = new QPushButton(this);
    m_speedUpBtn->setIcon(makeTriangleIcon(true));  // 正三角 = 加速
    m_speedUpBtn->setIconSize(QSize(14, 14));
    m_speedUpBtn->setFixedWidth(32);
    m_speedUpBtn->setCursor(Qt::PointingHandCursor);
    m_speedUpBtn->setToolTip(QStringLiteral("增加滚动速度 (Ctrl+↑)"));
    connect(m_speedUpBtn, &QPushButton::clicked, this, [this]() {
        if (m_autoScrollSpeed < 5) {
            m_autoScrollSpeed++;
            updateSpeedDisplay();
            if (m_autoScrollActive && !m_autoScrollPaused)
                recalcAutoScrollSpeed();
        }
    });
    toolbar->addWidget(m_speedUpBtn);

    // ---- 快捷键（QShortcut 确保焦点在任意子控件时也能响应） ----
    m_shortcutLeft = new QShortcut(QKeySequence(Qt::Key_Left), this);
    m_shortcutLeft->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_shortcutLeft, &QShortcut::activated, this, &HymnViewerPage::previousRequested);

    m_shortcutRight = new QShortcut(QKeySequence(Qt::Key_Right), this);
    m_shortcutRight->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_shortcutRight, &QShortcut::activated, this, &HymnViewerPage::nextRequested);

    m_shortcutZoomIn = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus), this);
    m_shortcutZoomIn->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_shortcutZoomIn, &QShortcut::activated, this, &HymnViewerPage::zoomIn);
    // 兼容小键盘 + 和 = 键
    auto *shortcutZoomIn2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Equal), this);
    shortcutZoomIn2->setContext(Qt::WidgetWithChildrenShortcut);
    connect(shortcutZoomIn2, &QShortcut::activated, this, &HymnViewerPage::zoomIn);

    m_shortcutZoomOut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus), this);
    m_shortcutZoomOut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_shortcutZoomOut, &QShortcut::activated, this, &HymnViewerPage::zoomOut);

    m_shortcutSpace = new QShortcut(QKeySequence(Qt::Key_Space), this);
    m_shortcutSpace->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_shortcutSpace, &QShortcut::activated, this, [this]() {
        if (!m_autoScrollActive) {
            startAutoScroll();
        } else if (!m_autoScrollPaused) {
            pauseAutoScroll();
        } else {
            resumeAutoScroll();
        }
    });

    m_shortcutSpeedUp = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Up), this);
    m_shortcutSpeedUp->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_shortcutSpeedUp, &QShortcut::activated, this, [this]() {
        if (m_autoScrollSpeed < 5) {
            m_autoScrollSpeed++;
            updateSpeedDisplay();
            if (m_autoScrollActive && !m_autoScrollPaused)
                recalcAutoScrollSpeed();
        }
    });

    m_shortcutSpeedDown = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Down), this);
    m_shortcutSpeedDown->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_shortcutSpeedDown, &QShortcut::activated, this, [this]() {
        if (m_autoScrollSpeed > 1) {
            m_autoScrollSpeed--;
            updateSpeedDisplay();
            if (m_autoScrollActive && !m_autoScrollPaused)
                recalcAutoScrollSpeed();
        }
    });

    // 0 键回到歌谱顶部
    m_shortcutKey0 = new QShortcut(QKeySequence(Qt::Key_0), this);
    m_shortcutKey0->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_shortcutKey0, &QShortcut::activated, this, &HymnViewerPage::scrollToTop);

    layout->addWidget(toolbar);

    // 图片显示区域
    m_scene = new QGraphicsScene(this);
    m_graphicsView = new ZoomableGraphicsView(this);
    m_graphicsView->setScene(m_scene);
    m_graphicsView->viewport()->installEventFilter(this);
    layout->addWidget(m_graphicsView, 1);

    // 监听缩放变化，更新显示
    connect(m_graphicsView, &ZoomableGraphicsView::zoomChanged, this, [this](double factor) {
        m_zoomLabel->setText(QStringLiteral("%1%").arg(static_cast<int>(std::round(factor * 100))));
    });

    // 底部歌名和页码
    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setObjectName(QStringLiteral("hymnTitle"));
    m_titleLabel->setStyleSheet(QStringLiteral(
        "QLabel#hymnTitle {"
        "  color: #3a2a1a;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  padding: 6px 0 2px 0;"
        "}"
    ));
    layout->addWidget(m_titleLabel);

    m_pageLabel = new QLabel(this);
    m_pageLabel->setAlignment(Qt::AlignCenter);
    m_pageLabel->setObjectName(QStringLiteral("hymnPage"));
    m_pageLabel->setStyleSheet(QStringLiteral(
        "QLabel#hymnPage {"
        "  color: #8a7a6a;"
        "  font-size: 12px;"
        "  padding: 0 0 8px 0;"
        "}"
    ));
    layout->addWidget(m_pageLabel);

    // 默认无图片状态
    loadHymn(0);
}

void HymnViewerPage::loadHymn(int index)
{
    // 切换歌谱时停止自动滚动
    if (m_autoScrollActive)
        stopAutoScroll();

    m_currentIndex = index;
    int total = HymnManager::instance().count();

    if (index < 0 || index >= total) {
        m_scene->clear();
        m_titleLabel->clear();
        m_pageLabel->clear();
        return;
    }

    Hymn hymn = HymnManager::instance().hymnByIndex(index);
    m_titleLabel->setText(hymn.title);
    m_pageLabel->setText(QStringLiteral("%1 / %2").arg(index + 1).arg(total));

    // 显示图片
    m_scene->clear();
    if (!hymn.imagePath.isEmpty()) {
        // 解析图片路径：images/ 目录与 data/ 目录平级
        // dataDir = .../data/, imagePath = images/xxx.png
        // → 完整路径 = .../images/xxx.png
        QString dataDir = QFileInfo(HymnManager::instance().dataFilePath()).absolutePath();
        QString fullPath = QDir(dataDir).absoluteFilePath(QStringLiteral("../") + hymn.imagePath);
        fullPath = QDir::cleanPath(fullPath);
        QPixmap pixmap(fullPath);
        if (!pixmap.isNull()) {
            auto *item = m_scene->addPixmap(pixmap);
            // 明确设置场景矩形为图片实际尺寸，确保 fitInView 能正确计算缩放比
            m_scene->setSceneRect(item->boundingRect());
            applyDisplayMode();
            checkAutoScrollStart();
            return;
        }
    }

    // 无图片时显示提示
    m_scene->addText(QStringLiteral("无歌谱图片\n请点击「编辑」添加"));
    m_graphicsView->zoomReset();

    // 获取键盘焦点，以便响应快捷键
    setFocus();
}

void HymnViewerPage::zoomIn()
{
    m_graphicsView->zoomIn();
}

void HymnViewerPage::zoomOut()
{
    m_graphicsView->zoomOut();
}

void HymnViewerPage::setBottomBarVisible(bool visible)
{
    m_titleLabel->setVisible(visible);
    m_pageLabel->setVisible(visible);
}

void HymnViewerPage::scrollToTop()
{
    QScrollBar *vbar = m_graphicsView->verticalScrollBar();
    if (vbar)
        vbar->setValue(0);
}

void HymnViewerPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    applyDisplayMode();
}

void HymnViewerPage::applyDisplayMode()
{
    if (m_scene->items().isEmpty())
        return;

    QSettings settings;
    int mode = settings.value(QStringLiteral("display/mode"), 1).toInt();
    switch (mode) {
    case 1:
        m_graphicsView->zoomFitToWidth();
        break;
    default:
        m_graphicsView->zoomReset();  // 全页显示
        break;
    }
}

void HymnViewerPage::recalcAutoScrollSpeed()
{
    QScrollBar *vbar = m_graphicsView->verticalScrollBar();
    if (!vbar || vbar->maximum() <= 0) {
        m_autoScrollPixelsPerTick = 0;
        return;
    }

    int totalScroll = vbar->maximum();
    const double durations[] = {60.0, 40.0, 30.0, 20.0, 12.0};
    double duration = durations[m_autoScrollSpeed - 1];
    int ticks = static_cast<int>(duration * 20);  // 20 ticks/s @ 50ms
    m_autoScrollPixelsPerTick = std::max(1, totalScroll / ticks);
}

void HymnViewerPage::startAutoScroll()
{
    if (m_autoScrollActive)
        return;

    QScrollBar *vbar = m_graphicsView->verticalScrollBar();
    if (!vbar || vbar->maximum() <= 0)
        return;  // 无需滚动

    // 如果已经滚到底部，回到顶部重新开始
    if (vbar->value() >= vbar->maximum())
        vbar->setValue(0);

    m_autoScrollActive = true;
    m_autoScrollPaused = false;
    m_autoScrollBtn->setText(QStringLiteral("⏸ 停止滚动"));
    m_autoScrollBtn->setChecked(true);

    recalcAutoScrollSpeed();
    m_autoScrollTimer->start();
}

void HymnViewerPage::stopAutoScroll()
{
    if (!m_autoScrollActive)
        return;

    m_autoScrollActive = false;
    m_autoScrollPaused = false;
    m_autoScrollTimer->stop();
    m_autoScrollBtn->setText(QStringLiteral("▶ 自动滚动"));
    m_autoScrollBtn->setChecked(false);
}

void HymnViewerPage::pauseAutoScroll()
{
    if (!m_autoScrollActive || m_autoScrollPaused)
        return;

    m_autoScrollPaused = true;
    m_autoScrollTimer->stop();
}

void HymnViewerPage::resumeAutoScroll()
{
    if (!m_autoScrollActive || !m_autoScrollPaused)
        return;

    m_autoScrollPaused = false;
    recalcAutoScrollSpeed();
    m_autoScrollTimer->start();
}

void HymnViewerPage::onAutoScrollTick()
{
    QScrollBar *vbar = m_graphicsView->verticalScrollBar();
    if (!vbar || vbar->maximum() <= 0) {
        stopAutoScroll();
        return;
    }

    int newVal = vbar->value() + m_autoScrollPixelsPerTick;
    if (newVal >= vbar->maximum()) {
        vbar->setValue(vbar->maximum());
        stopAutoScroll();  // 滚动到底部，结束
    } else {
        vbar->setValue(newVal);
    }
}

void HymnViewerPage::updateSpeedDisplay()
{
    if (m_speedLabel)
        m_speedLabel->setText(QString::number(m_autoScrollSpeed));
}

void HymnViewerPage::checkAutoScrollStart()
{
    QSettings settings;
    bool autoScrollEnabled = settings.value(QStringLiteral("display/autoScroll"), false).toBool();
    if (autoScrollEnabled) {
        startAutoScroll();
    }
}

bool HymnViewerPage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_graphicsView->viewport()) {
        if (event->type() == QEvent::MouseButtonPress) {
            // 用户手动操作 → 暂停自动滚动
            if (m_autoScrollActive && !m_autoScrollPaused)
                pauseAutoScroll();
        } else if (event->type() == QEvent::MouseButtonRelease) {
            // 用户停止操作 → 恢复自动滚动
            if (m_autoScrollActive && m_autoScrollPaused)
                resumeAutoScroll();
        }
    }
    return QWidget::eventFilter(obj, event);
}

void HymnViewerPage::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        emit previousRequested();
        return;
    case Qt::Key_Right:
        emit nextRequested();
        return;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        if (event->modifiers() & Qt::ControlModifier) {
            zoomIn();
            return;
        }
        break;
    case Qt::Key_Minus:
        if (event->modifiers() & Qt::ControlModifier) {
            zoomOut();
            return;
        }
        break;
    case Qt::Key_Space:
        // 空格键：启动/暂停/继续自动滚动
        if (!m_autoScrollActive) {
            startAutoScroll();
        } else if (!m_autoScrollPaused) {
            pauseAutoScroll();
        } else {
            resumeAutoScroll();
        }
        return;
    case Qt::Key_Up:
        if (event->modifiers() & Qt::ControlModifier) {
            // Ctrl+上：增加滚动速度
            if (m_autoScrollSpeed < 5) {
                m_autoScrollSpeed++;
                updateSpeedDisplay();
                if (m_autoScrollActive && !m_autoScrollPaused)
                    recalcAutoScrollSpeed();
            }
            return;
        }
        break;
    case Qt::Key_Down:
        if (event->modifiers() & Qt::ControlModifier) {
            // Ctrl+下：降低滚动速度
            if (m_autoScrollSpeed > 1) {
                m_autoScrollSpeed--;
                updateSpeedDisplay();
                if (m_autoScrollActive && !m_autoScrollPaused)
                    recalcAutoScrollSpeed();
            }
            return;
        }
        break;
    default:
        break;
    }

    QWidget::keyPressEvent(event);
}
