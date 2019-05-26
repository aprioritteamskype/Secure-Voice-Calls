import QtQuick 2.0

Item {
    id: root
    anchors.fill: parent
    ClientsList {
        id: listViwer
        x: 20
        y: 20
        model: onlineClientsModel
        radius: 20
        width: parent.width / 3
        height: parent.height - 50
    }
    RefreshButton {
        width: 100
        height: 70
        anchors.top: listViwer.top
        anchors.horizontalCenter: root.horizontalCenter
    }

    Text {
        id: usersOnlineLabel
        text: "users online"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors { left: listViwer.left; right: listViwer.right; bottom: listViwer.top }
    }
}
