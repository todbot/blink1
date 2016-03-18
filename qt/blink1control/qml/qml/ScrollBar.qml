import QtQuick 2.0

Rectangle {
    property variant flickable

    property bool vertical: true

    property bool hideScrollBarsWhenStopped: true

    property int scrollbarWidth: 7

    color: "black"
    radius: vertical ? width/2 : height/2

    function sbOpacity()
    {
        if (!hideScrollBarsWhenStopped) {
            return 0.5;
        }

        return (flickable.flicking || flickable.moving) ? (vertical ? (height >= parent.height ? 0 : 0.5) : (width >= parent.width ? 0 : 0.5)) : 0;
    }

    opacity: sbOpacity()

    width: vertical ? scrollbarWidth : flickable.visibleArea.widthRatio * parent.width
    height: vertical ? flickable.visibleArea.heightRatio * parent.height : scrollbarWidth
    x: vertical ? parent.width - width : flickable.visibleArea.xPosition * parent.width
    y: vertical ? flickable.visibleArea.yPosition * parent.height : parent.height - height

    Behavior on opacity { NumberAnimation { duration: 200 }}
}
