import QtQuick 2.7
import QtQuick.Controls 2.1
import "Colors.js" as Colors

DarkBox {
    id: root
    color: Colors.logBackgroundColor

    function addMessage(time, id, name, msg)
    {
        var text = "<span style=\"color:" + Colors.timeTextColor + ";\">[" + time + "]</span> ";
        text += "<span style=\"color:" + Colors.uidTextColor + ";\">" + id + " </span>";
        text += "<span style=\"color:" + Colors.userTextColor + ";\">" + name + "</span> : ";
        text += msg + "</br>";

        textEdit.append(text);
    }

    function logStatus(msg)
    {
        var time = Qt.formatDateTime(new Date(), "hh:mm:ss")
        var text = "<span style=\"color:" + Colors.notifyTextColor + ";\">[" + time + "] ";
        text += "NOTICE: " + msg + "</span></br>";
        textEdit.append(text);
    }

    function logError(msg)
    {
        var time = Qt.formatDateTime(new Date(), "hh:mm:ss");
        var text = "<span style=\"color:" + Colors.errorTextColor + ";\">[" + time + "] ";
        text += "ERROR: " + msg + "</span></br>"
        textEdit.append(text)
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: textEdit.width
        contentHeight: textEdit.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        TextEdit {
            id: textEdit

            color: enabled ? Colors.normalTextColor : Colors.disabledTextColor

            width: root.width;

            textFormat: Text.RichText
            wrapMode: TextEdit.Wrap
            padding: 8

            readOnly: true
            selectByKeyboard: true
            selectByMouse: true
        }

        ScrollIndicator.vertical: ScrollIndicator{}
    }
}
