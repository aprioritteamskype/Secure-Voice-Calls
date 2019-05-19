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
            anchors.fill: parent
            source: "Authorization/Authorization.qml"
            active: globClientState.clientState === QMLClientState.Authorization
        }

        Loader {
            id: loaderOutgoingCallComponent
            anchors.fill: parent
            sourceComponent: outgoingCallIndicator
            active: globClientState.clientState === QMLClientState.OutgoingCall
            Component {
                id: outgoingCallIndicator
                BusyIndicator {
                    width: parent.width / 3
                    height: parent.width / 3
                    anchors.centerIn: parent
                }
            }
        }

        Loader {
            id: loaderIncomingCallComponent
            anchors.fill: parent
            sourceComponent: incomingCallIndicator
            active: globClientState.clientState === QMLClientState.IncomingCall

            Component {
                id: incomingCallIndicator
                Image {
                    id: userImage
                    width: parent.width / 3
                    height: parent.width / 3
                    anchors.centerIn: parent
                    source: "qrc:/Design/Images/user.png"
                }

            }
        }

        Loader {
            id: loaderInConversationComponent
            anchors.fill: parent
            source: "InConversation/InConversation.qml"
            active: globClientState.clientState === QMLClientState.InConversation
        }
    }
}
