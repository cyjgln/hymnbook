#include "HymnManager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QCoreApplication>
#include <algorithm>

HymnManager &HymnManager::instance()
{
    static HymnManager inst;
    return inst;
}

bool HymnManager::load(const QString &filePath)
{
    m_filePath = filePath;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray())
        return false;

    m_hymns.clear();
    m_nextId = 1;

    QJsonArray arr = doc.array();
    for (const QJsonValue &val : arr) {
        if (!val.isObject()) continue;
        Hymn h = Hymn::fromJson(val.toObject());
        m_hymns.append(h);
        if (h.id >= m_nextId)
            m_nextId = h.id + 1;
    }

    emit dataChanged();
    return true;
}

bool HymnManager::save()
{
    if (m_filePath.isEmpty())
        return false;

    QJsonArray arr;
    for (const Hymn &h : m_hymns) {
        arr.append(h.toJson());
    }

    QJsonDocument doc(arr);
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QList<Hymn> HymnManager::allHymns() const
{
    return m_hymns;
}

Hymn HymnManager::hymnById(int id) const
{
    for (const Hymn &h : m_hymns) {
        if (h.id == id)
            return h;
    }
    return Hymn();
}

Hymn HymnManager::hymnByIndex(int index) const
{
    if (index >= 0 && index < m_hymns.size())
        return m_hymns.at(index);
    return Hymn();
}

int HymnManager::count() const
{
    return m_hymns.size();
}

QList<Hymn> HymnManager::search(const QString &keyword) const
{
    if (keyword.isEmpty())
        return m_hymns;

    QList<Hymn> results;
    for (const Hymn &h : m_hymns) {
        if (h.title.contains(keyword, Qt::CaseInsensitive))
            results.append(h);
    }
    return results;
}

int HymnManager::addHymn(const Hymn &hymn)
{
    Hymn h = hymn;
    h.id = m_nextId++;
    h.pageNumber = m_hymns.size() + 1;
    m_hymns.append(h);
    emit dataChanged();
    return h.id;
}

void HymnManager::batchAddHymns(const QList<Hymn> &hymns)
{
    for (const Hymn &hymn : hymns) {
        Hymn h = hymn;
        h.id = m_nextId++;
        h.pageNumber = m_hymns.size() + 1;
        m_hymns.append(h);
    }
    emit dataChanged();
}

bool HymnManager::updateHymn(const Hymn &updated)
{
    for (int i = 0; i < m_hymns.size(); ++i) {
        if (m_hymns[i].id == updated.id) {
            m_hymns[i] = updated;
            // 保持 id 不变
            m_hymns[i].id = updated.id;
            emit dataChanged();
            return true;
        }
    }
    return false;
}

bool HymnManager::deleteHymn(int id)
{
    for (int i = 0; i < m_hymns.size(); ++i) {
        if (m_hymns[i].id == id) {
            m_hymns.removeAt(i);
            regeneratePageNumbers();
            emit dataChanged();
            return true;
        }
    }
    return false;
}

void HymnManager::regeneratePageNumbers()
{
    for (int i = 0; i < m_hymns.size(); ++i) {
        m_hymns[i].pageNumber = i + 1;
    }
}
