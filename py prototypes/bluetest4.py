import bluetooth
from bluetooth import *
import time

print("performing inquiry...")

nearby_devices = discover_devices(lookup_names = True)

print("found %d devices" % len(nearby_devices))

for name, addr in nearby_devices:
     print(" %s - %s" % (addr, name))

def package(data):
     header,body = data.split(',')
     assert len(body) + len(header) < 254

     new = [len(header)+len(body)+1,len(header)]
     new += header.encode('ascii')
     new += body.encode('ascii')
     new = b''.join([bytes(char) for char in new])
     
     print('packet sent: ')
     print(new)
     return(new)

s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

#connect('20:15:05:06:63:33')
def connect(serverMACAddress = '20:16:01:12:48:47', port = 1):
     global s
     s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
     s.connect((serverMACAddress, port))
     #s.settimeout(1.5)
     s.setblocking(False)
     counter = 0

     time.sleep(1)
    
     while 1:
          print('count no [%s]'%counter)
          data = b''

          time.sleep(1);
          
          while True:
               try:
                    data += s.recv(1)

                    #print("CHAR", char)
                    if len(data) > data[0]:
                         print('PACKET', data)
                         data = b''

               except (OSError, IOError) as e:
                    print(e)
                    break

          if data != b'':
               print('FINAL DATA: ', data)
          else: print('no data')

          try:
               text = raw_input()
               # Note change to the old (Python 2) raw_input
          except NameError:
               text = input()

          if text == "quit": break
          elif text[:5] == 'pass:':
               now,new = text[5:].split(',')
               now = now+'\0'*(8-len(now))
               new = new+'\0'*(8-len(new))
               s.send(package('CHANGE_PASSWORD,'+now+new))
               
          else:
               s.send(package(text))

          counter += 1
            
     s.close()

