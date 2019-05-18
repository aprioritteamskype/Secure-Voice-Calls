import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import com.securevoicecaller 1.0

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
        RefreshButton {
            width: 100
            height: 70
            anchors.top: root.top
            anchors.horizontalCenter: root.horizontalCenter
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

        Authorization {
            anchors.centerIn: parent
            width: parent.width / 2
            height: parent.height / 2
            visible: root.state == "authorization"
        }

        BusyIndicator {
            width: parent.width / 3
            height: parent.width / 3
            anchors.centerIn: parent
            visible: root.state == "outgoingcall"
        }

        Rectangle {
            width: parent.width / 3
            height: parent.width / 3
            anchors.centerIn: parent
            color: "red"
            visible: root.state == "incomingcall"
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
            },
            State {
                name: "outgoingcall"
                when: globClientState.clientState === QMLClientState.OutgoingCall
            },
            State {
                name: "incomingcall"
                when: globClientState.clientState === QMLClientState.IncomingCall
            }

        ]
    }
}
