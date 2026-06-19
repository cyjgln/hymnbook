#include "AboutPage.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTextBrowser>

AboutPage::AboutPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void AboutPage::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 20, 30, 20);

    // 返回按钮
    auto *backBtn = new QPushButton(QStringLiteral("← 返回目录"), this);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &AboutPage::directoryRequested);
    layout->addWidget(backBtn);

    // 说明内容
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setOpenExternalLinks(false);
    m_textBrowser->setStyleSheet(QStringLiteral(
        "QTextBrowser {"
        "  background: transparent;"
        "  border: none;"
        "  font-size: 14px;"
        "}"
    ));

    m_textBrowser->setHtml(QStringLiteral(
        "<div style='max-width: 600px; margin: 0 auto;'>"

        "  <h1 style='text-align: center; color: #4a3728; margin-top: 20px;'>"
        "    诗歌本"
        "  </h1>"
        "  <p style='text-align: center; color: #888; font-size: 14px;'>"
        "    版本 1.0.0"
        "  </p>"

        "  <hr style='border: none; border-top: 1px solid #e0d5c8; margin: 20px 0;'>"

        "  <h2 style='color: #5a4a3a;'>功能说明</h2>"
        "  <ul style='line-height: 1.8; color: #333;'>"
        "    <li><b>浏览目录</b> — 按页码排列显示全部诗歌，点击即可查看</li>"
        "    <li><b>查看歌谱</b> — 显示歌谱图片，支持缩放、拖动查看细节</li>"
        "    <li><b>搜索诗歌</b> — 按歌名关键字快速查找（Ctrl+F）</li>"
        "    <li><b>切换诗歌</b> — 上一首 / 下一首 方便连续查看</li>"
        "    <li><b>全屏投影</b> — 适合聚会、主日敬拜时使用（F11）</li>"
        "    <li><b>管理歌谱</b> — 新增、修改、删除歌谱，自动维护目录</li>"
        "  </ul>"

        "  <h2 style='color: #5a4a3a; margin-top: 24px;'>快捷键一览</h2>"
        "  <table style='width: 100%; border-collapse: collapse; line-height: 2.2;'>"
        "    <tr style='border-bottom: 1px solid #e0d5c8;'>"
        "      <td style='padding: 6px 12px; font-weight: bold; width: 40%;'>Ctrl + F</td>"
        "      <td style='padding: 6px 12px;'>打开搜索</td>"
        "    </tr>"
        "    <tr style='border-bottom: 1px solid #e0d5c8;'>"
        "      <td style='padding: 6px 12px; font-weight: bold;'>← (左方向键)</td>"
        "      <td style='padding: 6px 12px;'>上一首</td>"
        "    </tr>"
        "    <tr style='border-bottom: 1px solid #e0d5c8;'>"
        "      <td style='padding: 6px 12px; font-weight: bold;'>→ (右方向键)</td>"
        "      <td style='padding: 6px 12px;'>下一首</td>"
        "    </tr>"
        "    <tr style='border-bottom: 1px solid #e0d5c8;'>"
        "      <td style='padding: 6px 12px; font-weight: bold;'>Esc</td>"
        "      <td style='padding: 6px 12px;'>返回目录</td>"
        "    </tr>"
        "    <tr style='border-bottom: 1px solid #e0d5c8;'>"
        "      <td style='padding: 6px 12px; font-weight: bold;'>Ctrl + +</td>"
        "      <td style='padding: 6px 12px;'>放大图片</td>"
        "    </tr>"
        "    <tr style='border-bottom: 1px solid #e0d5c8;'>"
        "      <td style='padding: 6px 12px; font-weight: bold;'>Ctrl + -</td>"
        "      <td style='padding: 6px 12px;'>缩小图片</td>"
        "    </tr>"
        "    <tr style='border-bottom: 1px solid #e0d5c8;'>"
        "      <td style='padding: 6px 12px; font-weight: bold;'>滚轮</td>"
        "      <td style='padding: 6px 12px;'>放大 / 缩小图片</td>"
        "    </tr>"
        "    <tr style='border-bottom: 1px solid #e0d5c8;'>"
        "      <td style='padding: 6px 12px; font-weight: bold;'>F11</td>"
        "      <td style='padding: 6px 12px;'>全屏切换</td>"
        "    </tr>"
        "  </table>"

        "  <hr style='border: none; border-top: 1px solid #e0d5c8; margin: 24px 0;'>"

        "  <p style='text-align: center; color: #999; font-size: 12px; line-height: 1.6;'>"
        "    适用于教会聚会、主日敬拜、祷告会等场景<br>"
        "    数据本地存储，无需网络"
        "  </p>"

        "</div>"
    ));
    layout->addWidget(m_textBrowser, 1);
}
