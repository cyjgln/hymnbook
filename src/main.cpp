#include <QApplication>
#include "MainWindow.h"

static const char *globalStyleSheet = R"(
    /* ===== 全局 ===== */
    QMainWindow, QDialog {
        background-color: #f5efe8;
    }

    /* ===== 按钮 ===== */
    QPushButton {
        background-color: #8b6f47;
        color: #ffffff;
        border: 1px solid #7a5f3a;
        border-radius: 6px;
        padding: 7px 18px;
        font-size: 13px;
        font-weight: bold;
        min-height: 24px;
    }
    QPushButton:hover {
        background-color: #7a5f3a;
    }
    QPushButton:pressed {
        background-color: #6a4f2a;
    }

    /* 次要按钮（说明、返回目录等） */
    QPushButton#secondaryBtn {
        background-color: transparent;
        color: #5a4a3a;
        border: 1px solid #d0c5b8;
    }
    QPushButton#secondaryBtn:hover {
        background-color: #ede4d8;
    }
    QPushButton#secondaryBtn:pressed {
        background-color: #e0d5c8;
    }

    /* 危险按钮（删除） */
    QPushButton#dangerBtn {
        background-color: transparent;
        color: #c0392b;
        border: 1px solid #e8b4b0;
    }
    QPushButton#dangerBtn:hover {
        background-color: #fdf0ef;
    }

    /* ===== 工具条（歌谱页导航栏） ===== */
    QToolBar {
        background-color: #ede4d8;
        border-bottom: 1px solid #d0c5b8;
        padding: 4px 8px;
        spacing: 8px;
    }
    QToolBar QPushButton {
        padding: 6px 14px;
        font-size: 12px;
    }

    /* ===== 目录列表 ===== */
    QListWidget {
        background-color: transparent;
        border: none;
        outline: none;
    }
    QListWidget::item {
        background-color: #ffffff;
        border: 1px solid #e0d5c8;
        border-radius: 8px;
        padding: 14px 20px;
        margin: 5px 0px;
        color: #3a2a1a;
        font-size: 15px;
    }
    QListWidget::item:hover {
        background-color: #faf6f0;
        border-color: #c9a96e;
    }
    QListWidget::item:selected {
        background-color: #f5efe8;
        border-left: 4px solid #c9a96e;
    }

    /* ===== 搜索输入框 ===== */
    QLineEdit {
        background-color: #ffffff;
        border: 2px solid #d0c5b8;
        border-radius: 8px;
        padding: 8px 14px;
        font-size: 14px;
        color: #3a2a1a;
    }
    QLineEdit:focus {
        border-color: #c9a96e;
    }

    /* ===== 滚动条 ===== */
    QScrollBar:vertical {
        background: transparent;
        width: 10px;
        margin: 0;
    }
    QScrollBar::handle:vertical {
        background: #d0c5b8;
        border-radius: 5px;
        min-height: 30px;
    }
    QScrollBar::handle:vertical:hover {
        background: #b8ad9e;
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0;
    }

    /* ===== 说明页文本 ===== */
    QTextBrowser {
        background: transparent;
        border: none;
        font-size: 14px;
    }
)";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("HymnBook"));
    app.setApplicationName(QStringLiteral("HymnBookApp"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));

    app.setStyleSheet(QString::fromUtf8(globalStyleSheet));

    MainWindow window;
    window.show();

    return app.exec();
}
