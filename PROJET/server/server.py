#!/usr/bin/env python3

import http.server
server = http.server.HTTPServer

handler = http.server.CGIHTTPRequestHandler
handler.cgi_directories = ["/cgi"]

PORT = 8000
server_address = ("", PORT)

httpd = server(server_address, handler)   # objet "serveur"
httpd.serve_forever()
