import QtQuick 2.0
import QtQuick.Controls 1.4
import cni.MTC 1.0

Rectangle {
    width: 500
    height: 500
    color: "white"

    Component.onCompleted:
    {
        AppController.updateInfo.connect(onDataReception);
    }

    function onDataReception(_data)
    {
        title.text = _data;
    }


    Row
    {

        Button
        {
            text: "Start"
            onClicked:
            {
                if(text=="Start")
                {
                    text = "Stop";
                    AppController.start(configEdit.text);
                }
                else if(text=="Stop")
                {
                    text = "Start";
                    AppController.stop();
                }
            }
        }

        TextField
        {
            id: configEdit
            text: "pCAT" // loadSettings
        }
    }


    Column {
        anchors.fill: parent

        Image {
            id: happyButton
            height: parent.height / 5
            fillMode: Image.PreserveAspectFit
            source: "../images/cni-logo.png"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Figure
        {
            objectName: "fig"
            id: fig
            width: parent.width
            height: 400
        }
        Text {
            id: title
            color: "black"
            font.pixelSize: parent.width / 30
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
