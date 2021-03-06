import matplotlib
matplotlib.use('Qt4Agg')
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
import matplotlib.pyplot as plt
from PyQt4 import QtCore, QtGui, uic
import sys
from ..TestCom import testCom
from ..ComDriver import ComDriver
from .canvas import Canvas
import sys
import serial
import numpy as np
from ..process_data import process_data
from ..logger import logger
from ..process_fusion import process_fusion

qtCreatorFile = "solindar_lib/GUI/InfraredNetwork.ui" # my Qt Designer file

Ui_MainWindow, QtBaseClass = uic.loadUiType(qtCreatorFile)

class SolindarGUI(QtGui.QMainWindow, Ui_MainWindow):
    def __init__(self,con):
        QtGui.QMainWindow.__init__(self)
        Ui_MainWindow.__init__(self)
        self.setupUi(self)
        # self.navigation = NavigationToolbar(self.canvas, self)
        # self.hlayout.addWidget(self.navigation)
        # self.sonBox.stateChanged.connect(self.ChannelSon)
        # self.sonBox.setStatusTip("Activar la Grafica del Sonar")
        # self.lidBox.stateChanged.connect(self.ChannelLid)
        # self.lidBox.setStatusTip("Activar la Grafica del Lidar")
        # self.fusBox.stateChanged.connect(self.ChannelFus)
        # self.fusBox.setStatusTip("Activar la Grafica del Fusion")
        self.con=con
        # self.Ch_state=[False, False, False]
        # self.Ch_index=[]
        # self.Ch_colors=['bo','go','yo','ro','co','mo','wo']
        # self.pos_conv = 2 * np.pi / 84
        # self.ax = self.canvas.figure.add_subplot(111, projection = 'polar')
        # self.axesCom.activated[str].connect(self.changeDistanceScale)
        # self.axesCom.setStatusTip("Cambiar la escala de distancia")
        # self.axeslim = 80
        # #self.ax.set_xticklabels([]) #deletes labels from X-axes
        # self.lines = self.ax.plot()
        # self.set_grid=False
        # #Fifos
        # self.fusion_fifo = np.zeros((self.con.len_fifo))
        # self.sonarproc = np.zeros((self.con.len_fifo))
        # self.lidarproc = np.zeros((self.con.len_fifo))
        # #Logger
        # self.log = logger()
        # #Plot
        # plt.ion()
        self.zona1 = 1
        self.zona2 = 1
        self.zona3 = 1
        self.zona4 = 1
        self.zona5 = 1
        self.mode = True # 0 for slave, 1 for master
        self.messageSend = ''
        self.dict_data = {
            'msg': 0xA8,
            'is_master':True,
            'zones': [4,1,6,7,2]
            }
        self.text = '<h1>Recepcion</h1>'
        self.textEditRecieve.setText(self.text)
        self.messageRecieve = 'a'
        # self.textEditSend.setFontWeight(100)

        self.comboBoxMaster.activated[str].connect(self.OpMod)
        self.comboBoxZona1.activated[str].connect(self.SetZona1)
        self.comboBoxZona2.activated[str].connect(self.SetZona2)
        self.comboBoxZona3.activated[str].connect(self.SetZona3)
        self.comboBoxZona4.activated[str].connect(self.SetZona4)
        self.comboBoxZona5.activated[str].connect(self.SetZona5)
        self.pushButton.clicked.connect(self.SendMessage)


        # Timer
        timerRecieve=QtCore.QTimer(self)
        timerRecieve.timeout.connect(self.RecieveMessage)
        timerRecieve.start(100)



    def OpMod(self,text):
        if text == "Master":
            print("Mode is set in Master")
            self.mode = True
        elif text == "Slave":
            print("Mode is set in Slave")
            self.mode = False

    def SetZona1(self,text):
        if text == "1":
            print("Zona1 was set in 1")
            self.zona1 = 1
        elif text == "2":
            print("Zona1 was set in 2")
            self.zona1 = 2
        elif text == "3":
            print("Zona1 was set in 3")
            self.zona1 = 3
        elif text == "4":
            print("Zona1 was set in 4")
            self.zona1 = 4 
        elif text == "5":
            print("Zona1 was set in 5")
            self.zona1 = 5
        elif text == "6":
            print("Zona1 was set in 6")
            self.zona1 = 6

    def SetZona2(self,text):
        if text == "1":
            print("Zona2 was set in 1")
            self.zona2 = 1
        elif text == "2":
            print("Zona2 was set in 2")
            self.zona2 = 2
        elif text == "3":
            print("Zona2 was set in 3")
            self.zona2 = 3
        elif text == "4":
            print("Zona2 was set in 4")
            self.zona2 = 4 
        elif text == "5":
            print("Zona2 was set in 5")
            self.zona2 = 5
        elif text == "6":
            print("Zona2 was set in 6")
            self.zona2 = 6

    def SetZona3(self,text):
        if text == "1":
            print("Zona3 was set in 1")
            self.zona3 = 1
        elif text == "2":
            print("Zona3 was set in 2")
            self.zona3 = 2
        elif text == "3":
            print("Zona3 was set in 3")
            self.zona3 = 3
        elif text == "4":
            print("Zona3 was set in 4")
            self.zona3 = 4 
        elif text == "5":
            print("Zona3 was set in 5")
            self.zona3 = 5
        elif text == "6":
            print("Zona3 was set in 6")
            self.zona3 = 6
        
    def SetZona4(self,text):
        if text == "1":
            print("Zona4 was set in 1")
            self.zona1 = 1
        elif text == "2":
            print("Zona4 was set in 2")
            self.zona4 = 2
        elif text == "3":
            print("Zona4 was set in 3")
            self.zona4 = 3
        elif text == "4":
            print("Zona4 was set in 4")
            self.zona4 = 4 
        elif text == "5":
            print("Zona4 was set in 5")
            self.zona4 = 5
        elif text == "6":
            print("Zona4 was set in 6")
            self.zona4 = 6

    def SetZona5(self,text):
        if text == "1":
            print("Zona5 was set in 1")
            self.zona5 = 1
        elif text == "2":
            print("Zona5 was set in 2")
            self.zona5 = 2
        elif text == "3":
            print("Zona5 was set in 3")
            self.zona5 = 3
        elif text == "4":
            print("Zona5 was set in 4")
            self.zona5 = 4 
        elif text == "5":
            print("Zona5 was set in 5")
            self.zona5 = 5
        elif text == "6":
            print("Zona5 was set in 6")
            self.zona5 = 6

    def SendMessage(self):
        self.messageSend = self.textEditSend.toPlainText()
        # print(type(self.messageSend[2]))
        self.dict_data = {
            'msg' : ord(self.messageSend),
            'is_master' : self.mode,
            'zones' : [self.zona1,self.zona2,self.zona3,self.zona4,self.zona5]
        }
        self.con.send_data(self.dict_data)

    def RecieveMessage(self):
        if (type(self.con.get_msg()) == type('a')):
            self.messageRecieve = '<br><h1 align=center>' + self.con.get_msg() + '</h1>'
            # print(self.messageRecieve)
            self.textEditRecieve.setText(self.messageRecieve)

    # def ChannelSon (self,state):
    #     if state == QtCore.Qt.Checked:
    #         self.Ch_state[0] = True
    #     else:
    #         self.Ch_state[0] = False

    # def ChannelLid (self,state):
    #     if state == QtCore.Qt.Checked:
    #         self.Ch_state[1] = True
    #     else:
    #         self.Ch_state[1] = False

    # def ChannelFus (self,state):
    #     if state == QtCore.Qt.Checked:
    #         self.Ch_state[2] = True
    #     else:
    #         self.Ch_state[2] = False

    # def filter (self):
    #     if int(self.con.filter_on[self.con.currentPosition]):
    #         self.filPro.setValue(100)
    #     else:
    #         self.filPro.setValue(0)

    # def PosicionLcd(self):
    #     self.posLcd.display(int(((-1)*self.con.currentPosition*self.pos_conv*(180/np.pi))))

    # def SonarLcd(self):
    #     self.sonLcd.display(int(self.sonarproc[self.con.currentPosition]))

    # def LidarLcd(self):
    #     self.lidLcd.display(int(self.lidarproc[self.con.currentPosition]))

    # def FusionLcd(self):
    #     self.fusLcd.display(int(self.fusion_fifo[self.con.currentPosition]))

    # def logFun(self):       
    #     for i in self.con.position:
    #         self.log.info(','+str(self.con.filter_on[i]) + ',' + \
    #         str((-1)*i * self.pos_conv*(180/np.pi)) + ','  +\
    #         str(self.lidarproc[i]) + ',' + \
    #         str(self.sonarproc[i]) + ',')

    # def make_graph(self):
    #     self.sonarproc,self.lidarproc = process_data(self.con.sonar_fifo,self.con.lidar_fifo)
    #     self.fusion_fifo = process_fusion(self.sonarproc,self.lidarproc)
    #     currentPosition = (-1)*self.con.position_fifo * self.pos_conv

    #     self.lines = self.ax.plot([(-1)*self.con.currentPosition*self.pos_conv,(-1)*self.con.currentPosition*self.pos_conv],[0,300],'b',alpha=0.35)
    #     if self.Ch_state[0]:
    #         self.lines = self.ax.plot(currentPosition[:],self.sonarproc[:],self.Ch_colors[0])
    #         self.lines = self.ax.plot(currentPosition[self.con.currentPosition],self.sonarproc[self.con.currentPosition],self.Ch_colors[4])
    #     if self.Ch_state[1]:
    #         self.lines = self.ax.plot(currentPosition[:],self.lidarproc[:],self.Ch_colors[3])
    #         self.lines = self.ax.plot(currentPosition[self.con.currentPosition],self.lidarproc[self.con.currentPosition],self.Ch_colors[5])
    #     if self.Ch_state[2]:
    #         self.lines = self.ax.plot(currentPosition[:], self.fusion_fifo[:],self.Ch_colors[2])
    #         self.lines = self.ax.plot(currentPosition[self.con.currentPosition], self.fusion_fifo[self.con.currentPosition],self.Ch_colors[1])
    #     self.ax.set_rlim(bottom=0,top=self.axeslim)

    # #Repeat all over again
    # def run(self):
    #     self.con.update_fifos() #Update fifo
    #     self.ax.clear()
    #     self.make_graph() #Graph
    #     self.filter() #Check filter flag
    #     self.PosicionLcd() #Print motor position
    #     self.logFun()  #Log
    #     self.SonarLcd()
    #     self.LidarLcd()
    #     self.FusionLcd()

    # def changeDistanceScale(self,text):
    #     if text == "MaxDistance = 80cm":
    #         print("MaxDistance = 80cm") #Colocar cambio de escala
    #         self.ax.set_rlim(bottom=0,top=80)
    #         self.axeslim = 80
    #     else:
    #         print("MaxDistance = 300cm") #Colocar cambio de escala
    #         self.ax.set_rlim(bottom=0,top=300)
    #         self.axeslim = 300
