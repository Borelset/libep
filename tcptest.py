#!/usr/bin/python

import socket, sys

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('127.0.0.1', 9981))
sock.setblocking(0)
a = 'a' * int(sys.argv[1])
b = 'b' * int(sys.argv[2])
n1 = sock.send(a)
n2 = 0
try:
    n2 = sock.send(b)
except socket.error as ex:
    print ex
print n1 
print n2
sock.close()
