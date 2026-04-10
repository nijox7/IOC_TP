#!/usr/bin/env python

import http.server
 
PORT = 8888
server_address = ("", PORT)

server = http.server.HTTPServer
handler = http.server.CGIHTTPRequestHandler
handler.cgi_directories = ["/cgi-bin"]
print("Serveur actif sur le port :", PORT)

httpd = server(server_address, handler)
httpd.serve_forever()

# while(True):
#     str = f2s.readline()
#     if(str.startswith("led_on") or str.startswith("led_off")) :
#         s2f.write("led0" + '\n')