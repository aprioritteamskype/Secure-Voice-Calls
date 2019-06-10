#pragma once

#include <QAbstractItemModel>
#include <QTime>

namespace secure_voice_call{
    class QMLMissedCallsModel : public QAbstractListModel
    {
        Q_OBJECT

        struct MissedCall{
            std::string name;
            std::string time;
        };

    public:
        enum Roles {
            NameRole = Qt::UserRole + 1,
            TimeRole
        };
        explicit QMLMissedCallsModel(QObject *parent = nullptr);

    public:
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        void addMissedCall(const std::string& name, const QTime& time);
        Q_INVOKABLE void clear();
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        QHash<int, QByteArray> roleNames() const override;
    private:
        std::vector<MissedCall> mMissedCalls;
    };
}
