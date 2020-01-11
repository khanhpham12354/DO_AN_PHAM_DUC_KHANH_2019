from PyQt5 import QtWidgets, uic, QtGui, QtCore
import sys
import time
from PyQt5.QtWidgets import QMessageBox, QFileDialog, QAction, QGroupBox, QTableWidget, QTableWidgetItem, QWidget
from PyQt5.QtCore import QTimer, QTime, QThread, pyqtSignal, QDate, Qt
from PyQt5.QtGui import QPixmap, QCloseEvent, QColor
import os
from datetime import datetime
import urllib.request
import socket
import serial
import serial.tools.list_ports
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
import random
import db_handler as DB
import pyqtgraph


App = QtWidgets.QApplication([])

app = uic.loadUi("guis\\main.ui")

check = 0


def check_internet():
    try:
        urllib.request.urlopen('http://google.com')  # Python 3.x
        return True
    except:
        return False


class TimeAxisItem(pyqtgraph.AxisItem):
    def tickStrings(self, values, scale, spacing):
        return [datetime.fromtimestamp(value) for value in values]


def Table_init():
    global app, model
    model = QStandardItemModel(app)
    model.setHorizontalHeaderLabels(
        ['Nhiệt độ', 'Độ ẩm', 'Độ ẩm đất', 'Tín hiệu', 'Thời gian'])
    header = app.tableView.horizontalHeader()
    stylesheet = "::section{font: 75 14pt \"Courier New\";color: \"#0000ff\"}"
    header.setStyleSheet(stylesheet)
    app.tableView.resizeColumnsToContents()
    header.setDefaultAlignment(Qt.AlignHCenter)
    app.tableView.setModel(model)

    app.tableView.setEditTriggers(QAbstractItemView.NoEditTriggers)

# val is list of lists


def update_table(val):
    global app, model
    for i in range(0, len(val)):
        model.removeRow(i)
        row = []
        for item in range(1, len(val[i])):
            cell = QStandardItem(str(val[i][item]))
            cell.setTextAlignment(Qt.AlignHCenter)
            row.append(cell)
        model.appendRow(row)


def load_data():
    global app, model, check, pen, pen1, pen2, graph, ptr1, curve, ptr1, curve1, curve2
    if(check == 0):
        values = DB.Query(5)
        if (len(values) > 0):
            payload = {
                'battery': int(values[len(values)-1][0]),
                'temp': int(values[len(values)-1][1]),
                'hum': int(values[len(values)-1][2]),
                'soil': int(values[len(values)-1][3]),
                'signal': int(values[len(values)-1][4])
            }
            hour = []
            temp = []
            hum = []
            soil = []
            for i in range(0, len(values)):
                datetime_object = datetime.strptime(
                    str(values[i][5][2:]), '%y-%m-%d %H:%M:%S')
                timestamp = datetime.timestamp(datetime_object)
                hour.append(timestamp)
                temp.append(int(values[i][0]))
                hum.append(int(values[i][1]))
                soil.append(int(values[i][2]))
            update_progress(payload)
            update_table(values)

            graph.plot(x=hour, y=temp, pen=pen)
            graph.plot(x=hour, y=hum, pen=pen1)
            graph.plot(x=hour, y=soil, pen=pen2)


def btn_clear():
    global graph
    graph.clear()


def graph_init():
    global app, pen, pen1, pen2, graph, ptr1, curve, curve1, curve2
    app.btn_clear.clicked.connect(btn_clear)
    date_axis = TimeAxisItem(orientation='bottom')
    graph = pyqtgraph.PlotWidget(axisItems={'bottom': date_axis}, parent=app)
    graph.move(680, 160)
    graph.resize(654, 511)
    graph.setYRange(0, 120, padding=0)
    graph.setLabel(
        'bottom', "<span style=\"color:yellow;font-size:15px\">Thời gian(s)</span>")
    graph.setLabel(
        'left', "<span style=\"color:yellow;font-size:15px\">Nhiệt độ(°C)\nĐộ ẩm không khí(%)\nĐộ ẩm đất(%)</span>")
    values = DB.Query(1)
    if (len(values) > 0):
        payload = {
            'battery': int(values[len(values)-1][0]),
            'temp': int(values[len(values)-1][1]),
            'hum': int(values[len(values)-1][2]),
            'soil': int(values[len(values)-1][3]),
            'signal': int(values[len(values)-1][4])
        }
        hour = []
        temp = []
        hum = []
        soil = []
        for i in range(0, len(values)):
            datetime_object = datetime.strptime(
                str(values[i][5][2:]), '%y-%m-%d %H:%M:%S')
            timestamp = datetime.timestamp(datetime_object)
            hour.append(timestamp)
            temp.append(int(values[i][0]))
            hum.append(int(values[i][1]))
            soil.append(int(values[i][2]))

        update_progress(payload)
        update_table(values)

        pen = pyqtgraph.mkPen(color=(255, 47, 6), width=1, style=Qt.SolidLine)
        pen1 = pyqtgraph.mkPen(color=(0, 0, 255), width=1, style=Qt.SolidLine)
        pen2 = pyqtgraph.mkPen(color=(202, 202, 0),
                               width=1, style=Qt.SolidLine)

        graph.plot(x=hour, y=temp, pen=pen, name='Nhiệt độ')
        graph.plot(x=hour, y=hum, pen=pen1, name='Độ ẩm không khí')
        graph.plot(x=hour, y=soil, pen=pen2, name='Độ ẩm đất')
    # Add Title

#  val is 1 json payload


def update_progress(val):
    app.progressBar_pin.setValue(val['battery'])
    app.progressBar_temp.setValue(val['temp'])
    app.progressBar_hum.setValue(val['hum'])
    app.progressBar_soil.setValue(val['soil'])
    app.progressBar_signal.setValue(val['signal'])


def read_data():
    global check
    check = 1
    now = datetime.now()
    timestamp = int(datetime.timestamp(now))
    now = datetime.fromtimestamp(timestamp)
    payload = {
        'battery': random.randint(0, 100),
        'temp': random.randint(0, 100),
        'hum': random.randint(0, 100),
        'soil': random.randint(0, 100),
        'signal': random.randint(0, 100),
        'time': now
    }
    if(check == 1):
        DB.insert_data(payload['battery'], payload['temp'],
                       payload['hum'], payload['soil'], payload['signal'], payload['time'])
    check = 0


def set_status_internet(status):
    global app
    if (status == 1):
        app.lbl_C.setStyleSheet(
            "QLabel {border-radius: 25px;border: 2px solid red;background-color: rgb(0, 255, 0)}")
        app.lbl_internet.setText("KẾT NỐI INTERNET")
        app.lbl_internet.setStyleSheet(
            "QLabel {color: green; border-radius: 9px;border: 2px solid green}")
    else:
        app.lbl_C.setStyleSheet(
            "QLabel {border-radius: 25px;border: 2px solid green;background-color: rgb(255, 0, 0)}")
        app.lbl_internet.setText("KHÔNG CÓ INTERNET")
        app.lbl_internet.setStyleSheet(
            "QLabel {color: red; border-radius: 9px;border: 2px solid red}")


if __name__ == "__main__":
    Time = QTimer()
    read = QTimer()
    Time.timeout.connect(load_data)
    read .timeout.connect(read_data)
    Time.start(5000)
    read.start(6000)
    Table_init()
    graph_init()
    if (check_internet() == True):
        set_status_internet(1)
    else:
        set_status_internet(0)
    app.show()
    sys.exit(App.exec())
