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
        self.connection_status = 0

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
                    data = h.read(64, 3000)
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


def main():
    
    app = QtGui.QApplication(sys.argv)
    ex = NixieTubeMainWindow()
    sys.exit(app.exec_())




if __name__ == '__main__':
    main()
