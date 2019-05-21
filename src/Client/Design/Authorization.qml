import QtQuick 2.7

Rectangle {
    radius: 70
    border.width: 6
    border.color: "#7f1d8b"

    Column {
        id: column
        anchors.centerIn: parent
        spacing: 50
        width: parent.width - 50
        height: parent.height - 50

        Row {
            id: row
            width: parent.width
            spacing: 10
            Text {
                height: 30
                text: qsTr("Enter name: ")
                font.pixelSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Rectangle {
                id: enterNameDelegate
                property string enteredText: enterName.text
                border.color: "black"
                border.width: 2
                width: enterName.width + 10
                height: enterName.height
                TextInput {
                    id: enterName
                    width: 100
                    maximumLength: 70
                    height: 30
                    focus: true
                    horizontalAlignment: TextInput.AlignHCenter
                    verticalAlignment: TextInput.AlignVCenter
                    color: "red"
                    text: ""
                    font.pixelSize: 12
                    layer.enabled: true
                }
            }


            Rectangle {
                id: buttonAuthorizate
                border.color: buttonAuthorizateMouseArea.containsMouse ? "lime" : "blue"
                border.width: 2

                width: authorizateText.contentWidth+ 10
                height: 30
                color: "#c4fb9c"
                radius: 80
                MouseArea{
                    id: buttonAuthorizateMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        globClientState.tryAuthorizate(enterNameDelegate.enteredText);
                    }
                }
                Text {
                    id: authorizateText
                    text: qsTr("Authorizate")
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12
                    anchors.fill: parent
                }
            }
        }

        Row {
            id: row1
            width: parent.width

            Text {
                text: qsTr("Status:")
                font.pixelSize: 12
            }

            Text {
                id: statusLable
                width: 80
                height: 20
                text: qsTr("")
                font.pixelSize: 12
            }
        }
    }
}
