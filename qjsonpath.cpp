#include "qjsonpath.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QRegExp>

#include <stack>
#include <memory>

namespace QJsonPath {

std::pair<int, QString> getKey(const QString &path, int lpos)
{
    if (path.at(lpos) != '.')
        throw std::logic_error("Path parsing error: getIndex, should be used with position of dot");
    int pos = path.indexOf(QRegExp("[\\[\\]\\.]"), ++lpos);
    if (pos == -1)
        pos = path.size();
    int size = pos - lpos;
    if (size <= 0)
        throw std::logic_error("Parsing error: zero-length index");
    return {pos, path.mid(lpos, size)};
}

std::pair<int, QString> getIndex(const QString &path, int lpos)
{
    if (path.at(lpos) != '[')
        throw std::logic_error("Path parsing error: getIndex, should be used with position of left bracket");
    int pos = path.indexOf(QRegExp("[\\[\\]\\.]"), ++lpos);
    if (pos == -1)
        throw std::logic_error("Path parsing error: unclosed brackets - end of path encountered");
    if (path[pos] != ']')
        throw std::logic_error("Parsing error: unexpected closing bracket");
    int size = pos - lpos;
    if (size <= 0)
        throw std::logic_error("Parsing error: zero-length index");
    return {++pos, path.mid(lpos, size)};
}

QJsonValue getValue(const QJsonValue &root, const QString &path)
{
    QJsonValue new_root = root;
    int pos = 0;
    QString key;
    while (pos != path.size()) {
        switch (path.at(pos).toLatin1())
        {
            case '.':
                std::tie(pos, key) = getKey(path, pos);
                new_root = new_root[key];
                break;
            case '[':
                std::tie(pos, key) = getIndex(path, pos);
                new_root = new_root[key.toInt()];
                break;
            default:
                throw std::logic_error("Parsing error: expected key or index");
        }
    }
    return new_root;
}

QJsonValue getValue(const QJsonDocument &root, const QString &path)
{
    QJsonValue new_root;
    if (path.startsWith('['))
    {
        if (not root.isArray())
            throw std::logic_error("Parsing error: expected array, but document is not an array");
        new_root = root.array();
    }
    else if (path.startsWith('.'))
    {
        if (not root.isObject())
            throw std::logic_error("Parsing error: expected object, but document is not an object");
        new_root = root.object();
    }
    else {
        return new_root;
    }
    return getValue(new_root, path);
}

void setValue(QJsonValue &root, const QString &path, const QJsonValue &value)
{
    std::stack<QJsonValue> nodes;
    nodes.push(root);
    std::stack<QString> keys {};
    int pos = 0;
    QString key;
    while (pos != path.size()) {
        switch (path.at(pos).toLatin1())
        {
            case '.':
                std::tie(pos, key) = getKey(path, pos);
                nodes.push(nodes.top()[key]);
                keys.push(key);
                break;
            case '[':
                std::tie(pos, key) = getIndex(path, pos);
                nodes.push(nodes.top()[key.toInt()]);
                keys.push(key);
                break;
            default:
                throw std::logic_error("Parsing error: expected key or index");
        }
    }
    QJsonValue node = value;
    nodes.pop();
    while (nodes.size() >= 1)
    {
        if (nodes.top().isArray())
        {
            QJsonArray array = nodes.top().toArray();
            array[keys.top().toInt()] = node;
            node = array;
        }
        else if (nodes.top().isObject())
        {
            QJsonObject object = nodes.top().toObject();
            object[keys.top()] = node;
            node = object;
        }
        nodes.pop();
        keys.pop();
    }
    root = node;
}

void setValue(QJsonDocument &root, const QString &path, const QJsonValue &value)
{
    QJsonValue new_root;
    if (path.startsWith('['))
    {
        if (not root.isArray())
            throw std::logic_error("Parsing error: expected array, but document is not an array");
        new_root = root.array();
    }
    else if (path.startsWith('.'))
    {
        if (not root.isObject())
            throw std::logic_error("Parsing error: expected object, but document is not an object");
        new_root = root.object();
    }
    else {
        new_root = value;
    }
    setValue(new_root, path, value);
    if (new_root.isArray())
        root = QJsonDocument(new_root.toArray());
    else
    if (new_root.isObject())
        root = QJsonDocument(new_root.toObject());
}

}
