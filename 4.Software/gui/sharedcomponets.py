import os

from PyQt5 import QtGui, QtWidgets, QtCore

class GUIToolKit(object):
    ''' This class is used to provide icons for the rest of the application
        hiding the location of the resources
    '''
    RED_COLOR = (255, 92, 92)
    GREEN_COLOR = (57, 217, 138)
    BLUE_COLOR = (91, 141, 236)
    ORANGE_COLOR = (253, 172, 66)
    YELLOW_COLOR = (255,255,51)
    PURPLE_COLOR = (75,0,130)
    MAROON_COLOR = (222,184,135)

    @staticmethod
    def getIconByName(icoName):

        file_index = {
            'add': 'add.png',
            'add_motor': 'add_motor.png',
            'tree': 'tree.png',
            'gen': 'gen.png',
            'home': 'home.png',
            'form': 'form.png',
            'edit': 'edit.png',
            'delete': 'delete.png',
            'statistics': 'statistics.png',
            'reddot': 'reddot.png',
            'orangedot': 'orangedot.png',
            'greendot': 'greendot.png',
            'bluedot': 'bluedot.png',
            'purpledot': 'purpledot.png',
            'yellowdot': 'yellowdot.png',
            'maroondot': 'maroondot.png',
            'send': 'send.png',
            'zoomall': 'zoomall.png',
            'connect': 'connect.png',
            'continue': 'continue.png',
            'alert': 'alert.png',
            'gear': 'gear.png',
            'generalsettings': 'generalsettings.png',
            'open': 'open.png',
            'loop': 'loop.png',
            'save': 'save.png',
            'stop': 'stop.png',
            'restart': 'continue.png',
            'res': 'res.png',
            'sensor': 'sensor.png',
            'start': 'start.png',
            'motor': 'motor.png',
            'pause': 'pause.png',
            'pull': 'pull.png',
            'push': 'push.png',
            'list': 'list.png',
            'disconnect': 'disconnect.png',
            'configure': 'configure.png',
            'pidconfig': 'pidconfig.png',
            'consoletool': 'consoletool.png',
            'fordward': 'fordward.png',
            'fastbackward': 'fastbackward.png',
            'backward': 'backward.png',
            'stopjogging': 'stopjogging.png',
            'fastfordward': 'fastfordward.png',
            'customcommands':'customcommands.png'
        }
        currentDir = os.path.dirname(__file__)
        icon_path = os.path.join(currentDir, './resources', file_index[icoName])
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(icon_path), QtGui.QIcon.Normal,
                      QtGui.QIcon.Off)
        return icon