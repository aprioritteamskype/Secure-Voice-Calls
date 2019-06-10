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
            height: row.height + 20

            border.width: 1
            border.color: "#5a4c4c"

            radius: 20

                Row{
                    id: row
                    spacing: 10
                    anchors.centerIn: parent
                    Text {
                        text: model.time + " - " + model.username
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        height: 20
                        width: implicitWidth
                    }

                    CallButton {
                        height: 20
                        width: 40
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
