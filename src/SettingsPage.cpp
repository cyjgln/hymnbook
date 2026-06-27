// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#include "SettingsPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QSettings>

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadSettings();
}

void SettingsPage::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 20, 30, 20);

    // 返回按钮
    auto *backBtn = new QPushButton(QStringLiteral("← 返回目录"), this);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &SettingsPage::directoryRequested);
    layout->addWidget(backBtn);

    // 标题
    auto *titleLabel = new QLabel(QStringLiteral("设置"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName(QStringLiteral("settingsTitle"));
    titleLabel->setStyleSheet(QStringLiteral(
        "QLabel#settingsTitle {"
        "  color: #5a4a3a;"
        "  font-size: 24px;"
        "  font-weight: bold;"
        "  padding: 8px 0 16px 0;"
        "}"
    ));
    layout->addWidget(titleLabel);

    // ---- 歌谱显示分组 ----
    m_displayGroup = new QGroupBox(QStringLiteral("歌谱显示"), this);
    m_displayGroup->setStyleSheet(QStringLiteral(
        "QGroupBox {"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  color: #5a4a3a;"
        "  border: 1px solid #e0d5c8;"
        "  border-radius: 8px;"
        "  margin-top: 16px;"
        "  padding: 20px 16px 16px 16px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 16px;"
        "  padding: 0 8px;"
        "}"
    ));

    auto *groupLayout = new QVBoxLayout(m_displayGroup);
    groupLayout->setSpacing(12);

    m_displayButtonGroup = new QButtonGroup(this);

    // 全页显示
    auto *radioFull = new QRadioButton(QStringLiteral("全页显示"), this);
    m_displayButtonGroup->addButton(radioFull, 0);
    groupLayout->addWidget(radioFull);
    auto *descFull = new QLabel(QStringLiteral("整张歌谱完整显示在窗口中，适合浏览翻阅"), this);
    descFull->setStyleSheet(QStringLiteral("color: #8a7a6a; font-size: 12px; font-weight: normal; padding-left: 24px;"));
    descFull->setWordWrap(true);
    groupLayout->addWidget(descFull);

    // 横向最大化
    auto *radioWidth = new QRadioButton(QStringLiteral("横向最大化"), this);
    m_displayButtonGroup->addButton(radioWidth, 1);
    groupLayout->addWidget(radioWidth);
    auto *descWidth = new QLabel(QStringLiteral("图片横向铺满窗口，适合竖版歌谱，高度超出可滚动"), this);
    descWidth->setStyleSheet(QStringLiteral("color: #8a7a6a; font-size: 12px; font-weight: normal; padding-left: 24px;"));
    descWidth->setWordWrap(true);
    groupLayout->addWidget(descWidth);

    layout->addWidget(m_displayGroup);
    layout->addSpacing(8);

    // ---- 自动滚动分组 ----
    auto *scrollGroup = new QGroupBox(QStringLiteral("自动滚动"), this);
    scrollGroup->setStyleSheet(QStringLiteral(
        "QGroupBox {"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  color: #5a4a3a;"
        "  border: 1px solid #e0d5c8;"
        "  border-radius: 8px;"
        "  margin-top: 16px;"
        "  padding: 20px 16px 16px 16px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 16px;"
        "  padding: 0 8px;"
        "}"
    ));

    auto *scrollLayout = new QVBoxLayout(scrollGroup);
    m_autoScrollCheck = new QCheckBox(QStringLiteral("进入歌谱时自动开始滚动"), this);
    m_autoScrollCheck->setStyleSheet(QStringLiteral(
        "QCheckBox { font-size: 14px; font-weight: normal; color: #5a4a3a; }"
    ));
    scrollLayout->addWidget(m_autoScrollCheck);
    auto *scrollDesc = new QLabel(QStringLiteral("启用后，查看歌谱时图片会自动从顶部滚动到底部，速度可在歌谱页工具栏调整"), this);
    scrollDesc->setStyleSheet(QStringLiteral("color: #8a7a6a; font-size: 12px; font-weight: normal; padding-left: 24px;"));
    scrollDesc->setWordWrap(true);
    scrollLayout->addWidget(scrollDesc);

    layout->addWidget(scrollGroup);
    layout->addStretch(1);

    // 监听单选按钮变化
    connect(m_displayButtonGroup, &QButtonGroup::idClicked,
            this, &SettingsPage::onDisplayModeChanged);

    // 监听自动滚动选项变化
    connect(m_autoScrollCheck, &QCheckBox::toggled, this, [](bool checked) {
        QSettings settings;
        settings.setValue(QStringLiteral("display/autoScroll"), checked);
    });
}

void SettingsPage::loadSettings()
{
    QSettings settings;
    int mode = settings.value(QStringLiteral("display/mode"), 0).toInt();
    auto *btn = m_displayButtonGroup->button(mode);
    if (btn)
        btn->setChecked(true);

    bool autoScroll = settings.value(QStringLiteral("display/autoScroll"), false).toBool();
    m_autoScrollCheck->setChecked(autoScroll);
}

void SettingsPage::onDisplayModeChanged(int modeId)
{
    QSettings settings;
    settings.setValue(QStringLiteral("display/mode"), modeId);
    emit displayModeChanged();
}
