#include "qmlclientsonlinemodel.h"

secure_voice_call::QMLClientsOnlineModel::QMLClientsOnlineModel(QObject *parent)
    : QAbstractListModel(parent)
{
    //FIXME
    beginResetModel();
    for (int i = 0; i < 10; ++i) {
        mClients.push_back(std::to_string(i)+ " user");
    }
    endResetModel();
}

void secure_voice_call::QMLClientsOnlineModel::setCLients(std::vector<std::string> &clients)
{
    beginResetModel();
    mClients = clients;
    endResetModel();
}

int secure_voice_call::QMLClientsOnlineModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return  static_cast<int>(mClients.size());
}

QVariant secure_voice_call::QMLClientsOnlineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(Roles(role) == Roles::NameRole){
        int n = static_cast<int>(index.row());
        return QString::fromStdString(mClients[n]);
    }else {
        return  QVariant();
}
//    switch (role) {
//    case Roles::NameRole:
//        return QString::fromStdString(mClients[index.row()]);
//        //another roles
//    default:
//        return QVariant();
//    }
}

QHash<int, QByteArray> secure_voice_call::QMLClientsOnlineModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Roles::NameRole] = "username";
    return roles;
}
