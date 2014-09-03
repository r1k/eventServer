#!/usr/bin/env python

from socket import socket, AF_INET, SOCK_STREAM
from collections import deque
import argparse
import select
import signal
import json
import curses

import pydevd

class eventHandler:
    MAX_NUM_PIDS = 8192

    def __init__(self):
        self.bitrateLookup = {"0": 0}
        self.stdscr = curses.initscr()
        self.stdscr.addstr(0, 0, "Waiting for first bitrate")
        self.height, self.width = self.stdscr.getmaxyx()
        self.stdscr.refresh()
        self.br_pos = 18
        self.activity_toggle = ""

    def printTitle(self):
        if self.activity_toggle == "*":
            self.activity_toggle = ""
        else:
            self.activity_toggle = "*"
            
        self.stdscr.addstr(0, 0, "PID")
        self.stdscr.addstr(0, self.br_pos - 5, "Bitrate")
        self.stdscr.addstr(0, self.br_pos + 9, self.activity_toggle)
        self.height, self.width = self.stdscr.getmaxyx()
        self.stdscr.refresh()

    def simplifyBitrate(self, bitrate):
        units = " b/s"
        if ((bitrate * 1.0) > 1000):
            bitrate /= 1000.0
            units = " Kb/s"
        if (bitrate > 1000):
            bitrate /= 1000.0
            units = " Mb/s"
        return "%.3f" % bitrate, units

    def printBitrateTable(self):
        self.stdscr.clear()
        self.printTitle()
        line = 1
        cummulative_bitrate = 0
        pidList = [int(x) for x in list(self.bitrateLookup)]
        for i in sorted(pidList):
            brate = int(self.bitrateLookup[str(i)])
            cummulative_bitrate += brate
            self.stdscr.addstr(line, 0, ("%4d", i))
            bitrate, units = self.simplifyBitrate(brate)
            self.stdscr.addstr(line, self.br_pos - len(bitrate), bitrate)
            self.stdscr.addstr(line, self.br_pos, units)
            line += 1
            if line > self.height:
                break
        if line > self.height:
            line -=1
        bitrate, units = self.simplifyBitrate(cummulative_bitrate)
        self.stdscr.addstr(line, 0, "Total")
        self.stdscr.addstr(line, self.br_pos - len(bitrate), bitrate)
        self.stdscr.addstr(line, self.br_pos, units)
        self.stdscr.refresh()

    def termResize(self):
        self.stdscr = curses.initscr()

    def processEvent(self, ev):

        if ev is None:
            return

        if ev['type'] == 'bitrate_event':
            self.bitrateLookup[str(ev['pid'])] = int(ev['bitrate'])
            self.printBitrateTable()
            
        elif ev['type'] == 'bitrate_list':
            self.bitrateLookup = {}
            for pair in ev['bitrates']:
                self.bitrateLookup[str(pair['pid'])] = int(pair['bitrate'])
            self.printBitrateTable()

    def __del__(self):
        curses.nocbreak()
        self.stdscr.keypad(0)
        curses.echo()
        curses.endwin()


class dataHandler:

    def __init__(self):
        self.eventObjectList = deque()
        self.unbufferedData = ""

    def getNextEvent(self):
        yield(self.eventObjectList.pop())

    def findNextJSONBlock(self, unbuffered):
        start = end = depth = 0
        start = 0
        end = 0
        escapeNext = False
        position = 0

        for i in unbuffered:
            if escapeNext:
                escapeNext = False

            elif i == '\\':
                escapeNext = True

            elif escapeNext is False:
                if i == '{':
                    if depth == 0:
                        start = position
                    depth += 1
                elif i == '}':
                    depth -= 1
                    if depth == 0:
                        end = position
                        break
                    elif depth < 0:
                        # error condition we got a '}' without an opening '{'
                        # skip over data upto here
                        start = end = position + 1

            position += 1

        if end > start:
            return unbuffered[start:end + 1], (end + 1)
        else:
            return None, start

    def addRawData(self, unbuffered):

        toProcess = self.unbufferedData
        toProcess += unbuffered
        searchIndex = 0
        exhausted = False
        try:
            while not exhausted:
                block, into = self.findNextJSONBlock(toProcess[searchIndex:])
                searchIndex += into
                if block is not None:
                    # turn block into JSON struct the put on deque
                    self.eventObjectList.append(json.loads(block))
                else:
                    exhausted = True
            # keep remaining data for next time
            self.unbufferedData = toProcess[searchIndex:]
        except:
            # clear buffer and carry on
            self.unbufferedData = ""


class clientSckt:

    HOST = "127.0.0.1"
    PORT = 60000

    def __init__(self):
        pass

    def createConnection(self, Host=None, Port=None):
        HOST = Host
        PORT = Port
        if HOST is None:
            HOST = self.HOST
        if PORT is None:
            PORT = self.PORT

        ADDR = (HOST, PORT)
        print (("Connecting to ", ADDR))
        self.sckt = socket(AF_INET, SOCK_STREAM)
        # bind our socket to the address
        self.sckt.connect((ADDR))

    def send(self, data):
        return self.sckt.send(data)

    def close(self):
        return self.sckt.close()


def ParseCommandLine():
    parser = argparse.ArgumentParser(description='Receives events from eventServer')
    parser.add_argument('ipaddr', metavar='IP', help='The ipaddress of the eventServer')
    parser.add_argument('port', metavar='PORT', type=int, help='The listening port of the eventServer')
    return parser.parse_args()


continueRunning = True

def sigHandler(signum, frame):
    continueRunning = False
    
if __name__ == '__main__':

    pydevd.settrace()                       # for remote debug in Eclispe
    args = ParseCommandLine()
    signal.signal(signal.SIGINT, sigHandler)
    signal.signal(signal.SIGTERM, sigHandler)
    cls = clientSckt()
    cls.createConnection(args.ipaddr, args.port)

    dh = dataHandler()
    eh = eventHandler()

    input_src = [cls.sckt]
    while (continueRunning):
        try:
            inputready, outputready, exceptready = select.select(input_src, [], [], None)
            for s in inputready:
                if s == cls.sckt:
                    # handle all other sockets
                    data = s.recv(1024)
                    if data:
                        dh.addRawData(data)
                    else:
                        s.close()
                        input_src.remove(s)

            # deal with event
            for ev in dh.getNextEvent():
                eh.processEvent(ev)

        except Exception as err:
            print (("Exception received: " + str(err)))
            continueRunning = False

    print ("closing connection")
    cls.close
