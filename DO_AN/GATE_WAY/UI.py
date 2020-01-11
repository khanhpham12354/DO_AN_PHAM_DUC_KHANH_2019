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


class Gateway():
    def __init__(self, port_name, baudrate, timeout):
        self.ser = serial.Serial()
        self.ser.port = str(port_name)
        self.ser.baudrate = baudrate
        self.ser.timeout = timeout

    def open(self):
        if (self.ser.is_open == False):
            self.ser.open()

    def write_data(self, data):
        self.ser.write(data.encode('utf-8'))

    def read_data(self):
        return self.ser.readlines()


def Lora_init():
    global GW
    GW = Gateway("COM16", 9600, 0.2)
    GW.open()
    GW.write_data("AT\r\n")
    print(GW.read_data())
    GW.write_data("AT+MODE=TEST\r\n")
    print(GW.read_data())
    GW.write_data("AT+TEST=RFCFG,433\r\n")
    print(GW.read_data())
    GW.write_data("AT+TEST=RXLRPKT\r\n")
    print(GW.read_data())


def load_data():
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


App = QtWidgets.QApplication([])

app = uic.loadUi("guis\\main.ui")

if __name__ == "__main__":
    Lora_init()
    Time = QTimer()
    Time.timeout.connect(load_data)
    Time.start(4000)
    app.show()
    sys.exit(App.exec())
