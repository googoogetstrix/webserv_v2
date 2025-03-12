#!/usr/bin/env python3

import cgi

# Set the content type to HTML
print("Content-Type: text/html")    
print()  # Blank line to end headers

# print("OK")


# Parse GET and POST data
get_params = cgi.FieldStorage()
post_params = cgi.FieldStorage(environ={'REQUEST_METHOD': 'POST'})

# Preparing response
response_html = """
<html>
<head><title>CGI Output</title></head>
<body>
<h2>GET parameters:</h2>
<pre>{}</pre>
<h2>POST parameters:</h2>
<pre>{}</pre>
</body>
</html>
"""

# Format GET and POST data similarly to print_r in PHP
get_output = {key: get_params.getvalue(key) for key in get_params.keys()}
post_output = {key: post_params.getvalue(key) for key in post_params.keys()}

# Print the response
print(response_html.format(get_output, post_output))