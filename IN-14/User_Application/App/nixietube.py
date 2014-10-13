#!/usr/bin/python
# -*- coding: utf-8 -*-
import usb
import sys
from PyQt4 import QtGui, QtCore


class NixieTubeMainWindow(QtGui.QMainWindow):
    def __init__(self):
        super(NixieTubeMainWindow, self).__init__()
        
        self.init_ui()
        
    def init_ui(self):
        
        
        #label
        self.lbl_device_status = QtGui.QLabel(self)
        self.lbl_device_status.setText("Device: <b>Unconnected</b>") #Normal, DFU, Unconnected
        self.lbl_device_status.move(15, 12)
        self.lbl_device_status.adjustSize()

        
        
        #btn
        #refresh_btn = QtGui.QPushButton('Refresh', self)
        #refresh_btn.setToolTip('Refresh device connection state.')
        #refresh_btn.resize(refresh_btn.sizeHint())
        #refresh_btn.move(200, 6)
        #refresh_btn.clicked.connect(self.refresh_btn_clicked)
        
        #tab
        tab_widget = QtGui.QTabWidget(self)     # add tab
        tab1 = QtGui.QWidget()
        tab2 = QtGui.QWidget()
        tab_widget.addTab(tab1, "Configuration") 
        tab_widget.addTab(tab2, "Device Firmware Upgrade")
        tab_widget.setGeometry(10, 100, 480, 190)
        
        
        #timer
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.timer_one)
        self.timer.start(300)
        
        self.setGeometry(0, 0, 500, 300)
        self.setWindowTitle('NixieTube Assistant')
        self.setWindowIcon(QtGui.QIcon('icon.png'))        
        frameGm = self.frameGeometry()
        screen = QtGui.QApplication.desktop().screenNumber(QtGui.QApplication.desktop().cursor().pos())
        centerPoint = QtGui.QApplication.desktop().screenGeometry(screen).center()
        frameGm.moveCenter(centerPoint)
        self.move(frameGm.topLeft())
        self.show()
    def timer_one(self):
        sender = self.sender()
        
        busses = usb.busses()
        for bus in busses:
            devices = bus.devices
            for dev in devices:
                if (dev.idVendor == 1) and (dev.idProduct == 57105): #0xdf11
                    self.lbl_device_status.setText("Device: <font color=red><b>Connceted[DFU]</b></font>")
                    self.lbl_device_status.adjustSize()
                    return
                if (dev.idVendor == 1203) and (dev.idProduct == 17748):
                    self.lbl_device_status.setText("Device: <font color=green><b>Connceted[Normal]</b></font>")
                    self.lbl_device_status.adjustSize()
                    dev.open()
                    dev.write("p")
                    return
            self.lbl_device_status.setText("Device: <b>Unconnected</b>")
            self.lbl_device_status.adjustSize()
            
            
        
def main():
    
    app = QtGui.QApplication(sys.argv)
    ex = NixieTubeMainWindow()
    sys.exit(app.exec_())




if __name__ == '__main__':
    main()
