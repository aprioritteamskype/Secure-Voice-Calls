#ifndef QMLCLIENTSONLINEMODEL_H
#define QMLCLIENTSONLINEMODEL_H

#include <QAbstractItemModel>

namespace secure_voice_call{
class QMLClientsOnlineModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1
    };
    explicit QMLClientsOnlineModel(QObject *parent = nullptr);

    void setCLients(std::vector<std::string> &clients);
    // interface:
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;
private:
    std::vector<std::string> mClients;
};
}
#endif // QMLCLIENTSONLINEMODEL_H
