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
