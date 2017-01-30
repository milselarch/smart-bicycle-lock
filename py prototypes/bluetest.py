import bluetooth
from bluetooth import *

print("performing inquiry...")

nearby_devices = discover_devices(lookup_names = True)

print("found %d devices" % len(nearby_devices))

for name, addr in nearby_devices:
     print(" %s - %s" % (addr, name))

#connect('20:15:05:06:63:33')
def connect(serverMACAddress = '20:16:01:12:48:47', port = 1):
    s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    s.connect((serverMACAddress, port))
    s.settimeout(0.3)
    counter = 0
    
    while 1:
        print('count no [%s]'%counter)
        text = input() # Note change to the old (Python 2) raw_input
        try:
            data = s.recv(1);
            print(data)
        except OSError:
            print('no data')
            
        if text == "quit": break
        s.send(text)
        counter += 1
            
    sock.close()
