import serial

# Open the serial port
ser = serial.Serial('COM6', baudrate=115200, timeout=1)

try:
    while True:
        # Read a line of data from the serial port
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)
except KeyboardInterrupt:
    print("Program interrupted")
finally:
    ser.close()
