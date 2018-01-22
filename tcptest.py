#!/usr/bin/python

import socket, sys, time

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('127.0.0.1', 9981))
sock.setblocking(1)
a = 'a' * int(sys.argv[1])
n1 = sock.send(a)
n2 = sock.recv(100)
print(n2)
time.sleep(100)
sock.close()
