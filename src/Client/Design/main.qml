import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import com.securevoicecaller 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Secure Voice Caller")

    Background {
        id: root

        Loader {
            id: loaderOnlineComponent
            anchors.fill: parent
            source: "Online/Online.qml"
            active: globClientState.clientState === QMLClientState.Online
        }

        Loader {
            id: loaderAuthorizationComponent
            anchors.centerIn: parent
            width: parent.width / 2
            height: parent.height / 2
            source: "Authorization/Authorization.qml"
            active: globClientState.clientState === QMLClientState.Authorization
        }

        Loader {
            id: loaderOutgoingCallComponent
            anchors.centerIn: parent
            source: "OutgoingCall/OutgoingCall.qml"
            active: globClientState.clientState === QMLClientState.OutgoingCall
        }

        Loader {
            id: loaderIncomingCallComponent
            anchors.centerIn: parent
            source: "IncomingCall/IncomingCall.qml"
            active: globClientState.clientState === QMLClientState.IncomingCall
        }

        Loader {
            id: loaderInConversationComponent
            anchors.fill: parent
            source: "InConversation/InConversation.qml"
            active: globClientState.clientState === QMLClientState.InConversation
        }
    }
}
