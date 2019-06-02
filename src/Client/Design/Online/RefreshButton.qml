import QtQuick 2.0

Rectangle {
    radius: 80
    gradient: Gradient {
        GradientStop {
            position: 0.04;
            color: "#15e08b";
        }
        GradientStop {
            position: 1.00;
            color: "#ffffff";
        }
    }
    color: "#e85815"
    border.color: mouseArea.containsMouse ? "lime" : "blue"
    border.width: 2

    Text {
        id: name
        text: qsTr("Refresh")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            client.sendClientsOnlineRequest()
        }
        hoverEnabled: true
    }
}
