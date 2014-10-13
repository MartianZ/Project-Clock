#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import hid
from PyQt4 import QtGui, QtCore, uic
import time

form_class = uic.loadUiType("nixietube.ui")[0]

class NixieTubeMainWindow(QtGui.QMainWindow, form_class):
    def __init__(self):
        super(NixieTubeMainWindow, self).__init__()
        self.setupUi(self)

        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.timer_one)
        self.timer.start(300)

        self.btn_ctime.clicked.connect(self.btn_ctime_clicked)
        self.btn_en_dis_nixietube.clicked.connect(self.btn_en_dis_nixietube_clicked)
        self.btn_en_dis_aoff.clicked.connect(self.btn_en_dis_aoff_clicked)
        self.btn_rgb.clicked.connect(self.btn_rgb_clicked)

        self.connection_status = 0
        self.setFixedSize(self.size())
        self.setWindowFlags(QtCore.Qt.Window | QtCore.Qt.WindowTitleHint | QtCore.Qt.CustomizeWindowHint | QtCore.Qt.WindowCloseButtonHint | QtCore.Qt.WindowMinimizeButtonHint)
        self.show()


    def timer_one(self):
        sender = self.sender()
        d = hid.enumerate(0x04b3, 0x1234)

        if len(d) > 0:
            self.lbl_device_status.setText("Device: <font color=green><b>Connceted[Normal]</b></font>")
            self.lbl_device_status.adjustSize()
            self.connection_status = 1
            return
        #todo:dfu
        #if len(d) > 0:
        #    self.lbl_device_status.setText("Device: <font color=green><b>Connceted[Normal]</b></font>")
        #    self.lbl_device_status.adjustSize()
        #    return
        self.lbl_device_status.setText("Device: <b>Unconnected</b>")
        self.lbl_device_status.adjustSize()
        self.connection_status = 0
            
    def btn_ctime_clicked(self):
        self.btn_ctime.setText("Sending instructions...")
        if self.connection_status == 1:
            try:
                h = hid.device()
                h.open(0x04b3, 0x1234)
                s = time.strftime("cs%Scm%Mch%Hcy%yco%mcd%d",time.localtime(time.time()))
                print s
                x = []
                for c in s:
                    x.append(ord(c))
                h.write(x)
                while 1:
                    data = h.read(64, 1000)
                    if data:
                        sys.stdout.write(''.join(chr(i) for i in data))
                        sys.stdout.flush()
                    else:
                        break
                h.close()
                
                QtGui.QMessageBox.information(self, 'Message', "Successfully calibrate NixieTube clock time!", QtGui.QMessageBox.Yes, QtGui.QMessageBox.Yes)
            except ex:
                print ex
            finally:
                self.btn_ctime.setText("Calibrate NixieTube Clock Time")

    def btn_en_dis_nixietube_clicked(self):
        self.btn_en_dis_nixietube.setText("Sending instructions...")
        if self.connection_status == 1:
            try:
                h = hid.device()
                h.open(0x04b3, 0x1234)
                h.write([ord('e')])
                while 1:
                    data = h.read(64, 100)
                    if data:
                        sys.stdout.write(''.join(chr(i) for i in data))
                        sys.stdout.flush()
                    else:
                        break
                h.close()
                
            except ex:
                print ex
            finally:
                self.btn_en_dis_nixietube.setText("Enable / Disable NixieTube")

    def btn_en_dis_aoff_clicked(self):
        self.btn_en_dis_aoff.setText("Sending instructions...")
        if self.connection_status == 1:
            try:
                h = hid.device()
                h.open(0x04b3, 0x1234)
                h.write([ord('z')])
                while 1:
                    data = h.read(64, 100)
                    if data:
                        sys.stdout.write(''.join(chr(i) for i in data))
                        sys.stdout.flush()
                    else:
                        break
                h.close()
                
            except ex:
                print ex
            finally:
                self.btn_en_dis_aoff.setText("Enable / Disable Automatically turn off NixieTube between 0:00 to 7:00")

    def btn_rgb_clicked(self):
        self.btn_rgb.setText("Sending instructions...")
        s = "cr%02dcg%02dcb%02d" % (self.text_r.value(), self.text_g.value(), self.text_b.value())
        print s

        if self.connection_status == 1:
            try:
                h = hid.device()
                h.open(0x04b3, 0x1234)
                x = []
                for c in s:
                    x.append(ord(c))
                h.write(x)
                while 1:
                    data = h.read(64, 1000)
                    if data:
                        sys.stdout.write(''.join(chr(i) for i in data))
                        sys.stdout.flush()
                    else:
                        break
                h.close()

                QtGui.QMessageBox.information(self, 'Message', "Successfully apply new LED R.G.B brightness value!", QtGui.QMessageBox.Yes, QtGui.QMessageBox.Yes)

            except ex:
                print ex
            finally:
                self.btn_rgb.setText("Set LED R.G.B Brightness")


def main():
    
    app = QtGui.QApplication(sys.argv)
    ex = NixieTubeMainWindow()
    sys.exit(app.exec_())




if __name__ == '__main__':
    main()
