#include "EditDialog.h"
#include "HymnManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QDateTime>

EditDialog::EditDialog(Mode mode, const Hymn &hymn, QWidget *parent)
    : QDialog(parent), m_mode(mode), m_originalHymn(hymn)
{
    setupUI();

    if (mode == EditMode) {
        setWindowTitle(QStringLiteral("编辑歌谱"));
        m_titleInput->setText(hymn.title);
        m_imagePathInput->setText(hymn.imagePath);
        // 尝试加载现有图片预览
        if (!hymn.imagePath.isEmpty()) {
            QString dataDir = QFileInfo(HymnManager::instance().dataFilePath()).absolutePath();
            QString fullPath = QDir(dataDir).absoluteFilePath(QStringLiteral("../") + hymn.imagePath);
            fullPath = QDir::cleanPath(fullPath);
            QPixmap pix(fullPath);
            if (!pix.isNull()) {
                m_previewLabel->setPixmap(pix.scaled(200, 280, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
        }
    } else {
        setWindowTitle(QStringLiteral("新增歌谱"));
    }
}

void EditDialog::setupUI()
{
    setMinimumSize(450, 400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);

    // 表单区域
    auto *formLayout = new QFormLayout();

    m_titleInput = new QLineEdit(this);
    m_titleInput->setPlaceholderText(QStringLiteral("请输入歌名"));
    m_titleInput->setFont(QFont(m_titleInput->font().family(), 14));
    formLayout->addRow(QStringLiteral("歌名："), m_titleInput);

    // 图片选择行
    auto *imageLayout = new QHBoxLayout();
    m_imagePathInput = new QLineEdit(this);
    m_imagePathInput->setPlaceholderText(QStringLiteral("选择歌谱图片 (PNG/JPG)"));
    m_imagePathInput->setReadOnly(true);
    imageLayout->addWidget(m_imagePathInput, 1);

    auto *browseBtn = new QPushButton(QStringLiteral("浏览..."), this);
    connect(browseBtn, &QPushButton::clicked, this, &EditDialog::browseImage);
    imageLayout->addWidget(browseBtn);

    formLayout->addRow(QStringLiteral("图片："), imageLayout);
    layout->addLayout(formLayout);

    // 图片预览
    m_previewLabel = new QLabel(this);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setMinimumHeight(200);
    m_previewLabel->setStyleSheet(QStringLiteral("border: 1px solid #ccc; background: #f9f9f9;"));
    m_previewLabel->setText(QStringLiteral("预览区域"));
    layout->addWidget(m_previewLabel, 1);

    // 按钮区域
    auto *btnLayout = new QHBoxLayout();

    if (m_mode == EditMode) {
        auto *deleteBtn = new QPushButton(QStringLiteral("删除歌谱"), this);
        deleteBtn->setStyleSheet(QStringLiteral("color: red;"));
        connect(deleteBtn, &QPushButton::clicked, this, &EditDialog::confirmDelete);
        btnLayout->addWidget(deleteBtn);
    }

    btnLayout->addStretch();

    auto *cancelBtn = new QPushButton(QStringLiteral("取消"), this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    auto *saveBtn = new QPushButton(QStringLiteral("保存"), this);
    saveBtn->setDefault(true);
    connect(saveBtn, &QPushButton::clicked, this, [this]() {
        if (saveHymn())
            accept();
    });
    btnLayout->addWidget(saveBtn);

    layout->addLayout(btnLayout);

    m_titleInput->setFocus();
}

void EditDialog::browseImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("选择歌谱图片"),
        QString(),
        QStringLiteral("图片文件 (*.png *.jpg *.jpeg);;所有文件 (*)")
    );

    if (filePath.isEmpty())
        return;

    m_selectedImagePath = filePath;
    m_imagePathInput->setText(filePath);

    // 显示预览
    QPixmap pix(filePath);
    if (!pix.isNull()) {
        m_previewLabel->setPixmap(pix.scaled(200, 280, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

bool EditDialog::saveHymn()
{
    QString title = m_titleInput->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请输入歌名"));
        m_titleInput->setFocus();
        return false;
    }

    Hymn hymn = m_originalHymn;
    hymn.title = title;

    // 处理图片复制
    if (!m_selectedImagePath.isEmpty()) {
        QFileInfo fi(m_selectedImagePath);
        // 目标文件名：基于歌名的时间戳，避免冲突
        QString ext = fi.suffix().toLower();
        if (ext != QStringLiteral("png") && ext != QStringLiteral("jpg") && ext != QStringLiteral("jpeg")) {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("仅支持 PNG/JPG 格式图片"));
            return false;
        }

        // 确定 images 目录（在 data 目录旁边）
        QString dataDir = QFileInfo(HymnManager::instance().dataFilePath()).absolutePath();
        QString imagesDir = QDir(dataDir).absoluteFilePath(QStringLiteral("../images"));
        QDir().mkpath(imagesDir);

        // 生成文件名
        QString safeName = title;
        safeName.replace(QRegularExpression(QStringLiteral("[\\\\/:*?\"<>|]")), QStringLiteral("_"));
        QString destName = QStringLiteral("%1_%2.%3")
                               .arg(safeName)
                               .arg(QDateTime::currentMSecsSinceEpoch())
                               .arg(ext);
        QString destPath = QDir(imagesDir).absoluteFilePath(destName);

        if (QFile::copy(m_selectedImagePath, destPath)) {
            // 存储相对路径（相对于 data 目录的父目录）
            QString relativePath = QStringLiteral("images/") + destName;
            hymn.imagePath = relativePath;
        } else {
            QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("图片复制失败，请重试"));
            return false;
        }
    } else if (m_mode == AddMode) {
        // 新增模式：图片可选，允许跳过
        hymn.imagePath = QString();
    }

    // 保存到 HymnManager
    HymnManager &mgr = HymnManager::instance();
    if (m_mode == AddMode) {
        mgr.addHymn(hymn);
    } else {
        mgr.updateHymn(hymn);
    }
    mgr.save();

    m_result = hymn;
    return true;
}

void EditDialog::confirmDelete()
{
    auto result = QMessageBox::question(
        this,
        QStringLiteral("确认删除"),
        QStringLiteral("确定要删除「%1」吗？\n此操作不可撤销。").arg(m_originalHymn.title),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (result == QMessageBox::Yes) {
        emit deleteRequested(m_originalHymn.id);
        accept();
    }
}
