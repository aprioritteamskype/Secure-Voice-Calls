import QtQuick 2.0

Rectangle {
    color: "#b9e2c7"
    anchors.fill: parent
    gradient: Gradient {
        GradientStop {
            SequentialAnimation on color {
                loops: Animation.Infinite
                ColorAnimation { from: "#b9e2c7"; to: "#210fe6"; duration: 8000}
                ColorAnimation { from: "#210fe6"; to: "#b9e2c7"; duration: 8000}
            }

            position: 0.20;
            color: "#b9e2c7";
        }
        GradientStop {
            SequentialAnimation on color {
                loops: Animation.Infinite
                ColorAnimation { from: "#210fe6"; to: "#b9e2c7"; duration: 8000}
                ColorAnimation { from: "#b9e2c7"; to: "#210fe6"; duration: 8000}
            }
            position: 1.00;
            color: "#210fe6";
        }
    }
}
