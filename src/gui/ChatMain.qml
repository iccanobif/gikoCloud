import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.1
import "Colors.js" as Colors

ApplicationWindow {
    id: appWindow

    visible: true
    width: 1000
    height: 600

    function onConnectionFailed(source)
    {
        removeView(source);
        if (swipeView.count == 0) {
            addLoginForm();
        }
    }

    function onClosed(source) {
        source.closed.disconnect(onClosed);
        removeView(source);
        if (swipeView.count == 0) {
            addLoginForm();
        }
    }

    function doneLogin(source)
    {
        var convo = addConversation(source.serverText, source.stageText);
        convo.character = source.character();
        convo.clientId = source.clientId;
        convo.loginCount = source.loginCount;
        convo.tripcode = source.tripcode;
        convo.stage = source.stageText;
        convo.connectSlots(source.connection);
        convo.closed.connect(onClosed);

        source.disconnectSlots();
        source.loginCompleted.disconnect(doneLogin);
        source.loginButtonPressed.disconnect(doLogin);
        source.connectionFailed.disconnect(onConnectionFailed);

        removeView(source);
    }

    function doLogin(loginForm)
    {
        loginForm.loginButton.enabled = false;

        var conn = Qt.createQmlObject("import CutiePoi.Connection 1.0; CPConnection{}", appWindow);
        loginForm.connectSlots(conn);
        loginForm.connectToServer();
    }

    function removeView(view)
    {
        for (var i = 0; i < swipeView.count; ++i) {
            if (swipeView.itemAt(i) == view) {
                if (swipeView.currentIndex == i) {
                    if (i == swipeView.count - 1) {
                        swipeView.decrementCurrentIndex();
                    } else {
                        swipeView.incrementCurrentIndex();
                    }
                }

                swipeView.removeItem(i);
                conversations.model.remove(i);
                view.destroy();
            }
        }
    }

    function addConversation(server, stage)
    {
        var compo = Qt.createComponent("ConversationPage.qml");
        var convo = compo.createObject(swipeView);
        conversations.addConversation(server, stage);
        return convo;
    }

    function addLoginForm()
    {
        var component = Qt.createComponent("Login.qml");
        var form = component.createObject(swipeView);
        conversations.addConversation("Gikopoi", "Login");

        form.loginButtonPressed.connect(doLogin);
        form.loginCompleted.connect(doneLogin);
        form.connectionFailed.connect(onConnectionFailed);
    }

    function onConversationListClicked(index)
    {
        swipeView.currentIndex = index;
    }

    Component.onCompleted: {
        conversations.clicked.connect(onConversationListClicked);
        addLoginForm();
    }

    DarkBox {
        anchors.top: parent.top
        anchors.left: conversations.right
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        SwipeView {
            id: swipeView
            anchors.fill: parent
        }
    }

    ConversationList {
        id: conversations
        currentIndex: swipeView.currentIndex

        width: 300
        height: parent.height - (addConversationBar.height)

        anchors.left: parent.left
        anchors.top: parent.top
    }

    ToolBar {
        id: addConversationBar

        width: conversations.width
        anchors.top: conversations.bottom
        anchors.left: parent.left

        background: DarkBox {}

        ToolButton {
            id: addButton

            anchors.centerIn: parent
            width: 64

            hoverEnabled: true

            font.pixelSize: 32
            font.bold: true

            text: "+"

            background: ButtonBackground {
                border.color: color
            }

            onPressed: {
                addLoginForm();
            }

            contentItem: TextContext {}
        }
    }
}
