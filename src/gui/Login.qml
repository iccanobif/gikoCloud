import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.0
import CutiePoi.Connection 1.0
import CutiePoi.SharedObject 1.0
import "Utils.js" as Utils

DarkBox {
    id: root

    property alias proxy: proxyText.text
    property alias useProxy: useProxyCheckBox.checked
    property alias loginButton: loginButton
    property alias serverText: serverBox.currentText
    property alias stageText: stageBox.currentText

    property var connection;
    property var clientId;
    property var loginCount;
    property var tripcode;

    signal loginButtonPressed(QtObject obj);
    signal loginCompleted(QtObject obj);
    signal connectionFailed(QtObject obj);

    function emitLoginCompleted()
    {
        loginCompleted(root);
    }

    function emitWaitingForStageEntry()
    {
        waitingForStageEntry(root);
    }

    function onServerResponse(res, comm)
    {
        // Connected
        if (res === true) {
            connection.sendPassword("");
        } else {
            onDisconnected();
        }
    }

    function onHandshaken() {
        connection.connectToServer(server(), username());
    }

    function connectToServer()
    {
        if (useProxy && proxy.includes(":")) {
            var tmp = proxy.split(":");
            connection.setProxy(tmp[0], tmp[1]);
        }

        connection.connectToHost();
    }

    function onLoginCountReceived(count)
    {
        loginCount = count;
    }

    function onClientIdReceived(cid)
    {
        clientId = cid;
    }

    function onTripcodeReceived(trip)
    {
        tripcode = trip;
    }

    function onDisconnected()
    {
        connection.destroy();
        connection = null;
        connectionFailed(root);
    }

    function sendLoginDetails()
    {
        if (hasTripbase()) {
            connection.sendTripcodeBase(tripbase());
        } else {
            connection.sendHash();
        }
    }

    function enterStage()
    {
        connection.enterStage(stage(), character());
    }

    function connectSlots(obj)
    {
        connection = obj

        obj.handshaken.connect(onHandshaken);

        // These have handlers have to be named like this because of qml notify events
        obj.clientIdReceived.connect(onClientIdReceived);
        obj.loginCountChanged.connect(onLoginCountReceived);
        obj.tripcodeReceived.connect(onTripcodeReceived);

        obj.error.connect(displayError);
        obj.serverResponse.connect(onServerResponse);
        obj.loginDetailsRequested.connect(sendLoginDetails);
        obj.waitingForStageEntry.connect(enterStage);
        obj.stageEntrySuccessful.connect(emitLoginCompleted);

        obj.disconnected.connect(onDisconnected);
    }

    function disconnectSlots()
    {
        connection.handshaken.disconnect(onHandshaken);
        connection.clientIdReceived.disconnect(onClientIdReceived);
        connection.loginCountChanged.disconnect(onLoginCountReceived);
        connection.tripcodeReceived.disconnect(onTripcodeReceived);

        connection.error.disconnect(displayError);
        connection.serverResponse.disconnect(onServerResponse);
        connection.loginDetailsRequested.disconnect(sendLoginDetails);
        connection.waitingForStageEntry.disconnect(enterStage);
        connection.stageEntrySuccessful.disconnect(emitLoginCompleted);

        connection.disconnected.disconnect(onDisconnected)
    }

    function server()
    {
        switch(serverBox.currentIndex) {
        case 0:
            return CPConnection.Foreign
        case 1:
            return CPConnection.General;
        case 2:
            return CPConnection.Young;
        case 3:
            return CPConnection.Vip;
        case 4:
            return CPConnection.Kws;
        }
    }

    function username()
    {
        if (hasTripbase()) {
            return usernameText.text.split("#")[0];
        }
        return usernameText.text;
    }

    function stage()
    {
        return Utils.stageNameToId(stageText);
    }

    function character()
    {
        // Should be better than doing string comparisons
        switch(charBox.currentIndex) {
        case 0:
            return CPSharedObject.Giko;
        case 1:
            return CPSharedObject.Shii;
        case 2:
            return CPSharedObject.Boon;
        case 3:
            return CPSharedObject.SantaGiko;
        case 4:
            return CPSharedObject.SantaShii;
        case 5:
            return CPSharedObject.FuroshikiGiko;
        case 6:
            return CPSharedObject.PianicaShii;
        }
    }

    function hasTripbase()
    {
        return usernameText.text.includes("#");
    }

    function tripbase()
    {
        if (hasTripbase()) {
            return usernameText.text.slice(usernameText.text.indexOf('#') + 1);
        }
        return undefined;
    }


    function emitLoginButtonPressed()
    {
        loginButtonPressed(root)
    }

    // TODO: implement
    function displayError(message)
    {
        console.log(message);
    }

    Column {
        id: toolColumn

        spacing: 8

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        TextField {
            id: usernameText
            placeholderText: "Username"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ComboBox {
            id: serverBox

            // NOTE: change the switch block in server() if the order
            // of these are changed as it uses the index to return the
            // server enum
            model: ["International", "General", "Young", "VIP", "KWS"]
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ComboBox {
            id: charBox

            // NOTE: change the switch block in character() if the order
            // of these are changed as it uses the index to return the
            // character enum
            model: ["Giko", "Shii", "Boon", "Christmas Giko", "Christmas Shii", "Furoshiki Giko", "Piamica Shii"]

            anchors.horizontalCenter: parent.horizontalCenter
        }

        ComboBox {
            id: stageBox

            anchors.horizontalCenter: parent.horizontalCenter

            model: [
                "Shrine",
                "Shrine Entrance",
                "Eel Track",
                "Schoolyard",
                "School Crossing",
                "School Classroom",
                "School Hallway",
                "School Cultural Room",
                "School Computer Lab",
                "Bar 774",
                "Drunker's Bar",
                "Underground Town",
                "Bar",
                "Bar Street",
                "Ramen Stand",
                "Admin Street",
                "Admin's Bar",
                "Developer's Lounge",
                "Banqueting Hall",
                "Cafe Vista",
                "Well A",
                "Well B",
                "Bus Stop",
                "Seashore",
                "Radio Studio",
                "Radio Room 1",
                "Dressing Room",
                "Backstage",
                "Beat Giko",
                "G-Squid",
                "Hilltop",
                "Hilltop Stairway"]
        }

        ToolSeparator {
            width: parent.width
            orientation: Qt.Horizontal

            anchors.horizontalCenter: parent.horizontalCenter
        }

        CheckBox {
            id: useProxyCheckBox

            onClicked: {
                if (useProxyCheckBox.checked) {
                    proxyText.enabled = true;
                } else {
                    proxyText.enabled = false;
                }
            }

            text: "Use proxy"
            checked: false

            anchors.horizontalCenter: parent.horizontalCenter
        }

        TextField {
            id: proxyText

            enabled: false
            placeholderText: "SOCKS5 Server"

            anchors.horizontalCenter: parent.horizontalCenter
        }

        ToolSeparator {
            width: parent.width
            orientation: Qt.Horizontal

            anchors.horizontalCenter: parent.horizontalCenter
        }

        CheckBox {
            id: saveLoginInfoCheckBox

            text: "Save login options"

            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item {
            id: statusRow

            width: parent.width
            height: busyIndicator.height * 0.5
            visible: true

            anchors.horizontalCenter: parent.horizontalCenter

            BusyIndicator {
                id: busyIndicator

                scale: 0.5
                visible: false

                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
            }

            Label {
                id: statusLabel

                text: ""
                visible: false

                anchors {
                    left: busyIndicator.right
                    leftMargin: 8
                    verticalCenter: parent.verticalCenter
                }
            }
        }

        Button {
            id: loginButton

            onClicked: {
                emitLoginButtonPressed()
            }

            text: "Login"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
