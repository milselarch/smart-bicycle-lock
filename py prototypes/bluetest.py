import bluetooth
from bluetooth import *

import time

print("performing inquiry...")

nearby_devices = discover_devices(lookup_names = True)

print("found %d devices" % len(nearby_devices))

for name, addr in nearby_devices:
     print(" %s - %s" % (addr, name))

#connect('20:15:05:06:63:33')
#connect('20:16:01:12:48:47')
timeout = 1
s = None
     
def connect(serverMACAddress = '20:16:01:12:48:47', port = 1):
     global s, timeout
     s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
     s.connect((serverMACAddress, port))
     #timeout doesn't matter when you have nonblocking sockets
     #s.settimeout(1)
     s.setblocking(0)
     counter = 0

     while True:          
          print('in[%s]'%counter)

          try:
               text = raw_input()
               # Note change to the old (Python 2) raw_input
          except NameError:
               text = input()

          time.sleep(timeout)
          data = ''
               
          while True:
               try:
                    data += s.recv(1);
               except (OSError, IOError) as e:
                    print(e)
                    break

          if data == '': print('no data')
          else: print('>>> ' + data)
          
          if text == "quit": break
          s.send(text)
          counter += 1

     s.close()
