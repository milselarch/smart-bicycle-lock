import bluetooth
from bluetooth import *

print("performing inquiry...")

nearby_devices = discover_devices(lookup_names = True)

print("found %d devices" % len(nearby_devices))

for name, addr in nearby_devices:
     print(" %s - %s" % (addr, name))

def package(data):
     header,body = data.split(',')
     assert len(body) + len(header) < 254

     new = [len(header)+len(body)+1,len(header)]
     new += bytes(header.encode('ascii'))
     new += bytes(body.encode('ascii'))
     print('packet sent:',bytes(new))
     return(bytes(new))

s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

def connect(serverMACAddress = '20:16:01:12:48:47', port = 1):
     global s
     s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
     s.connect((serverMACAddress, port))
     s.settimeout(1.5)
     counter = 0
    
     while 1:
          print('count no [%s]'%counter)

          isEmpty = False
          isProper = False
          data = b''
          
          while isEmpty == False:
               try:
                    data += s.recv(1);
                    if len(data) > data[0]:
                         isEmpty = True
                         isProper = True
                         print('PACKET',data)
                         data = b''

               except OSError:
                    isEmpty = True
                    print('no data')

          if isProper == False:
               print('FINAL DATA',data)

          text = input() # Note change to the old (Python 2) raw_input
        

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

