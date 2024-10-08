import serial
import csv
import time

# Define the serial port and baud rate
serial_port = 'COM6'
baud_rate = 115200  # Adjust this if your baud rate is different

# Open the serial port
ser = serial.Serial(serial_port, baud_rate)

# Define the CSV file to write to
csv_file = 'flight_data.csv'

# Open the CSV file in append mode
with open(csv_file, mode='a', newline='') as file:
    writer = csv.writer(file)
    
    # Write the header if the file is empty
    if file.tell() == 0:
        writer.writerow(['Time', 'Temperature', 'Pressure', 'Altitude', 'Pitch', 'Yaw', 'Roll', 
                         'AccelX', 'AccelY', 'AccelZ', 'ServoX', 'ServoY', 'Phase'])

    try:
        while True:
            # Read a line from the serial port
            line = ser.readline().decode('utf-8').strip()
            
            # Split the line into data fields (assuming data is comma-separated)
            data = line.split(', ')
            
            # Write the data to the CSV file
            writer.writerow(data)
            
            # Optional: print the data to the console
            print(data)
            
            # Optional: add a delay
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("Data collection stopped by user.")

    finally:
        # Close the serial port
        ser.close()
