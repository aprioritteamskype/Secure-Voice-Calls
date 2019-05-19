import QtQuick 2.0
import QtQuick.Controls 2.5

Item {
    width: 250
    height: declineButton.height + 50 + busyIndicator.height

    BusyIndicator {
        id: busyIndicator
        width: 250
        height: 250
    }
    RoundButton {
        id: declineButton
        anchors.top: busyIndicator.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: busyIndicator.horizontalCenter
        radius: 100
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
            }
        }
        width: 100
        height: 60
        text: "cancel call"
        onClicked: {
            console.log("decline pressed")
        }
    }
}
