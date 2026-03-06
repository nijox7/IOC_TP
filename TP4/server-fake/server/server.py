#!/usr/bin/env python
import BaseHTTPServer
import CGIHTTPServer
import cgitb; cgitb.enable()

server = BaseHTTPServer.HTTPServer
handler = CGIHTTPServer.CGIHTTPRequestHandler
server_address = ("", 8000)
handler.cgi_directories = ["/cgi-bin"]

httpd = server(server_address, handler)
httpd.serve_forever()

while(True):
    str = f2s.readline()
    if(str.startswith("led_on") or str.startswith("led_off")) :
        s2f.write("led0" + '\n')
