import QtQuick 2.7
import QtQuick.Controls 2.1
import "Colors.js" as Colors

DarkBox {
    id: root

    property alias view: listView
    property alias model: listModel

    function addUser(uid, name)
    {
        var user = {};

        user.uid = String(uid);
        user.name = String(name).replace("\n", "").replace("\r", "");

        model.append(user);
    }

    function getUsername(uid)
    {
        uid = String(uid);

        for (var i = 0; i < model.count; ++i) {
            var item = model.get(i);
            if (item.uid == uid) {
                return item.name;
            }
        }

        return undefined;
    }

    function removeUser(uid)
    {
        uid = String(uid);

        for (var i = 0; i < model.count; ++i) {
            var item = model.get(i);
            if(item.uid == uid) {
                model.remove(i);
            }
        }
    }

    ToolBar {
        id: header
        anchors.top: parent.top
        width: parent.width

        background: DarkBox {}

        Text {
            text: "Users"
            color: enabled ? Colors.normalTextColor : Colors.disabledTextColor
            font.pixelSize: 16

            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    ListView {
        id: listView

        anchors.top: header.bottom
        anchors.bottom: parent.bottom;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 8

        spacing: 4
        clip: true

        model: listModel
        delegate: listDelegate

        ScrollIndicator.vertical: ScrollIndicator{}
    }

    ListModel {
        id: listModel
    }

    Component {
        id: listDelegate

        DarkBox {
            id: wrapper

            height: 24
            width: listView.width

            Row {
                id: row

                anchors.left: parent.left
                anchors.leftMargin: 12

                height: parent.height
                width: parent.width

                spacing: 8

                Text {
                    id: uidText

                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    width: 50

                    text: uid
                    elide: Text.ElideRight
                    color: enabled ? Colors.normalTextColor : Colors.disabledTextColor
                }

                Text {
                    id: nameText

                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter

                    width: 200 < parent.width - (58 + 16 + 8) ? 200 : parent.width - (58 + 16 + 8)

                    text: name
                    elide: Text.ElideRight
                    color: enabled ? Colors.normalTextColor : Colors.disabledTextColor
                }
            }
        }
    }
}
