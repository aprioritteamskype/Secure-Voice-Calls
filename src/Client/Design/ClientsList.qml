import QtQuick 2.0

Rectangle {
    id: root
    property var model: null
    width: 300; height: 400

    Component {
        id: dragDelegate

        Rectangle {
            id: content

            anchors { left: parent.left; right: parent.right }
            height: column.implicitHeight + 20

            border.width: 1
            border.color: "lightsteelblue"

            radius: 20

            Column {
                id: column
                anchors { fill: parent; margins: 2 }


                Row{
                    Text {
                        text: model.username
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        height: 20
                        width: 70
                    }

                    CallButton{
                        height: 20
                        width: 40
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                //#TODO call
                                console.log("Call " + username)
                            }
                        }
                    }
                }
            }
        }
    }
    ListView {
        id: view

        anchors { centerIn: parent; topMargin: 20; margins: 2 }
        implicitHeight: parent.height - 100
        implicitWidth: parent.width - 20

        model: root.model
        delegate: dragDelegate

        spacing: 4
        cacheBuffer: 50
    }
}
