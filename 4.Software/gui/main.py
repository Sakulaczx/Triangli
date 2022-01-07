import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QMessageBox
from PyQt5.QtGui import QTextCursor
from main_ui import *
from wifi_udp import *
import threading    #引入并行
import numpy as np
import pyqtgraph as pg
import re
from sharedcomponets import GUIToolKit
import copy

class MyWindow(QMainWindow, Ui_MainWindow):
    signalColors = [GUIToolKit.RED_COLOR, GUIToolKit.BLUE_COLOR, GUIToolKit.PURPLE_COLOR, GUIToolKit.YELLOW_COLOR,
                    GUIToolKit.MAROON_COLOR, GUIToolKit.ORANGE_COLOR, GUIToolKit.GREEN_COLOR]
    signalIcons = ['reddot', 'bluedot', 'purpledot', 'yellowdot', 'maroondot', 'orangedot', 'greendot']
    textColors = ['FF5C5C','398AD9','5BEC8D','FD42AC','FF33FF','4B8200','DE87B8']
    def __init__(self, parent=None):
        super(MyWindow, self).__init__(parent)
        self.setupUi(self)
        # 变量初始化
        self.wifi_open_flag = 0
        # self.variable_init()
        # 设置实例
        # self.CreateItems()
        # 设置信号与槽
        self.CreateSignalSlot()
    # 设置信号与槽
    def CreateSignalSlot(self):
        self.horizontalSlider_1.valueChanged.connect(self.horizontalSlider_1_valueChanged)
        self.horizontalSlider_2.valueChanged.connect(self.horizontalSlider_2_valueChanged)
        self.horizontalSlider_3.valueChanged.connect(self.horizontalSlider_3_valueChanged)
        self.wifi_config_pushButton.clicked.connect(self.wifi_config_pushButton_clicked)
        self.wifi_command_pushButton_1.clicked.connect(self.wifi_command_pushButton_1_clicked)
        self.wifi_command_pushButton_2.clicked.connect(self.wifi_command_pushButton_2_clicked)
        self.wifi_command_pushButton_3.clicked.connect(self.wifi_command_pushButton_3_clicked)
        self.TA_pushButton.clicked.connect(self.TA_pushButton_clicked)
        self.SA_pushButton.clicked.connect(self.SA_pushButton_clicked)
        self.SV_pushButton.clicked.connect(self.SV_pushButton_clicked)
        self.VP1_pushButton.clicked.connect(self.VP1_pushButton_clicked)
        self.VI1_pushButton.clicked.connect(self.VI1_pushButton_clicked)
        self.VP2_pushButton.clicked.connect(self.VP2_pushButton_clicked)
        self.VI2_pushButton.clicked.connect(self.VI2_pushButton_clicked)

        self.raw_pushButton.clicked.connect(self.raw_pushButton_clicked)
        self.wave_pushButton.clicked.connect(self.wave_pushButton_clicked)
    def raw_pushButton_clicked(self):
        self.change_state = 1
        self.stackedWidget.setCurrentIndex(1)
        # for i in range(self.gridLayout.count()):
        #     self.gridLayout.removeWidget(self.gridLayout.itemAt(i).widget())
        # self.gridLayout.addWidget(self.raw_line)

    def wave_pushButton_clicked(self):
        self.change_state = 0
        self.stackedWidget.setCurrentIndex(0)
        # for i in range(self.gridLayout.count()):
        #     self.gridLayout.removeWidget(self.gridLayout.itemAt(i).widget())
        #
        #
        # self.gridLayout.addWidget(self.plotWidget)

    # 设置实例
    def CreateItems(self):
        # 定时器-绘图刷新
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(20)
        # wifi udp
        self.udp = udp()
        # self.wifi_IP_lineEdit.setText(self.udp.user_ip)
    def variable_init(self):
        # wifi变量
        self.wifi_recv_flag = 0
        self.close_flag = 1
        self.change_state = 0
        self.re_item = []
        self.raw_roll = 0
    def plot_init(self):
        # 绘图对象
        pg.setConfigOptions(antialias=True)
        self.plotWidget = pg.PlotWidget()
        self.plotWidget.showGrid(x=True, y=True, alpha=0.5)
        self.plotWidget.addLegend()
        self.controlPlotWidget = ControlPlotPanel(controllerPlotWidget=self)
        # 图表可视化数组
        self.numberOfSamples = 300
        self.signalDataArrays = []
        self.signalPlots = []
        self.signalPlotFlags = []
        self.timeArray = np.arange(-self.numberOfSamples, 0, 1)
        for (sig, sigColor, checkBox,tooltip) in zip(self.re_item, self.signalColors, self.controlPlotWidget.signalCheckBox,self.re_item):
            # define signal plot data array
            self.signalDataArrays.append(np.zeros(self.numberOfSamples))
            # configure signal plot parameters
            signalPen = pg.mkPen(color=sigColor, width=1.5)
            self.signalPlots.append(pg.PlotDataItem(self.timeArray,
                                            self.signalDataArrays[-1],
                                            pen=signalPen, name=tooltip))
            self.plotWidget.addItem(self.signalPlots[-1])
            # is plotted flag
            self.signalPlotFlags.append(True)
            # add callback
            checkBox.stateChanged.connect(self.signalPlotFlagUpdate)
        self.stackedWidget = QtWidgets.QStackedWidget()
        self.gridLayout.addWidget(self.stackedWidget)
        self.raw_line = QtWidgets.QTextEdit()
        self.raw_line.setStyleSheet('background: rgb(0, 0, 0)')
        self.stackedWidget.addWidget(self.plotWidget)
        self.stackedWidget.addWidget(self.raw_line)
        self.tool_layout.addWidget(self.controlPlotWidget)



    # checkbox
    def signalPlotFlagUpdate(self):
        for i, (checkBox, plotFlag) in enumerate(zip(self.controlPlotWidget.signalCheckBox, self.signalPlotFlags)):
            if checkBox.isChecked() and (not plotFlag):
                self.signalPlotFlags[i] = True
                self.plotWidget.addItem( self.signalPlots[i] )
            elif (not checkBox.isChecked()) and plotFlag:
                self.signalPlotFlags[i]  = False
                self.plotWidget.removeItem( self.signalPlots[i] )
    # 滑条绑定
    def horizontalSlider_1_valueChanged(self):
        value = self.horizontalSlider_1.value()
        value = float(self.left_lineEdit_1.text())+(value+100)*(float(self.right_lineEdit_1.text())-float(self.left_lineEdit_1.text()))/200
        self.num_lineEdit_1.setText(str(value))
        value = self.command_lineEdit_1.text()+str(value)
        self.udp.send_message(str(value))
    def horizontalSlider_2_valueChanged(self):
        value = self.horizontalSlider_2.value()
        value = float(self.left_lineEdit_2.text())+(value+100)*(float(self.right_lineEdit_2.text())-float(self.left_lineEdit_2.text()))/200
        self.num_lineEdit_2.setText(str(value))
        value = self.command_lineEdit_2.text()+str(value)
        self.udp.send_message(str(value))
    def horizontalSlider_3_valueChanged(self):
        value = self.horizontalSlider_3.value()
        value = float(self.left_lineEdit_3.text())+(value+100)*(float(self.right_lineEdit_3.text())-float(self.left_lineEdit_3.text()))/200
        self.num_lineEdit_3.setText(str(value))
        value = self.command_lineEdit_3.text()+str(value)
        self.udp.send_message(str(value))
    # command命令发送事件
    def wifi_command_pushButton_1_clicked(self):
        self.udp.send_message(self.wifi_command_lineEdit_1.text())
    def wifi_command_pushButton_2_clicked(self):
        self.udp.send_message(self.wifi_command_lineEdit_2.text())
    def wifi_command_pushButton_3_clicked(self):
        self.udp.send_message(self.wifi_command_lineEdit_3.text())
    def TA_pushButton_clicked(self):
        self.udp.send_message("TA"+self.TA_doubleSpinBox.text())
    def SA_pushButton_clicked(self):
        self.udp.send_message("SA"+self.SA_doubleSpinBox.text())
    def SV_pushButton_clicked(self):
        self.udp.send_message("SV"+self.SV_doubleSpinBox.text())
    def VP1_pushButton_clicked(self):
        self.udp.send_message("VP1"+self.VP1_doubleSpinBox.text())
    def VI1_pushButton_clicked(self):
        self.udp.send_message("VI1"+self.VI1_doubleSpinBox.text())
    def VP2_pushButton_clicked(self):
        self.udp.send_message("VP2"+self.VP2_doubleSpinBox.text())
    def VI2_pushButton_clicked(self):
        self.udp.send_message("VI2"+self.VI2_doubleSpinBox.text())

    # WIFI设置点击后判断有无接收到esp32发来的数据
    def wifi_config_pushButton_clicked(self):
        if self.wifi_open_flag == 0:
            try:
                self.variable_init()
                self.CreateItems()
                print(self.wifi_IP_lineEdit.text(),type(self.wifi_IP_lineEdit.text()))
                self.udp.udpClientSocket.bind((self.wifi_IP_lineEdit.text(), 2333))
                self.udp.udpClientSocket.settimeout(1)
                # 第一次接受数据，用于判断项目数，
                self.udp.send_message("START")
                recv_data = self.udp.udpClientSocket.recv(1024)
                recv_data = recv_data.decode('utf-8')
                recv_data = recv_data[:-1]
                recv_data = recv_data.split(',')
                """处理接受的信息"""
                for i, data in enumerate(recv_data):
                    self.re_item.append(''.join(re.split(r'[^A-Za-z]', data)))
                print(self.re_item)
                # 图表初始化
                self.plot_init()
                t1 = threading.Thread(target=self.udp_recv)
                t1.start()
                self.wifi_open_flag = 1
                self.wifi_config_pushButton.setText("断开连接")
                self.wifi_config_pushButton.setStyleSheet("QPushButton{color:rgb(255,0,0,255);}")
            except Exception as e:
                print(e)
                QMessageBox.critical(self, "错误", str(e))
        else:
            self.udp.send_message("START")
            del self.udp
            self.wifi_open_flag = 0
            self.tool_layout.itemAt(0).widget().deleteLater()
            self.gridLayout.itemAt(0).widget().deleteLater()
            self.wifi_config_pushButton.setText("设置")
            self.variable_init()
    def udp_recv(self):
        while self.close_flag:
            recv_data = self.udp.udpClientSocket.recv(1024)
            recv_data = recv_data.decode('utf-8')
            recv_data = recv_data[:-1]
            recv_data = recv_data.split(',')
            """处理接受的信息"""
            # print(recv_data)
            self.re_text = ''
            for i, data in enumerate(recv_data):
                if self.signalPlotFlags[i]:
                    # self.re_item.append(''.join(re.split(r'[^A-Za-z]', data)))
                    data = data.replace(self.re_item[i],'')
                    if self.change_state:
                        self.re_text += '<font color=\"#{1}\">{0}\t</font>'.format(data,self.textColors[i])
                    else:
                        self.signalDataArrays[i] = np.roll(self.signalDataArrays[i], -1)
                        self.signalDataArrays[i][-1] = data
    def update_plot(self):
        if self.wifi_recv_flag:
            if self.change_state:
                self.raw_line.append(self.re_text)
                if self.raw_roll:
                    self.raw_line.moveCursor(QTextCursor.End)
            else:
                for i, plotFlag in enumerate(self.signalPlotFlags):
                    if plotFlag:
                        self.signalPlots[i].setData(self.timeArray, self.signalDataArrays[i])
                        self.signalPlots[i].updateItems()
                        self.signalPlots[i].sigPlotChanged.emit(self.signalPlots[i])

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        print("关闭")
        self.udp.send_message('START')
        self.close_flag = 0
        self.udp.udpClientSocket.close()


class ControlPlotPanel(QtWidgets.QWidget):

    def __init__(self, parent=None, controllerPlotWidget=None):
        '''Constructor for ToolsWidget'''
        super().__init__(parent)
        # 变量
        self.MotorEnable_flag = 0

        # 继承主窗口对象
        self.controlledPlot = controllerPlotWidget

        self.horizontalLayout1 = QtWidgets.QHBoxLayout()
        self.horizontalLayout1.setObjectName('horizontalLayout')
        self.setLayout(self.horizontalLayout1)

        self.startStopButton = QtWidgets.QPushButton(self)
        self.startStopButton.setText('Start')
        self.startStopButton.setObjectName('Start')
        self.startStopButton.clicked.connect(self.wifi_recv_open_pushButton_clicked)
        self.startStopButton.setIcon(GUIToolKit.getIconByName('start'))
        self.horizontalLayout1.addWidget(self.startStopButton)

        self.MotorEnableButton = QtWidgets.QPushButton(self)
        self.MotorEnableButton.setText('Disable')
        self.MotorEnableButton.setObjectName('Disable')
        self.MotorEnableButton.clicked.connect(self.MotorEnableButton_clicked)
        self.MotorEnableButton.setIcon(GUIToolKit.getIconByName('stop'))
        self.horizontalLayout1.addWidget(self.MotorEnableButton)

        self.zoomAllButton = QtWidgets.QPushButton(self)
        self.zoomAllButton.setObjectName('zoomAllButton')
        self.zoomAllButton.setText('View all')
        self.zoomAllButton.setIcon(GUIToolKit.getIconByName('zoomall'))
        self.zoomAllButton.clicked.connect(self.zoomAllPlot)
        self.horizontalLayout1.addWidget(self.zoomAllButton)

        self.signalCheckBox = []
        for i in range(len(self.controlledPlot.re_item)):
            checkBox = QtWidgets.QCheckBox(self)
            checkBox.setObjectName('signalCheckBox' + str(i))
            checkBox.setToolTip(self.controlledPlot.re_item[i])
            checkBox.setText(self.controlledPlot.re_item[i])
            checkBox.setIcon(GUIToolKit.getIconByName(self.controlledPlot.signalIcons[i]))
            checkBox.setChecked(True)
            self.signalCheckBox.append(checkBox)
            self.horizontalLayout1.addWidget(checkBox)
    def zoomAllPlot(self):
        if self.controlledPlot.change_state:
            self.controlledPlot.raw_roll = ~self.controlledPlot.raw_roll
        else:
            self.controlledPlot.plotWidget.enableAutoRange()
    def wifi_recv_open_pushButton_clicked(self):
        if self.controlledPlot.wifi_recv_flag == 0:
            # 打开wifi接收
            self.controlledPlot.wifi_recv_flag = 1
            self.startStopButton.setText('Stop')
        else:
            self.controlledPlot.wifi_recv_flag = 0
            self.startStopButton.setText('Start')
    def MotorEnableButton_clicked(self):
        if self.MotorEnable_flag == 0:
            # Motor关闭
            self.MotorEnable_flag = 1
            self.MotorEnableButton.setText('Enable')
            self.controlledPlot.udp.send_message("MOTOR")
        else:
            self.MotorEnable_flag = 0
            self.MotorEnableButton.setText('Disable')
            self.controlledPlot.udp.send_message("MOTOR")
if __name__ == '__main__':
    app = QApplication(sys.argv)
    myWin = MyWindow()
    myWin.show()
    sys.exit(app.exec_())