import QtQuick 2.0
import QtQuick.Controls 2.5

Item {
    id: root
    anchors.fill: parent
    visible: false

    RoundButton {
            id: backButton
            anchors.top: root.top
            anchors.topMargin: 5
            anchors.right:  root.right
            anchors.rightMargin: 20
            radius: 100
            width: implicitWidth + 40
            height: 30
            text: "Back"
            background: Rectangle {
                radius: parent.radius
                gradient: Gradient {
                    GradientStop {
                        position: 0.00;
                        color: "#dbdfe0";
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
                        root.visible = false;
                    }
                }
            }
        }

    MissedCallsList {
        id: listViwer
        x: 20
        y: 20
        border.width: 5
        model: missedCallsModel
        radius: 20
        width: parent.width / 3
        height: parent.height - 50
    }

    ClearButton {
        width: 100
        height: 30
        anchors.bottom: listViwer.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: listViwer.horizontalCenter
    }

    Text {
        id: missedCallsLabel
        text: "Missed Calls"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors { left: listViwer.left; right: listViwer.right; bottom: listViwer.top }
    }

    Component.onDestruction: {
        missedCallsModel.clear();
    }
}
