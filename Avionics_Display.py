import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget
from PyQt5.QtCore import QTimer
import pyqtgraph as pg
import numpy as np
import serial

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        print("Creating MainWindow")
        self.setWindowTitle('Real-time IMU Data')

        # Set up serial port
        self.ser = serial.Serial('COM6', baudrate=115200, timeout=1)

        # Create a central widget and set the layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout()
        central_widget.setLayout(layout)

        # Create a PlotWidget and add it to the layout
        self.plot_widget = pg.PlotWidget()
        layout.addWidget(self.plot_widget)

        # Set up data arrays
        self.max_samples = 100
        self.sample_index = np.arange(self.max_samples)
        self.roll_data = np.zeros(self.max_samples)
        self.pitch_data = np.zeros(self.max_samples)
        self.yaw_data = np.zeros(self.max_samples)

        # Add plots to the widget
        self.roll_plot = self.plot_widget.plot(self.sample_index, self.roll_data, pen='r', name='Roll')
        self.pitch_plot = self.plot_widget.plot(self.sample_index, self.pitch_data, pen='g', name='Pitch')
        self.yaw_plot = self.plot_widget.plot(self.sample_index, self.yaw_data, pen='b', name='Yaw')

        # Set up a timer to update the plot
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(50)  # Update every 50 ms

    def update_plot(self):
        # Read from the serial port
        if self.ser.in_waiting > 0:
            line = self.ser.readline().decode('utf-8').rstrip()
            try:
                # Parse the data
                parts = line.split(',')
                roll = float(parts[0].split(':')[1])
                pitch = float(parts[1].split(':')[1])
                yaw = float(parts[2].split(':')[1])

                # Update the data arrays
                self.roll_data = np.roll(self.roll_data, -1)
                self.pitch_data = np.roll(self.pitch_data, -1)
                self.yaw_data = np.roll(self.yaw_data, -1)
                self.roll_data[-1] = roll
                self.pitch_data[-1] = pitch
                self.yaw_data[-1] = yaw

                # Update the plots
                self.roll_plot.setData(self.sample_index, self.roll_data)
                self.pitch_plot.setData(self.sample_index, self.pitch_data)
                self.yaw_plot.setData(self.sample_index, self.yaw_data)

            except Exception as e:
                print(f"Error parsing data: {e}")

# Create a Qt application
app = QApplication(sys.argv)

# Create and show the main window
window = MainWindow()
window.show()

# Start the Qt event loop
sys.exit(app.exec_())
