import QtQuick 2.0
import com.securevoicecaller 1.0

Rectangle {
    id: root

    color: "black"
    radius: 50

    anchors.horizontalCenter: parent.horizontalCenter
    border.color: "#f7f7f7"
    border.width: 2

    width: parent.width - 10
    height: parent.height / 5
    y: 0 - root.height

    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#ffffff";
        }
        GradientStop {
            position: 1.00;
            color: "#000000";
        }
    }


//-----------------------------------------------------------------methods{
    function show(){
        showAnimation.restart();
        hideNotificationTimer.restart();
    }

    function hide(){
        hideAnimation.start()
    }
//-----------------------------------------------------------------methods}

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
                hideNotificationTimer.stop();
                root.hide();
        }
    }

    Text {
        id: statusLable
        anchors.centerIn: parent
        width: 80
        height: 20
        text: globClientState.status
        color: "yellow"
        font.pixelSize: 20
        style: Text.Outline
        horizontalAlignment: TextInput.AlignHCenter
        verticalAlignment: TextInput.AlignVCenter
    }

    Timer {
        id: hideNotificationTimer
        interval: 4000
        onTriggered: {
            hide();
        }
    }

    SequentialAnimation {
        id: showAnimation
        NumberAnimation {
            target: root
            property: "y"

            from: 0 - root.height
            to: 0
            duration: 500
        }
    }

    SequentialAnimation {
        id: hideAnimation
        NumberAnimation {
            target: root
            property: "y"

            from: 0
            to: 0 - root.height
            duration: 500
        }
    }

    Connections {
        id: statusChangeConnection
        target: globClientState
        onStatusChanged: {
            if(globClientState.clientState !== QMLClientState.Authorization){
                show();
            }
        }
    }

}
