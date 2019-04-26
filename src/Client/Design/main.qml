import QtQuick 2.9
import QtQuick.Window 2.2

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Secure Voice Caller")
    AuthorizationItem{
        id: item
        ClientsList{
            id: listViwer
            x: 20
            y: 20
            radius: 20
            width: parent.width / 3
            height: parent.height - 50

        }
        Text {
            text: "users online"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors { left: listViwer.left; right: listViwer.right; bottom: listViwer.top }
        }
    }
}
