#!/usr/bin/env python3

import cgi
import http.server

class SimpleHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        # Get and print query parameters
        params = cgi.parse_qs(self.path.split('?', 1)[1] if '?' in self.path else '')
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(b"<html><body>")
        self.wfile.write(b"<h2>GET parameters:</h2>")
        self.wfile.write(b"<pre>")
        self.wfile.write(str(params).encode('utf-8'))
        self.wfile.write(b"</pre>")
        self.wfile.write(b"</body></html>")

    def do_POST(self):
        # Get and print form data
        form = cgi.FieldStorage(fp=self.rfile, headers=self.headers, environ={'REQUEST_METHOD': 'POST'})
        post_data = {field: form.getvalue(field) for field in form.keys()}
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(b"<html><body>")
        self.wfile.write(b"<h2>POST parameters:</h2>")
        self.wfile.write(b"<pre>")
        self.wfile.write(str(post_data).encode('utf-8'))
        self.wfile.write(b"</pre>")
        self.wfile.write(b"</body></html>")

def run(server_class=http.server.HTTPServer, handler_class=SimpleHTTPRequestHandler):
    server_address = ('', 8000)  # Serve on localhost:8000
    httpd = server_class(server_address, handler_class)
    print("Starting server on port 8000...")
    httpd.serve_forever()

if __name__ == '__main__':
    run()