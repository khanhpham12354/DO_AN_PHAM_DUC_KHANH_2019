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
from Lora import Gateway
import paho.mqtt.client as mqtt
import json
import numpy as np


App = QtWidgets.QApplication([])

app = uic.loadUi("guis\\main.ui")

check = 0
# -------------------------------- Setup MQTT---------------------------------------------------

MQTT_HOST = '80.211.97.175'
MQTT_USER = 'BAbEt1qcJN97t1yUJ4tY'
# MQTT_PWD = '12354'
MQTT_TOPIC = 'v1/devices/me/telemetry'

#----------------------------------- Lora-------------------------------------------------------


def Lora_init():
    global GW,app
    ports = serial.tools.list_ports.comports()
    check_device = ''

    if(len(ports) > 0):
        for port in ports:
            if ("USB-SERIAL CH340" in str(port) or "USB-to-Serial" in str(port)):
                check_device = port.device
                print(check_device)
                break
        if (check_device != ''):
            GW = Gateway(check_device, 9600, 0.2)
            app.lbl_com.setText("ĐÃ KẾT NỐI "+str(check_device))
        else:
            print("Không đúng thiết bị!")
            QMessageBox.critical(app, "LỖI KẾT NỐI COM",
                                 "KHÔNG ĐÚNG THIẾT BỊ!")
            sys.exit()
    else:
        QMessageBox.critical(app, "LỖI KẾT NỐI COM",
                             "KHÔNG CÓ COM NÀO KẾT NỐI!")
        sys.exit()
    try:
        GW.open()
        GW.write_data("AT\r\n")
        print(GW.read_data())
        GW.write_data("AT+MODE=TEST\r\n")
        print(GW.read_data())
        GW.write_data("AT+TEST=RFCFG,433\r\n")
        print(GW.read_data())
        GW.write_data("AT+TEST=RXLRPKT\r\n")
        print(GW.read_data())
    except:
        QMessageBox.critical(app, "LỖI KẾT NỐI COM",
                             "KHÔNG THỂ KẾT NỐI")
    

def setCOM_status(status):
    global app
    if (status == 1):
        app.lbl_C_2.setStyleSheet(
            "QLabel {border-radius: 25px;border: 2px solid red;background-color: rgb(0, 255, 0)}")
    else:
        app.lbl_C_2.setStyleSheet(
            "QLabel {border-radius: 25px;border: 2px solid green;background-color: rgb(255, 0, 0)}")
# -------------------------------- check internet-----------------------------------------------

def check_internet():
    try:
        urllib.request.urlopen('http://google.com')  # Python 3.x
        return True
    except:
        return False
# ----------------------------------------------------------------------------------------------

class TimeAxisItem(pyqtgraph.AxisItem):
    def tickStrings(self, values, scale, spacing):
        return [datetime.fromtimestamp(value) for value in values]

# -------------------------------------Table init ----------------------------------------------
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

# --------------------------------load data from database --------------------------------------
def load_data():
    global app, model, check, pen, pen1, pen2, graph,client
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

            now = datetime.now()
            timestamp = int(datetime.timestamp(now))
            now = datetime.fromtimestamp(timestamp)
            old = datetime.timestamp(now)

            for i in range(0, len(values)):
                datetime_object = datetime.strptime(
                    str(values[i][5][2:]), '%y-%m-%d %H:%M:%S')
                timestamp = datetime.timestamp(datetime_object)
                hour.append(timestamp)
                temp.append(int(values[i][1]))
                hum.append(int(values[i][2]))
                soil.append(int(values[i][3]))

            if (hour[len(hour) - 1] != old):

                old = hour[len(hour) - 1]

                update_progress(payload)
                update_table(values)

                graph.plot(x=hour, y=temp, pen=pen)
                graph.plot(x=hour, y=hum, pen=pen1)
                graph.plot(x=hour, y=soil, pen=pen2)

        if (check_internet() == True):
            set_status_internet(1)
            client.publish(MQTT_TOPIC,
                           json.dumps(payload))
        else:
            set_status_internet(0)
        check = 1
            

# --------------------------------------------clear data ---------------------------------------
def btn_clear():
    global graph
    graph.clear()

# ---------------------------------------------graph init --------------------------------------
def graph_init():
    global app, pen, pen1, pen2, graph
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
            temp.append(int(values[i][1]))
            hum.append(int(values[i][2]))
            soil.append(int(values[i][3]))

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

# ---------------------------------------------update_progress --------------------------------------

def update_progress(val):
    app.progressBar_pin.setValue(val['battery'])
    app.progressBar_temp.setValue(val['temp'])
    app.progressBar_hum.setValue(val['hum'])
    app.progressBar_soil.setValue(val['soil'])
    app.progressBar_signal.setValue(val['signal'])
# ------------------------------------- read data from Module Lora --------------------------------------

def read_data():
    global check,GW
    check = 1
    now = datetime.now()
    timestamp = int(datetime.timestamp(now))
    now = datetime.fromtimestamp(timestamp)

    try:
        data = GW.read_data()
        if (len(data) != 0):
            for i in range(0, len(data)):
                data[i] = data[i].decode('utf-8')
            Str = data[0]
            Str1 = data[1]
            now = datetime.now()
            timestamp = int(datetime.timestamp(now))
            now = datetime.fromtimestamp(timestamp)
            payload = {
                'LEN': Str[Str.find('LEN:') + len("LEN:"):Str.find(',', Str.find('LEN:') + len("LEN:"))],
                'RSSI': Str[Str.find('RSSI:') + len("RSSI:"):Str.find(',', Str.find('RSSI:') + len("RSSI:"))],
                'SNR': Str[Str.find("SNR:") + len("SNR:"):len(Str) - 2],
                'DATA': bytes.fromhex(Str1[Str1.find("\"") +
                                        1: Str1.find("\"", 1 + int(Str1.find("\"")))]).decode('utf-8'),
                'TIME': now}
            print(payload)

            Data = payload['DATA'].split('_')
            if (int(payload['RSSI']) >= -54 and int(payload['RSSI']) <= 0):
                signal = 98
            elif (int(payload['RSSI']) >= -69 and int(payload['RSSI']) <= -55):
                signal = 75
            elif (int(payload['RSSI']) >= -79 and int(payload['RSSI']) <= -70):
                signal = 50
            elif (int(payload['RSSI']) >= -100 and int(payload['RSSI']) <= -80):
                signal = 25
            else:
                signal = 0
            x = float(Data[5])
            z = (x - 2.8)/(3.3-2.8)
            payload_1 = {
                'battery': int(z*100),
                'temp': Data[2],
                'hum': Data[3],
                'soil': Data[1],
                'signal': signal,
                'time': payload['TIME']
            }
            print(payload_1)
            if (check == 1):
                app.lbl_battery.setText(Data[5] + "V")
                app.lbl_rssi.setText(payload['RSSI']+"dBm")
                DB.insert_data(payload_1['battery'], payload_1['temp'],
                            payload_1['hum'], payload_1['soil'], payload_1['signal'], payload_1['time'])
        check = 0
    except :
        QMessageBox.critical(app, "LỖI KẾT NỐI COM",
                              "KHÔNG THỂ ĐỌC DỮ LIỆU TỪ MẠCH!")
        sys.exit()
    
# ------------------------------------Set status internet --------------------------------------
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
    Lora_init()
    Time = QTimer()
    read = QTimer()
    Time.timeout.connect(load_data)
    read .timeout.connect(read_data)
    Time.start(2000)
    read.start(1000)
    Table_init()
    graph_init()

    if (check_internet() == True):
        set_status_internet(1)
        client = mqtt.Client()
        client.username_pw_set(MQTT_USER)
        client.connect(MQTT_HOST, 1885)
        client.loop_start()
    else:
        set_status_internet(0)
    
    app.show()
    sys.exit(App.exec())
