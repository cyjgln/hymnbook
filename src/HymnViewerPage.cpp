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
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QFileInfo>
#include <QDir>
#include <cmath>

HymnViewerPage::HymnViewerPage(QWidget *parent)
    : QWidget(parent), m_currentIndex(0)
{
    setFocusPolicy(Qt::StrongFocus);
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

    layout->addWidget(toolbar);

    // 图片显示区域
    m_scene = new QGraphicsScene(this);
    m_graphicsView = new ZoomableGraphicsView(this);
    m_graphicsView->setScene(m_scene);
    layout->addWidget(m_graphicsView, 1);

    // 监听缩放变化，更新显示
    connect(m_graphicsView, &ZoomableGraphicsView::zoomChanged, this, [this](double factor) {
        m_zoomLabel->setText(QStringLiteral("%1%").arg(static_cast<int>(std::round(factor * 100))));
    });

    // 底部歌名和页码
    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    layout->addWidget(m_titleLabel);

    m_pageLabel = new QLabel(this);
    m_pageLabel->setAlignment(Qt::AlignCenter);
    QFont pageFont = m_pageLabel->font();
    pageFont.setPointSize(11);
    m_pageLabel->setFont(pageFont);
    layout->addWidget(m_pageLabel);

    // 默认无图片状态
    loadHymn(0);
}

void HymnViewerPage::loadHymn(int index)
{
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
            m_graphicsView->zoomReset();
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
    default:
        break;
    }

    QWidget::keyPressEvent(event);
}
