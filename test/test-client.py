import socket
import binascii
import struct
import pdb


def pack_data(data):
    size = len(data)    
    size_crc = binascii.crc32(struct.pack('i', size))
    data_crc = binascii.crc32(data)
    return struct.pack('iii',  size, size_crc, data_crc) + data

def run_test():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('127.0.0.1', 32450))
    data = pack_data('hello,kitty')
    s.send(data)
    print(s.recv(1024))

    
if __name__ == '__main__':
    run_test()
