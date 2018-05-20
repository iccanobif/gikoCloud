import QtQuick 2.7
import QtQuick.Controls 2.1


DarkBox {
    id: root

    property alias view: listView
    property alias model: listModel
    property alias currentIndex: listView.currentIndex
    property alias currentItem: listView.currentIndex
    property alias count: listView.count

    signal clicked(int index);

    function addConversation(area, stage)
    {
        var conv = {};

        conv.area = area;
        conv.stage = stage;

        model.append(conv);
    }

    ToolBar {
        id: header
        anchors.top: parent.top
        width: parent.width

        background: DarkBox {}

        Text {
            text: "Conversations"
            color: "#cfcfc2"
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

        Rectangle {
            color: ListView.isCurrentItem ? "#2d5c76" : "#31363b"
            border.color: Qt.lighter(color, 1.5)

            height: 24
            width: listView.width

            Row {
                spacing: 8
                anchors.left: parent.left
                anchors.leftMargin: 12

                height: parent.height
                width: parent.width

                Text {
                    color: "#cfcfc2"
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: area
                    width: 100
                    elide: Text.ElideRight
                }

                Text {
                    color: "#cfcfc2"
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter

                    text: stage
                    width: 200 < parent.width - (68 + 16 + 8) ? 200 : parent.width - (68 + 16 + 8)
                    elide: Text.ElideRight
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent

                onClicked: {
                    root.clicked(index);
                }
            }
        }
    }
}
