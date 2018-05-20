import QtQuick 2.7
import "Colors.js" as Colors

Rectangle {
        color: (parent.down || parent.pressed) ? Colors.activeBorderColor : Qt.darker(Colors.backgroundColor, parent.hovered ? 2.0 : 1.0)
        border.color: (parent.hovered || parent.down || parent.pressed) ? Qt.lighter(Colors.activeBorderColor, ((parent.down || parent.pressed) ? 1.5 : 1.0))  : Qt.lighter(Colors.backgroundColor, 2.0)
}
