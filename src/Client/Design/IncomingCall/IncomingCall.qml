import QtQuick 2.0
import QtQuick.Controls 2.5

Item {
    id: root
    width: column.width
    height: column.height
    property string  callername: ""

    Column {
        id: column
        spacing: 50
        Image {
            id: userImage
            width: 250
            height: 250
            source: "qrc:/Design/Images/user.png"
            Text {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 30
                anchors.horizontalCenter: parent.horizontalCenter
                color: "red"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 12
                text: root.callername
            }
        }
        Row {
            spacing: 50
            RoundButton {
                radius: 100
                background: Rectangle {
                    radius: parent.radius
                    gradient: Gradient {
                        GradientStop {
                            position: 0.00;
                            color: "#008000";
                        }
                        GradientStop {
                            position: 1.00;
                            color: "#ffffff";
                        }
                    }
                    border.width: 3
                    border.color: acceptArea.containsMouse ? "lime" : "black"
                    MouseArea{
                        id: acceptArea
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }

                width: 100
                height: 60
                text: "accept"
                onClicked: {
                    console.log("accept pressed")
                }
            }
            RoundButton {
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
                        onClicked: {
                            console.log("nevatest decline pressed");
                        }
                    }
                }
                width: 100
                height: 60
                text: "stop call"
            }
        }
    }
}
