import QtQuick 2.0
import QtQuick.Controls 2.5

Item {
    id: root

    Rectangle {
        id: echo
        border.color: "black"
        border.width: 7
        color: "transparent"
        width: 250
        height: 250
        radius: 200
        anchors.centerIn: parent
        SequentialAnimation {
            running: true
            loops: Animation.Infinite
            ParallelAnimation {
                NumberAnimation {
                    target: echo
                    property: "width"
                    duration: 800
                    from: 250
                    to: 350
                }
                NumberAnimation {
                    target: echo
                    property: "height"
                    duration: 800
                    from: 250
                    to: 350
                }
                NumberAnimation {
                    target: echo
                    property: "border.width"
                    duration: 800
                    from: 7
                    to: 0
                }
                PropertyAnimation {
                    target: echo
                    property: "anchors.centerIn"
                    to: echo.parent
                }
            }
        }
    }
    Rectangle {
        id: userImage
        width: 250
        height: 250
        anchors.centerIn: parent
        color: "transparent"
        Image {
            anchors.fill: parent
            source: "qrc:/Design/Images/user.png"
        }
    }
    RoundButton {
        anchors.top: userImage.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: userImage.horizontalCenter
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
        text: "decline"
        onClicked: {
            console.log("decline pressed")
        }
    }
}
