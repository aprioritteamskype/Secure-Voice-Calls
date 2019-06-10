import QtQuick 2.12

Rectangle {
    anchors.centerIn: parent
    radius: 70
    border.width: 6
    border.color: "#7f1d8b"

    Column {
        id: column
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenterOffset: 40
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
                font.pixelSize: 15
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Rectangle {
                id: enterNameDelegate
                property string enteredText: enterName.text
                border.color: "black"
                border.width: 2
                width: row.width - 220
                height: enterName.height
                TextInput {
                    id: enterName
                    width: enterNameDelegate.width - 10
                    maximumLength: 70
                    height: 30
                    horizontalAlignment: TextInput.AlignHCenter
                    verticalAlignment: TextInput.AlignVCenter
                    color: "red"
                    text: ""
                    font.pixelSize: 15
                    layer.enabled: true
                }
            }


            Rectangle {
                id: buttonAuthorizate
                border.color: buttonAuthorizateMouseArea.containsMouse ? "lime" : "blue"
                border.width: 2

                width: authorizateText.contentWidth + row.width/15 +  10
                height: 30
                color: "#c4fb9c"
                radius: 80
                MouseArea{
                    id: buttonAuthorizateMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        client.sendAuthorizationRequest(enterNameDelegate.enteredText);
                    }
                }
                Text {
                    id: authorizateText
                    text: qsTr("Authorizate")
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 15
                    anchors.fill: parent
                }
            }
        }

        Row {
            id: row1
            width: parent.width
            spacing: 10
            Text {
                text: qsTr("Status:")
                font.pixelSize: 15
            }

            Text {
                id: statusLable
                width: 80
                height: 20
                text: globClientState.status
                font.pixelSize: 15
            }
        }
    }
}
