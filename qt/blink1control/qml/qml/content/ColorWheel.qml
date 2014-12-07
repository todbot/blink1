import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import CursorTools 1.0
Rectangle {
    id: rec
    width: 450
    height: 400
    color: "transparent"

    property int red: 255
    property int green: 0
    property int blue: 0
    property int hue: 0
    property int saturation: 255
    property int brightness: 255
    property string text: ""
    property int cR:0
    property int cG:0
    property int cB:0
    property string currentColorName: "000000"
    property bool editMode
    property int indexOfColorPattern: 0
    property bool start: false
    property real time: 1


    Component.onCompleted: {
        var text;
        var multiplerB = 42;
        var multiplerH = 32;
        var h = 0;
        var s = 0;
        var b = 0;
        for(var y=5; y>=0; y--)
        {
            b = (y+1) * multiplerB - 21;
            for(var x=0; x<12; x++) {
                if(x == 0) {
                    s = 0;
                    h = 0;
                }
                else {
                    s = 255;
                    h = (x-1) * multiplerH;
                }
                hsl2Rgb(h,s,b);
                text = "#";
                text += intToHex(red);
                text += intToHex(green);
                text += intToHex(blue);
                modelSimpleColors.append({"colorCode": text})
            }
        }

        red = 0;
        green = 0;
        blue = 0;
        rec.start = true;
        setTime(1.0)
        setColorRgb(red,green,blue)
    }


    Rectangle {
        id: rectangleSimpleColors
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.top: parent.top
        width: 230
        height: 130
        color: "transparent"

        GridView {
            id: gridSimpleColors
            property int borderIndex: -1

            x: 0
            y: 0
            width: parent.width
            height: parent.height
            model: modelSimpleColors
            interactive: false
            delegate: Item {
                Row {
                    x:1
                    y:1
                    Rectangle{
                        width: 20
                        height: 20
                        color: "transparent"
                        Rectangle {
                            id: rectCol
                            anchors.centerIn: parent
                            width: 16
                            height: 16
                            radius: 2
                            color: colorCode
                            border.width: (gridSimpleColors.borderIndex == index)?3:0//parseInt(parent.width / 5):0
                            border.color: "black"
                            Rectangle{
                                visible: mw.checkIfColorIsTooDark(colorCode) && gridSimpleColors.borderIndex == index
                                anchors.centerIn: parent
                                color: "transparent"
                                border.width: 2
                                border.color: "white"
                                width: parent.width-2
                                height: parent.height-2
                                radius: 2
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    setColorRgb(parseInt(parent.color.r*255), parseInt(parent.color.g*255), parseInt(parent.color.b*255));
                                    gridSimpleColors.borderIndex = index
                                }
                            }
                        }
                        MouseArea {
                            cursorShape: Qt.PointingHandCursor
                            anchors.fill: parent
                            onClicked: {
                                setColorRgb(parseInt(rectCol.color.r*255), parseInt(rectCol.color.g*255), parseInt(rectCol.color.b*255));
                                gridSimpleColors.borderIndex = index
                            }
                        }
                    }
                    spacing: 20
                }
            }
            cellHeight: 18
            cellWidth: 18

        }

        ListModel {
            id: modelSimpleColors
        }


    }


    Image {
        id: imageColorWheel
        x: 220
        y: 140
        width: 180
        height: 180
        fillMode: Image.Stretch
        //source: "ColorWheelImg/transparent.png"
        Image{
            //anchors.centerIn: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenterOffset: -2
            anchors.verticalCenterOffset: 1
            source: "ColorWheelImg/gradient-circle.png"
            width: 200
            height: 200
        }
        Rectangle {
            id: pointer
            x: 85
            y: 85
            width: 10
            height: 10
            //source: "ColorWheelImg/pointer.png"
            border.width: 1
            border.color: "black"
            color: "transparent"
        }

        MouseArea {
            cursorShape: Qt.PointingHandCursor
            id: pointerArea
            x: 0
            y: 0
            width: 180
            height: 180
            onPressed: {
                if(!pointerArea.drag)
                    return;

                var promien = imageColorWheel.width/2;
                var newX = promien - pointerArea.mouseX;
                var newY = pointerArea.mouseY - promien;
                var odleglosc = Math.sqrt(newX*newX + newY*newY);
                if(odleglosc <= promien)
                {
                    pointer.x = pointerArea.mouseX-5;
                    pointer.y = pointerArea.mouseY-5;
                }
                else
                {
                    var proporcje = promien/odleglosc;
                    pointer.x = promien-5 - proporcje * newX;
                    pointer.y = proporcje * newY + promien-5;
                    odleglosc = promien;
                }

                var h = Math.ceil((getWheelCirclePos(newX,newY)+180)) % 360;
                var s = Math.ceil(odleglosc/promien*255);
                var v = Math.ceil(sliderValue.value);
                hsvtext.text = h + " " + s + " " + v;
                hsl2Rgb(h,s,v);
                spinRed.value = red;
                spinGreen.value = green;
                spinBlue.value = blue;
                textHex.text = currentColor.color = "#" + intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
                currentColorName=textHex.text

            }
            onPositionChanged: {
                if(!pointerArea.drag)
                    return;

                var promien = imageColorWheel.width/2;
                var newX = promien - pointerArea.mouseX;
                var newY = pointerArea.mouseY - promien;
                var odleglosc = Math.sqrt(newX*newX + newY*newY);
                if(odleglosc <= promien)
                {
                    pointer.x = pointerArea.mouseX-5;
                    pointer.y = pointerArea.mouseY-5;
                }
                else
                {
                    var proporcje = promien/odleglosc;
                    pointer.x = promien-5 - proporcje * newX;
                    pointer.y = proporcje * newY + promien-5;
                    odleglosc = promien;
                }

                var h = Math.ceil((getWheelCirclePos(newX,newY)+180)) % 360;
                var s = Math.ceil(odleglosc/promien*255);
                var v = Math.ceil(sliderValue.value);
                hsvtext.text = h + " " + s + " " + v;
                hsl2Rgb(h,s,v);
                spinRed.value = red;
                spinGreen.value = green;
                spinBlue.value = blue;
                textHex.text = currentColor.color = "#" + intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
                currentColorName=textHex.text

            }
        }

        Rectangle {
            id: rectangle1
            x: 181
            y: 0
            width: 22
            height: 180
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "transparent"
                }

                GradientStop {
                    position: 1
                    color: "transparent"
                }
            }
        }

        Slider {
            id: sliderValue
            x: 191
            y: 0
            width: 22
            height: parent.height
            value: 255
            maximumValue: 255
            orientation: 0
            CursorShapeArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
            }
            style: SliderStyle {
                groove: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 8
                    color: "transparent"
                    radius: 8
                }
                handle: Rectangle {
                    anchors.centerIn: parent
                    color: control.pressed ? "white" : "lightgray"
                    //color: "transparent"
                    border.color: "gray"
                    border.width: 2
                    width: 20
                    height: 20
                    radius: 12
                }
            }

            onValueChanged: {
                if(!sliderValue.updateValueWhileDragging)
                    return;
                if(!rec.start) return;
                if(!sliderValue.pressed) return;
                var promien = imageColorWheel.width/2;
                var newX = promien - pointer.x-5;
                var newY = pointer.y - promien+5;
                var odleglosc = Math.sqrt(newX*newX + newY*newY);

                var h = Math.ceil((getWheelCirclePos(newX,newY)+180)) % 360;
                var s = Math.ceil(odleglosc/promien*255);
                var v = Math.ceil(sliderValue.value);
                hsvtext.text = h + " " + s + " " + v;
                hsl2Rgb(h,s,v);
                spinRed.value = red;
                spinGreen.value = green;
                spinBlue.value = blue;
                textHex.text = currentColor.color = "#" + intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
                currentColorName=textHex.text
            }
        }

        Rectangle{
            anchors.centerIn: sliderValue
            width: sliderValue.width-5
            height: sliderValue.height-1
            z: sliderValue.z -1
            border.width: 1
            border.color: "lightgray"
            radius: 8
            gradient: Gradient{
                GradientStop{
                    color: "white"
                    position: 0
                }
                GradientStop{
                    color: "black"
                    position: 1
                }
            }
        }
    }


    Rectangle {
        id: rectangleRGB
        width: 107
        height: 250
        color: "#00000000"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 30

        Label {
            id: label1
            x: 0
            y: -5//9
            text: "Time (s):"
            //font.pointSize: if(mw.mac()) 11; else 8;
            font.pixelSize:11
        }

        SpinBox {
            CursorShapeArea{
                cursorShape: Qt.PointingHandCursor
                anchors.right: parent.right
                anchors.rightMargin: 3
                anchors.top: parent.top
                width: 13
                height: parent.height
            }

            id: spinTime
            //font.pointSize: (!mw.mac())?10:12
            font.pixelSize:12
            x: 47
            y: -6
            width: 62
            //height: 20
            maximumValue: 360
            decimals: 1
            value: 0
            stepSize: 0.1
            onValueChanged: {
                time = spinTime.value;
            }
        }

        Label {
            id: label2
            x: 31
            y: 40
            text: "R:"
            //font.pointSize:  if(mw.mac()) 11; else 8;
            font.pixelSize:11
        }

        SpinBox {
            CursorShapeArea{
                cursorShape: Qt.PointingHandCursor
                anchors.right: parent.right
                anchors.rightMargin: 3
                anchors.top: parent.top
                width: 13
                height: parent.height
            }
            id: spinRed
            //font.pointSize: (!mw.mac())?10:12
            font.pixelSize:12
            x: 47
            y: 36
            width: 62
            //height: 20
            maximumValue: 255
            value: 255

            onValueChanged: {
                if(!rec.start) return;
                if(!pointerArea.pressed && !sliderValue.pressed)// && !editMode)
                    rgbToHsl();
                gridSimpleColors.borderIndex = -1
            }
        }
    
        Label {
            id: label3
            x: 31
            y: 65//70
            text: "G:"
            //font.pointSize:  if(mw.mac()) 11; else 8;
            font.pixelSize:11
        }

        SpinBox {
            CursorShapeArea{
                cursorShape: Qt.PointingHandCursor
                anchors.right: parent.right
                anchors.rightMargin: 3
                anchors.top: parent.top
                width: 13
                height: parent.height
            }
            id: spinGreen
            //font.pointSize: (!mw.mac())?10:12
            font.pixelSize:12
            x: 47
            y: 62//66
            width: 62
            //height: 20
            maximumValue: 255
            onValueChanged: {
                if(!rec.start) return;
                if(!pointerArea.pressed && !sliderValue.pressed)// && !editMode)
                    rgbToHsl();
                gridSimpleColors.borderIndex = -1
            }
        }

        Label {
            id: label4
            x: 32
            y: 90//100
            text: "B:"
            //font.pointSize:  if(mw.mac()) 11; else 8;
            font.pixelSize:11
        }

        SpinBox {
            CursorShapeArea{
                cursorShape: Qt.PointingHandCursor
                anchors.right: parent.right
                anchors.rightMargin: 3
                anchors.top: parent.top
                width: 13
                height: parent.height
            }
            id: spinBlue
            //font.pointSize: (!mw.mac())?10:12
            font.pixelSize:12
            x: 47
            y: 88//96
            width: 62
            //height: 20
            maximumValue: 255
            onValueChanged: {
                if(!rec.start) return;
                if(!pointerArea.pressed && !sliderValue.pressed)// && !editMode)
                    rgbToHsl();
                gridSimpleColors.borderIndex = -1
            }
        }

        TextField {
            id: textHex
            x: 47
            y: 132//125
            width: 62
            height: 22
            text: "FF0000"
            //font.pointSize: (!mw.mac())?10:12
            font.pixelSize:12
            placeholderText: qsTr("Text Field")
            inputMask: ">HHHHHH"

            onAccepted: {
                gridSimpleColors.borderIndex = -1
                setColor(textHex.text);
            }
            onFocusChanged: {
                if(!textHex.focus) {
                    if(textHex.text.length != 6)
                        textHex.text = intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value)
                }
            }
        }

        Label {
            id: label5
            x: 0
            y: 134//129
            width: 42
            height: 14
            text: "HEX #"
            verticalAlignment: Text.AlignTop
            //font.pointSize:  if(mw.mac()) 11; else 8;
            font.pixelSize:11
            horizontalAlignment: Text.AlignRight
        }
    }

    Rectangle {
        id: currentColor
        x: 30
        y: 29
        width: 107
        height: 75
        color: "#ffffff"
        visible: false
    }

    Text {
        id: hsvtext
        x: 220
        y: 357
        width: 168
        height: 14
        text: qsTr("Text")
        font.pixelSize: 12
        visible: false
    }

    // to distinguish between user clicking and it being changed programmatically
    // (really, there must be a better way to do this)
    // FIXME: how to get all UI elements of color picker?
    // if this were a proper GUI control group, would be easy
    function isUserAction()
    {
        return (pointerArea.pressed || sliderValue.pressed );
    }

    function getDistance(x,y)
    {
        return Math.sqrt(x*x + x*x);
    }

    function getWheelCirclePos(x, y)
    {
        var angle;
        //        x = Math.ceil(x-(imageColorWheel.width/2));
        //        y = Math.ceil((imageColorWheel.height/2)-y);
        if(x<0 && y<0) {
            x = -x;
            y = -y;
            angle = Math.atan2(x,y) * 180 / Math.PI;
            angle = (angle+270) % 360;
        } else {
            angle = Math.atan2(x,y) * 180 / Math.PI;
            angle = (angle+90) % 360;
        }
        return Math.ceil(angle);
    }

    function hsl2Rgb(h,s,v)
    {
        h = h % 360;
        s = s / 255.0;
        v = v / 255.0;
        var r = 0;
        var g = 0;
        var b = 0;
        var c = v * s;
        var x = (h / 60) % 2 - 1;
        var tmp = 1.0;
        var tmp2 = Math.sqrt(x*x);
        tmp = tmp - tmp2;
        x = c * tmp;
        var m = v - c;
        if(h >= 0 && h < 60) {
            r = c;
            g = x;
            b = 0;
        } else if(h >= 60 && h < 120) {
            r = x;
            g = c;
            b = 0;
        } else if(h >= 120 && h < 180) {
            r = 0;
            g = c;
            b = x;
        } else if(h >= 180 && h < 240) {
            r = 0;
            g = x;
            b = c;
        } else if(h >= 240 && h < 300) {
            r = x;
            g = 0;
            b = c;
        } else if(h >= 300 && h < 360) {
            r = c;
            g = 0;
            b = x;
        }
        red = (r+m)*255;
        green = (g+m)*255;
        blue = (b+m)*255;
    }

    function rgb2Hsl(r,g,b)
    {
        r = r / 255.0;
        g = g / 255.0;
        b = b / 255.0;
        var cmax = Math.max(Math.max(r,g),b);
        var cmin = Math.min(Math.min(r,g),b);
        var delta = cmax - cmin;
        var h=0;
        var s=0;
        var v =0;
        if(cmax == r) {
            h = 60 * ( ((g-b)/delta) % 6);
        } else if(cmax == g) {
            h = 60 * ( ((b-r)/delta) + 2);
        } else if(cmax == b) {
            h = 60 * ( ((r-g)/delta) + 4);
        }
        if(delta == 0) {
            s = 0;
        } else {
            s = delta / cmax;
        }
        v = cmax;

        if(h < 0)
            h = 360+h;
        hue = h;
        saturation = s*255;
        brightness = v*255;
    }

    function intToHex(value)
    {
        value = Math.ceil(value);
        var hex = "";
        var reszta;
        var dzielenie;
        while(value != 0) {
            dzielenie =  parseInt(value / 16);
            reszta = Math.ceil(value - (dzielenie * 16));
            value = dzielenie;
            hex = i2str(reszta) + hex;
        }
        if(hex.length == 0)
            hex = "00";
        if(hex.length == 1)
            hex = "0" + hex;
        return hex;
    }
    function i2str(value) {
        if(value == 10) {
            return "A";
        } else if(value == 11) {
            return "B";
        } else if(value == 12) {
            return "C";
        } else if(value == 13) {
            return "D";
        } else if(value == 14) {
            return "E";
        } else if(value == 15) {
            return "F";
        } else
            return value;
    }
    function hexToInt(value)
    {
        var v = "";
        v = value;
        var arg1 = str2i(v.charAt(0));
        var arg2 = str2i(v.charAt(1));
        var wynik = arg1*16 + arg2;
        return wynik;
    }
    function str2i(value)
    {
        if(value == 'A' || value == 'a') {
            return 10;
        } else if(value == 'B' || value == 'b') {
            return 11;
        } else if(value == 'C' || value == 'c') {
            return 12;
        } else if(value == 'D' || value == 'd') {
            return 13;
        } else if(value == 'E' || value == 'e') {
            return 14;
        } else if(value == 'F' || value == 'f') {
            return 15;
        } else
            return Math.ceil(parseInt(value));
    }

    function setTime(t) {
        var x = t;
        spinTime.value = parseFloat(t);
    }

    function setColor(hex) {
        text = hex;
        currentColorName = hex;

        switch(text.length)
        {
        case 6:
            cR = hexToInt(text.charAt(0) + text.charAt(1));
            cG = hexToInt(text.charAt(2) + text.charAt(3));
            cB = hexToInt(text.charAt(4) + text.charAt(5));
            currentColor.color = "#" + hex;
            break;
        case 7:
            cR = hexToInt(text.charAt(1) + text.charAt(2));
            cG = hexToInt(text.charAt(3) + text.charAt(4));
            cB = hexToInt(text.charAt(5) + text.charAt(6));
            currentColor.color = hex;
            break;
        default:
            return;
        }
        spinRed.value = cR;
        spinGreen.value = cG;
        spinBlue.value = cB;
        rgb2Hsl(cR, cG, cB);

        setPointerByHS(hue, saturation);
        sliderValue.value = brightness;
    }

    function setPointerByHS(h,s)
    {
        var promien = imageColorWheel.width/2;
        var odleglosc = (s/255.0)*promien;

        //h = h ;
        var angle = (h)*Math.PI/180.0;
        var x = Math.cos(angle);// * 180 / Math.PI;
        x = x * odleglosc;

        var y = Math.sin(angle);// * 180 / Math.PI;
        y = y * odleglosc;

        if(h<=90)
        {
            x += 5;
            y += 5;
            pointer.x = promien-Math.ceil(x);
            pointer.y = promien-Math.ceil(y);
        }
        else
        {
            if(h<=180)
            {
                //x = -x;
                x += 5;
                y += 5;
                pointer.x = promien-Math.ceil(x);
                pointer.y = promien-Math.ceil(y);
            }
            else
            {
                if(h<=270)
                {
                    //x = -x;
                    //y = -y;
                    x += 5;
                    y += 5;
                    pointer.x = promien-Math.ceil(x);
                    pointer.y = promien-Math.ceil(y);
                }
                else
                {
                    //y = -y;
                    x += 5;
                    y += 5;
                    pointer.x = promien-Math.ceil(x);
                    pointer.y = promien-Math.ceil(y);
                }
            }
        }

        hsvtext.text = pointer.x + " " + pointer.y;
        hue = (hue+1)%360;
    }


    function rgbToHsl() {
        rgb2Hsl(spinRed.value, spinGreen.value, spinBlue.value);

        //sliderHue.value = hue;
        //sliderSaturation.value = saturation;
        //sliderBrightness.value = brightness;
        var sat=saturation/255.0
        //console.log("SAT:"+sat)
        //console.log("HUE:"+hue)
        //var tmpy=Math.sin(hue*Math.PI/180.0)*(imageColorWheel.width/2*sat)
        //var tmpx=Math.cos(hue*Math.PI/180.0)*(imageColorWheel.width/2*sat)
        //console.log(tmpx+"-"+tmpy)
        //pointer.x=imageColorWheel.width/2-tmpx
        //pointer.y=imageColorWheel.width/2-tmpy
        setPointerByHS(hue,saturation)

        sliderValue.value = brightness;

        currentColor.color = "#" + intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
        currentColorName = textHex.text = intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
    }
    function setColorRgb(r, g, b) {
        spinRed.value = r;
        spinGreen.value = g;
        spinBlue.value = b;

        rgbToHsl();

        currentColor.color = "#" + intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
        textHex.text = intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
        currentColorName = textHex.text;
    }
    function setQColorAndTime(color, time)
    {
        var r = parseInt(color.r * 255);
        var g = parseInt(color.g * 255);
        var b = parseInt(color.b * 255);

        spinRed.value = r;
        spinGreen.value = g;
        spinBlue.value = b;

        rgb2Hsl(r,g,b);
        //sliderHue.value = hue;
        //sliderSaturation.value = saturation;
        setPointerByHS(hue,saturation)
        sliderValue.value = brightness;


        //        colorRectangle.rotation = sliderHue.value + 315;
        //        colorSelector.rotation = sliderHue.value - 90;

        currentColor.color = "#" + intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
        textHex.text = intToHex(spinRed.value) + intToHex(spinGreen.value) + intToHex(spinBlue.value);
        currentColorName = textHex.text;
        spinTime.value = parseFloat(time);
    }
    function getCurrentColor() {
        return "#" + currentColorName;
    }
    function getCurrentTime() {
        return spinTime.value;
    }
}
