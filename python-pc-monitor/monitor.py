import psutil
import wmi
import clr
import time
import sys
#pip install pyserial
import serial
import serial.tools.list_ports

CONST_LOOPTIME = 5.0

if __name__ == "__main__":

    CPULoad = 0
    CPUTemp = 0
    RAMLoad = 0
    RAMUsed = 0
    GPULoad = 0
    GPUTemp = 0

    starttime = time.time()

    w = wmi.WMI(namespace="root\OpenHardwareMonitor")

    port = "" # The serial port to which we will connect

    # find the correct port (at least down to an arduino uno)
    for comport in serial.tools.list_ports.comports():
        if "Arduino Uno" in str(comport.description):
            port = comport.device
            print("Identified port:")
            print(comport.description)
            break
    
    # If finding the correct port failed, end the program
    if port == "":
        sys.exit()

    ser = serial.Serial(port,9600)

    while True:

        HWInfo = w.Sensor()
        for sensor in HWInfo:
            if "Load" == sensor.SensorType and "CPU Total" == sensor.Name:
                CPULoad = sensor.Value
            elif "Temperature" == sensor.SensorType and "CPU Package" == sensor.Name:
                CPUTemp = sensor.Value
            elif "Load" == sensor.SensorType and "Memory" == sensor.Name:
                RAMLoad = sensor.Value
            elif "Data" == sensor.SensorType and "Used Memory" == sensor.Name:
                RAMUsed = sensor.Value
            elif "Load" == sensor.SensorType and "GPU Core" == sensor.Name:
                GPULoad = sensor.Value
            elif "Temperature" == sensor.SensorType and "GPU Core" == sensor.Name:
                GPUTemp = sensor.Value

        output = str(int(CPULoad)).rjust(3, " ") + "*" + str(int(CPUTemp)).rjust(3, " ") + "#" + str(int(RAMLoad)).rjust(3, " ") + "$" + str(int(RAMUsed)).rjust(2, " ") + "&"
        print(output)
        ser.write(output.encode('utf-8'))
        
        # Restart the loop every CONST_LOOPTIME seconds
        #print("loop calculation: " + (CONST_LOOPTIME - ((time.time() - starttime) % CONST_LOOPTIME)))
        time.sleep(CONST_LOOPTIME - ((time.time() - starttime) % CONST_LOOPTIME))

