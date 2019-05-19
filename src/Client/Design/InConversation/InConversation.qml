import QtQuick 2.0

Item {
    id: root
    readonly property int constSize: 300
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
}
