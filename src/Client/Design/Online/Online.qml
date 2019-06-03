import QtQuick 2.0
import QtQuick.Controls 2.5

Item {
    id: root
    anchors.fill: parent

    Row {
        spacing: 10
        anchors.top: root.top
        anchors.topMargin: 5
        anchors.right:  root.right
        anchors.rightMargin: 20

        Text {
            id: authorizatedAs
            text: "Authorizated as:"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            height: 30
            font.pixelSize: 15
        }
        Text {
            id: authorizatedName
            text: globClientState.authorizatedAs
            color: "yellow"
            font.pixelSize: 20
            style: Text.Outline
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            height: 30
        }
        RoundButton {
            id: exitButton
            radius: 100
            width: 100
            height: 30
            text: "Exit"
            background: Rectangle {
                radius: parent.radius
                gradient: Gradient {
                    GradientStop {
                        position: 0.00;
                        color: "#ff0000";
                    }
                    GradientStop {
                        position: 1.00;
                        color: "#ffffff";
                    }
                }
                border.width: 3
                border.color: declineArea.containsMouse ? "lime" : "black"
                MouseArea{
                    id: declineArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        client.exit();
                    }
                }
            }
        }
    }

    ClientsList {
        id: listViwer
        x: 20
        y: 20
        border.width: 5
        model: onlineClientsModel
        radius: 20
        width: parent.width / 3
        height: parent.height - 50
    }
    RefreshButton {
        width: 100
        height: 30
        anchors.bottom: listViwer.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: listViwer.horizontalCenter
    }

    Text {
        id: usersOnlineLabel
        text: "users online"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors { left: listViwer.left; right: listViwer.right; bottom: listViwer.top }
    }
}
