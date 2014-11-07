# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'nixietube.ui'
#
# Created: Mon Nov  3 10:00:19 2014
#      by: PyQt4 UI code generator 4.11.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.setWindowModality(QtCore.Qt.NonModal)
        MainWindow.resize(571, 260)
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        MainWindow.setFont(font)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.tabWidget = QtGui.QTabWidget(self.centralwidget)
        self.tabWidget.setEnabled(True)
        self.tabWidget.setGeometry(QtCore.QRect(10, 60, 551, 191))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        self.tabWidget.setFont(font)
        self.tabWidget.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.tabWidget.setObjectName(_fromUtf8("tabWidget"))
        self.tab1 = QtGui.QWidget()
        self.tab1.setObjectName(_fromUtf8("tab1"))
        self.btn_ctime = QtGui.QPushButton(self.tab1)
        self.btn_ctime.setGeometry(QtCore.QRect(20, 10, 501, 32))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        self.btn_ctime.setFont(font)
        self.btn_ctime.setObjectName(_fromUtf8("btn_ctime"))
        self.btn_en_dis_nixietube = QtGui.QPushButton(self.tab1)
        self.btn_en_dis_nixietube.setGeometry(QtCore.QRect(20, 46, 501, 32))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        self.btn_en_dis_nixietube.setFont(font)
        self.btn_en_dis_nixietube.setObjectName(_fromUtf8("btn_en_dis_nixietube"))
        self.btn_en_dis_aoff = QtGui.QPushButton(self.tab1)
        self.btn_en_dis_aoff.setGeometry(QtCore.QRect(20, 82, 501, 32))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        self.btn_en_dis_aoff.setFont(font)
        self.btn_en_dis_aoff.setObjectName(_fromUtf8("btn_en_dis_aoff"))
        self.btn_rgb = QtGui.QPushButton(self.tab1)
        self.btn_rgb.setGeometry(QtCore.QRect(210, 116, 311, 32))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        self.btn_rgb.setFont(font)
        self.btn_rgb.setObjectName(_fromUtf8("btn_rgb"))
        self.text_r = QtGui.QSpinBox(self.tab1)
        self.text_r.setGeometry(QtCore.QRect(25, 117, 57, 30))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        font.setKerning(True)
        self.text_r.setFont(font)
        self.text_r.setFocusPolicy(QtCore.Qt.ClickFocus)
        self.text_r.setMaximum(99)
        self.text_r.setProperty("value", 70)
        self.text_r.setObjectName(_fromUtf8("text_r"))
        self.text_g = QtGui.QSpinBox(self.tab1)
        self.text_g.setGeometry(QtCore.QRect(85, 117, 57, 30))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        font.setKerning(True)
        self.text_g.setFont(font)
        self.text_g.setFocusPolicy(QtCore.Qt.ClickFocus)
        self.text_g.setObjectName(_fromUtf8("text_g"))
        self.text_b = QtGui.QSpinBox(self.tab1)
        self.text_b.setGeometry(QtCore.QRect(145, 117, 57, 30))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        font.setKerning(True)
        self.text_b.setFont(font)
        self.text_b.setFocusPolicy(QtCore.Qt.ClickFocus)
        self.text_b.setProperty("value", 50)
        self.text_b.setObjectName(_fromUtf8("text_b"))
        self.tabWidget.addTab(self.tab1, _fromUtf8(""))
        self.tab2 = QtGui.QWidget()
        self.tab2.setObjectName(_fromUtf8("tab2"))
        self.label = QtGui.QLabel(self.tab2)
        self.label.setGeometry(QtCore.QRect(13, 30, 140, 21))
        self.label.setObjectName(_fromUtf8("label"))
        self.progressBar = QtGui.QProgressBar(self.tab2)
        self.progressBar.setGeometry(QtCore.QRect(13, 86, 341, 20))
        self.progressBar.setProperty("value", 0)
        self.progressBar.setObjectName(_fromUtf8("progressBar"))
        self.edt_dfu_filename = QtGui.QLineEdit(self.tab2)
        self.edt_dfu_filename.setGeometry(QtCore.QRect(153, 30, 261, 21))
        self.edt_dfu_filename.setObjectName(_fromUtf8("edt_dfu_filename"))
        self.btn_dfu_open = QtGui.QPushButton(self.tab2)
        self.btn_dfu_open.setGeometry(QtCore.QRect(420, 26, 114, 32))
        self.btn_dfu_open.setObjectName(_fromUtf8("btn_dfu_open"))
        self.btn_dfu_upload = QtGui.QPushButton(self.tab2)
        self.btn_dfu_upload.setGeometry(QtCore.QRect(363, 80, 171, 32))
        self.btn_dfu_upload.setObjectName(_fromUtf8("btn_dfu_upload"))
        self.tabWidget.addTab(self.tab2, _fromUtf8(""))
        self.lbl_device_status = QtGui.QLabel(self.centralwidget)
        self.lbl_device_status.setGeometry(QtCore.QRect(20, 10, 521, 16))
        self.lbl_device_status.setAutoFillBackground(False)
        self.lbl_device_status.setObjectName(_fromUtf8("lbl_device_status"))
        self.lbl_device_version = QtGui.QLabel(self.centralwidget)
        self.lbl_device_version.setGeometry(QtCore.QRect(20, 30, 521, 16))
        self.lbl_device_version.setAutoFillBackground(False)
        self.lbl_device_version.setObjectName(_fromUtf8("lbl_device_version"))
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "NixieTube", None))
        self.btn_ctime.setText(_translate("MainWindow", "Calibrate NixieTube Clock Time", None))
        self.btn_en_dis_nixietube.setText(_translate("MainWindow", "Enable / Disable NixieTube", None))
        self.btn_en_dis_aoff.setText(_translate("MainWindow", "Enable / Disable automatically turn off NixieTube between 0:00 to 7:00", None))
        self.btn_rgb.setText(_translate("MainWindow", "Set LED R.G.B Brightness", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab1), _translate("MainWindow", "Configuration", None))
        self.label.setText(_translate("MainWindow", "Select new firmware:", None))
        self.edt_dfu_filename.setText(_translate("MainWindow", "/Users/MartianZ/Documents/NixieTube-Clock/IN-14/STM32_C8_NixieTube/main.hex", None))
        self.btn_dfu_open.setText(_translate("MainWindow", "Open", None))
        self.btn_dfu_upload.setText(_translate("MainWindow", "Upload New Firmware", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab2), _translate("MainWindow", "Device Firmware Upgrade", None))
        self.lbl_device_status.setText(_translate("MainWindow", "Device: <b>Unconnected</b>", None))
        self.lbl_device_version.setText(_translate("MainWindow", "<html><head/><body><p>Firmware: <span style=\" font-weight:600;\">Unknown</span></p></body></html>", None))

