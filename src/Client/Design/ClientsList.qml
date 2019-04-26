import QtQuick 2.0

Rectangle {
    id: root

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
                        text:  name
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
                                console.log("Call " + name)
                            }
                        }
                    }
                }
            }
        }
    }
    ListView {
        id: view
        anchors.topMargin: 20

        anchors { fill: parent; margins: 2 }

        model: ClientsModel {}
        delegate: dragDelegate

        spacing: 4
        cacheBuffer: 50
    }
}
