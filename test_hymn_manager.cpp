// HymnManager 单元测试
// 编译：cd build && cmake .. && cmake --build . --target test_hymn_manager && ./test_hymn_manager
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <cassert>
#include "HymnManager.h"

#define ASSERT_TRUE(cond, msg) \
    do { \
        if (!(cond)) { \
            qCritical() << "FAIL:" << msg; \
            return 1; \
        } \
        qDebug() << "PASS:" << msg; \
    } while(0)

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // 使用临时文件测试
    QString testPath = QDir::tempPath() + QStringLiteral("/test_hymns.json");

    // 准备测试数据
    QJsonArray arr;
    {
        QJsonObject h1, h2;
        h1[QStringLiteral("id")] = 1;
        h1[QStringLiteral("title")] = QStringLiteral("奇异恩典");
        h1[QStringLiteral("imagePath")] = QStringLiteral("images/amazing_grace.png");
        h1[QStringLiteral("pageNumber")] = 1;
        arr.append(h1);

        h2[QStringLiteral("id")] = 2;
        h2[QStringLiteral("title")] = QStringLiteral("如鹿切慕溪水");
        h2[QStringLiteral("imagePath")] = QStringLiteral("images/as_the_deer.png");
        h2[QStringLiteral("pageNumber")] = 2;
        arr.append(h2);
    }

    QFile file(testPath);
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(arr).toJson());
    file.close();

    HymnManager &mgr = HymnManager::instance();

    // 1. 加载测试
    ASSERT_TRUE(mgr.load(testPath), "加载 hymns.json");
    ASSERT_TRUE(mgr.count() == 2, "数量为 2");

    // 2. 查询测试
    Hymn h = mgr.hymnById(1);
    ASSERT_TRUE(h.id == 1 && h.title == QStringLiteral("奇异恩典"), "通过 id 查询");

    h = mgr.hymnByIndex(1);
    ASSERT_TRUE(h.id == 2 && h.title == QStringLiteral("如鹿切慕溪水"), "通过索引查询");

    // 3. 搜索测试
    auto results = mgr.search(QStringLiteral("恩典"));
    ASSERT_TRUE(results.size() == 1 && results[0].id == 1, "搜索歌名含「恩典」");

    results = mgr.search(QStringLiteral("如鹿"));
    ASSERT_TRUE(results.size() == 1, "搜索歌名含「如鹿」");

    results = mgr.search(QStringLiteral("不存在"));
    ASSERT_TRUE(results.isEmpty(), "搜索不存在的歌名返回空");

    // 4. 新增测试
    Hymn newHymn;
    newHymn.title = QStringLiteral("有一天");
    newHymn.imagePath = QStringLiteral("images/someday.png");
    int newId = mgr.addHymn(newHymn);
    ASSERT_TRUE(newId > 0, "新增返回有效 id");
    ASSERT_TRUE(mgr.count() == 3, "新增后数量为 3");

    h = mgr.hymnById(newId);
    ASSERT_TRUE(h.title == QStringLiteral("有一天"), "新增的歌名正确");

    // 5. 修改测试
    h.title = QStringLiteral("有一天（更新版）");
    ASSERT_TRUE(mgr.updateHymn(h), "修改成功");
    h = mgr.hymnById(newId);
    ASSERT_TRUE(h.title == QStringLiteral("有一天（更新版）"), "修改后歌名正确");

    // 6. 删除测试
    ASSERT_TRUE(mgr.deleteHymn(newId), "删除成功");
    ASSERT_TRUE(mgr.count() == 2, "删除后数量为 2");
    h = mgr.hymnById(newId);
    ASSERT_TRUE(h.id == 0, "删除后查询不到");

    // 7. 保存再加载验证持久化
    ASSERT_TRUE(mgr.save(), "保存成功");

    // 重新加载验证
    ASSERT_TRUE(mgr.load(testPath), "重新加载");

    // 此时 mgr 内容被重置为原始 2 条，因为重新加载了测试文件
    // 不对，重新加载时 testPath 文件已经被 save() 覆盖为删除后的 2 条
    ASSERT_TRUE(mgr.count() == 2, "持久化后的数量为 2");

    // 清理
    QFile::remove(testPath);

    qDebug() << "\n=== 所有测试通过 ===";
    return 0;
}
