import QtQuick 2.0

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
                id: labelEnterName
                height: 30
                text: qsTr("Enter name: ")
                font.pixelSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Rectangle {
                border.color: "blue"
                border.width: 2

                width: enterName.width
                height: enterName.height
                TextEdit {
                    id: enterName
                    width: 80
                    height: 30
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "red"
                    text: ""
                    font.pixelSize: 12
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
                id: element1
                text: qsTr("Text")
                font.pixelSize: 12
            }

            TextEdit {
                id: textEdit1
                width: 80
                height: 20
                text: qsTr("Text Edit")
                font.pixelSize: 12
            }
        }
    }
}
