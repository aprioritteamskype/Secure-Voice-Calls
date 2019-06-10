#include "qmlmissedcallsmodel.h"

secure_voice_call::QMLMissedCallsModel::QMLMissedCallsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int secure_voice_call::QMLMissedCallsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return  static_cast<int>(mMissedCalls.size());
}

void secure_voice_call::QMLMissedCallsModel::addMissedCall(const std::string &name, const QTime &time)
{
    beginResetModel();
    MissedCall missedCall;
    missedCall.name = name;
    missedCall.time = time.toString("HH:mm:ss").toStdString();
    mMissedCalls.push_back(missedCall);
    endResetModel();
}

void secure_voice_call::QMLMissedCallsModel::clear()
{
    beginResetModel();
    mMissedCalls.clear();
    endResetModel();
}

QVariant secure_voice_call::QMLMissedCallsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    Roles castedRole = static_cast<Roles>(role);
    size_t vecIndex = static_cast<size_t>(index.row());

    switch (castedRole) {
        case NameRole:
        {
            return QString::fromStdString(mMissedCalls[vecIndex].name);
        }
        case TimeRole:
        {
            return QString::fromStdString(mMissedCalls[vecIndex].time);
        }
        default:
        {
            return  QVariant();
        }
    }
}

QHash<int, QByteArray> secure_voice_call::QMLMissedCallsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::NameRole] = "username";
    roles[Roles::TimeRole] = "time";
    return roles;
}
