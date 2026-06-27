// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>

class QButtonGroup;
class QCheckBox;
class QGroupBox;

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage() override = default;

signals:
    void directoryRequested();
    void displayModeChanged();

private:
    void setupUI();
    void loadSettings();
    void onDisplayModeChanged(int modeId);

    QGroupBox *m_displayGroup = nullptr;
    QButtonGroup *m_displayButtonGroup = nullptr;
    QCheckBox *m_autoScrollCheck = nullptr;
};

#endif // SETTINGSPAGE_H
