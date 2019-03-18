#pragma once

#include <utility>

class QJsonDocument;
class QJsonValue;
class QString;

namespace QJsonPath {

std::pair<int, QString> getKey(const QString &path, int lpos);
std::pair<int, QString> getIndex(const QString &path, int lpos);
QJsonValue getValue(const QJsonValue &root, const QString &path);
QJsonValue getValue(const QJsonDocument &root, const QString &path);
void setValue(QJsonValue &root, const QString &path, const QJsonValue &value);
void setValue(QJsonDocument &root, const QString &path, const QJsonValue &value);


}
