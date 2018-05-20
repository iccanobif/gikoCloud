import QtQuick 2.7
import "Colors.js" as Colors

Text {
    id: root

    text: parent.text
    font: parent.font
    color: (parent.down || parent.pressed) ? Colors.normalTextColor : parent.enabled ? Colors.inactiveTextColor : Colors.disabledTextColor;
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
}
