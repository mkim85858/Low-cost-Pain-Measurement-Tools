import serial
import csv
import os
from datetime import datetime

# === CONFIG ===
BLUETOOTH_PORT = 'COM7'  # or '/dev/rfcomm0' on Linux
BAUD_RATE = 115200

def to_hex_str(byte_data):
    return ' '.join(f'{b:02X}' for b in byte_data)

def get_new_csv_filename(file_index):
    """Generate a unique CSV filename in the script's directory."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    base_filename = "logPressure"
    return os.path.join(script_dir, f"{base_filename}{file_index}.csv")

def create_new_csv(file_index):
    """Create a new CSV file and return the writer and file object"""
    filename = get_new_csv_filename(file_index)
    print(f"Switching to new file: {filename}")
    file = open(filename, mode='w', newline='')
    writer = csv.writer(file)
    writer.writerow(['Time (ds)', 'Pressure (hPa)'])
    return writer, file

def main():
    try:
        ser = serial.Serial(BLUETOOTH_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {BLUETOOTH_PORT} at {BAUD_RATE} baud.")
    except serial.SerialException as e:
        print(f"Error: {e}")
        return

    buffer = bytearray()
    file_index = 1
    writer, file = create_new_csv(file_index)

    try:
        while True:
            if ser.in_waiting > 0:
                incoming = ser.read(ser.in_waiting)
                buffer.extend(incoming)
                # Process full pairs: 1 byte + 1 byte + 1 byte
                while len(buffer) >= 3:
                    data1 = buffer[0]
                    data2 = buffer[1]
                    time = buffer[2]
                    del buffer[:3]  # remove processed bytes
                    if (data1 == 0x00 and data2 == 0x00):
                        if (time == 0x00):
                            file.close()
                            file_index += 1
                            writer, file = create_new_csv(file_index)
                            continue
                        else:
                            time = "NaN"
                    pressure = (data1 << 8) | data2
                    print(f"Received -> Time: {time}, Pressure: {pressure}")
                    writer.writerow([time, pressure])
                    file.flush()
    except KeyboardInterrupt:
        print("Stopped by user.")
    finally:
        ser.close()

if __name__ == '__main__':
    main()