import QtQuick 2.9
import QtQuick.Window 2.2
import com.securevoivecaller 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Secure Voice Caller")

    Background {
        id: root
        ClientsList {
            id: listViwer
            x: 20
            y: 20
            model: onlineClientsModel
            radius: 20
            width: parent.width / 3
            height: parent.height - 50
            visible: root.state == "online"
        }
        Text {
            id: usersOnlineLabel
            text: "users online"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors { left: listViwer.left; right: listViwer.right; bottom: listViwer.top }
            visible: root.state == "online"
        }

        Authotization {
            anchors.centerIn: parent
            width: parent.width / 2
            height: parent.height / 2
            visible: root.state == "authorization"
        }

        states : [
            State {
                name: "authorization"
                when: globClientState.clientState === QMLClientState.Authorization
            },
            State {
                name: "online"
                when: globClientState.clientState === QMLClientState.Online
            },
            State {
                name: "inconversation"
                when: globClientState.clientState === QMLClientState.InConversation
            }
        ]
    }
}
