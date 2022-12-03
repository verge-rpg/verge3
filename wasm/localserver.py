#!/usr/bin/env python
import os

try:
    import http.server as SimpleHTTPServer
except:
    import SimpleHTTPServer

try:
    import socketserver as SocketServer
except:
    import SocketServer

PORT = 8000

class Handler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    pass

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    pass 

Handler.extensions_map['.wasm'] = 'application/wasm'

#httpd = SocketServer.TCPServer(("", PORT), Handler)
httpd = ThreadedTCPServer(("", PORT), Handler)

pwd = os.getcwd()
try:
    os.chdir('..')
    print(f'serving {os.getcwd()} at port {PORT}. go to http://127.0.0.1/wasm')
    httpd.serve_forever()
finally:
    pwd = os.chdir()
