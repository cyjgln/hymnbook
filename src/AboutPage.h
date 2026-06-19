// Copyright (c) 2026 cyjgln. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.


#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <QWidget>

class QTextBrowser;

class AboutPage : public QWidget
{
    Q_OBJECT

public:
    explicit AboutPage(QWidget *parent = nullptr);
    ~AboutPage() override = default;

signals:
    void directoryRequested();

private:
    void setupUI();

    QTextBrowser *m_textBrowser;
};

#endif // ABOUTPAGE_H
