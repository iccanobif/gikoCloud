import QtQuick 2.7
import QtQuick.Controls 2.1
import "Colors.js" as Colors
import "Utils.js" as Utils

DarkBox {
    id: root

    signal messageReady(string text)
    signal closed(QtObject obj);

    property alias userList: userList

    property var connection
    property var clientId
    property var loginCount
    property var tripcode
    property var stage
    property var character
    property bool myIdReceived: false

    function htmlEscape(msg)
    {
        return String(msg).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
    }

    function onLoginCountReceived(count)
    {
        loginCount = count;
    }

    function addMessage(time, uid, name, msg)
    {
        if (msg == "" || msg == null || msg == undefined) {
            return;
        }

        log.addMessage(time, uid, name, htmlEscape(msg));
    }

    function getUsername(uid)
    {
        return userList.getUsername(uid);
    }

    function onPlayerLeft(uid, leaveAct)
    {
        log.logStatus(userList.getUsername(uid) + " has left " + stage + ": " + leaveAct);
        userList.removeUser(uid);
    }

    function onPlayerMessageReceived(uid, msg)
    {
        addMessage(Qt.formatDateTime(new Date(), "hh:mm:ss"), uid, getUsername(uid), msg);
    }

    function addUser(uid, name)
    {
        name = htmlEscape(name)

        if (myIdReceived) {
            log.logStatus(name + " has entered " + stage);
        }

        userList.addUser(uid, name)

        if (uid == clientId) {
            myIdReceived = true;
        }
    }

    function onStageLoginInfoReceived(stages, broadcasters, loginCounts)
    {
        var table = "<table border=\"0.5\" cellpadding=\"4\"><tr>";

        table += "<th>Stage</th>";
        table += "<th>Broadcaster</th>";
        table += "<th>Login Count</th></tr>";

        for (var i = 0; i < stages.length; ++i) {
            table += "<tr>";
            table += "<td>" + Utils.escapeHtmlEntities(stages[i]) + "</td>";
            table += "<td>" + Utils.escapeHtmlEntities(broadcasters[i]) + "</td>";
            table += "<td>" + loginCounts[i] + "</td>";
            table += "</tr>";
        }

        table += "</table>";
        log.logStatus(table);
    }

    function connectSlots(conn)
    {
        connection = conn;

        conn.stageEntrySuccessful.connect(onStageEntrySuccessful);
        conn.playerMessageReceived.connect(onPlayerMessageReceived);
        conn.playerNameReceived.connect(addUser);
        conn.playerLeft.connect(onPlayerLeft);

        conn.loginCountChanged.connect(onLoginCountReceived);

        conn.error.connect(displayError);
        conn.disconnected.connect(onDisconnected);

        conn.stageLoginInfoReceived.connect(onStageLoginInfoReceived);
    }

    function onStageEntrySuccessful()
    {
        log.logStatus("You have entered " + stage);
    }

    function disconnectSlots()
    {
        if (connection != null || connection != undefined) {
            connection.stageEntrySuccessful.disconnect(onStageEntrySuccessful);
            connection.playerMessageReceived.disconnect(onPlayerMessageReceived);
            connection.playerNameReceived.disconnect(addUser);
            connection.playerLeft.disconnect(onPlayerLeft);

            connection.loginCountChanged.disconnect(onLoginCountReceived);

            connection.error.disconnect(displayError);
            connection.disconnected.disconnect(onDisconnected);

            connection.stageLoginInfoReceived.disconnect(onStageLoginInfoReceived);

            connection.close();
            connection.destroy();
            connection = null;
        }
    }

    function onDisconnected()
    {
        textArea.enabled = false;
        log.logError("Connection closed");
    }

    function displayError(msg)
    {
        console.log(msg);
        log.logError(msg);
    }

    function enterStage(sid)
    {
        var stagename = Utils.stageIdToName(sid);
        if (root.stage == stagename) {
            log.logError("You are already in '" + sid + "'.");
        } else if (connection.enterStage(sid, character) == false) {
            log.logError("The stage id '" + sid + "' is invalid.");
        } else {
            log.logStatus("You have left " + stage);
            userList.model.clear();
            stage = stagename;
            myIdReceived = false;
        }
    }

    function requestStageLoginInfo()
    {
        connection.requestStageLoginInfo();
    }

    function emitMessageReady()
    {
        if (!Utils.parseMessage(textArea.text, requestStageLoginInfo, enterStage)) {
            connection.sendClientMessage(textArea.text);
            messageReady(textArea.text);
        }
        textArea.text = "";
    }

    UserList {
        id: userList

        width: 250
        height: parent.height

        anchors.right: parent.right
        anchors.top: parent.top
    }

    Log {
        id: log

        anchors.top: parent.top
        anchors.right: userList.left
        anchors.left: parent.left
        anchors.margins: 8

        height: parent.height - (wrapper.height + 24 + 24)
    }

    NiceButton {
        id: sendButton
        text: "Send"

        anchors.top: log.bottom
        anchors.right: userList.left
        anchors.margins: 20

        width: 80
        height: 40

        onClicked: {
            emitMessageReady()
        }
    }

    NiceButton {
        id: logoutButton
        text: "Logout"

        anchors.top: sendButton.bottom
        anchors.right: userList.left
        anchors.topMargin: 15
        anchors.rightMargin: 20

        width: 80
        height: 40

        onClicked: {
            root.disconnectSlots();
        }
    }

    NiceButton {
        id: closeButton
        text: "Close"

        anchors.top: logoutButton.bottom
        anchors.right: userList.left

        anchors.topMargin: 15
        anchors.rightMargin: 20

        width: 80
        height: 40

        onClicked: {
            root.disconnectSlots();
            root.closed(root);
        }
    }

    DarkBox {
        id: wrapper
        anchors.top: log.bottom
        anchors.left: parent.left
        anchors.right: sendButton.left
        anchors.margins: 20

        height: 150

        Flickable {
            id: flick
            anchors.fill: parent

            TextArea.flickable: TextArea {
                id: textArea

                placeholderText: "Enter your text here"
                color: enabled ? Colors.normalTextColor : Colors.disabledTextColor

                wrapMode: TextArea.Wrap
                padding: 8

                background: Rectangle {
                    color: Colors.textAreaColor
                    border.color: textArea.focus ? Colors.activeBorderColor : Qt.lighter(color, 2.0);
                }

                Keys.onReturnPressed: {
                    if ((event.modifiers & Qt.ShiftModifier) != 0) {
                        emitMessageReady();
                    }
                }

                Keys.onEscapePressed: {
                    textArea.text = ''
                }
            }

            ScrollBar.vertical: ScrollBar {}
        }
    }
}
