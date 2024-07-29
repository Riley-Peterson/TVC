import serial
import pyqtgraph.opengl as gl
from pyqtgraph.Qt import QtCore, QtWidgets
import numpy as np

# Serial port configuration
port = 'COM6'  # Update with your COM port
baudrate = 115200
ser = serial.Serial(port, baudrate)

# PyQtGraph window setup
app = QtWidgets.QApplication([])
window = gl.GLViewWidget()
window.show()
window.setWindowTitle('3D Quaternion Visualization')

# 3D axis and grid
grid = gl.GLGridItem()
window.addItem(grid)
axis = gl.GLAxisItem()
window.addItem(axis)

# Vector component lines
x_line = gl.GLLinePlotItem()
y_line = gl.GLLinePlotItem()
z_line = gl.GLLinePlotItem()
window.addItem(x_line)
window.addItem(y_line)
window.addItem(z_line)

def update():
    if ser.in_waiting:
        data = ser.readline().decode('utf-8').strip()
        try:
            # Parse the quaternion data
            parts = data.split(',')
            w = float(parts[0].split(':')[1])
            x = float(parts[1].split(':')[1])
            y = float(parts[2].split(':')[1])
            z = float(parts[3].split(':')[1])

            # Normalize quaternion
            norm = np.sqrt(w*w + x*x + y*y + z*z)
            w, x, y, z = w/norm, x/norm, y/norm, z/norm

            # Convert quaternion to rotation matrix
            rot_matrix = np.array([
                [1 - 2*(y*y + z*z), 2*(x*y - z*w), 2*(x*z + y*w)],
                [2*(x*y + z*w), 1 - 2*(x*x + z*z), 2*(y*z - x*w)],
                [2*(x*z - y*w), 2*(y*z + x*w), 1 - 2*(x*x + y*y)]
            ])

            # Define vectors
            origin = np.array([0, 0, 0])
            x_vec = rot_matrix @ np.array([1, 0, 0])
            y_vec = rot_matrix @ np.array([0, 1, 0])
            z_vec = rot_matrix @ np.array([0, 0, 1])

            # Update plot data
            x_line.setData(pos=np.array([origin, x_vec]))
            y_line.setData(pos=np.array([origin, y_vec]))
            z_line.setData(pos=np.array([origin, z_vec]))

        except (ValueError, IndexError):
            pass

timer = QtCore.QTimer()
timer.timeout.connect(update)
timer.start(50)  # Update rate in milliseconds

QtWidgets.QApplication.instance().exec_()