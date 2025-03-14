#!/usr/bin/env python3

import cgi
import cgitb
import os
import http.cookies  # for handling cookies
import shelve  # for session management (a simple file-based session handling)

# Enable debugging
cgitb.enable()

# Set the content type
print("Content-Type: text/plain;charset=utf-8")
print()  # End of headers

# Read and display GET parameters
print("GET parameters:")
print(cgi.FieldStorage())

# Read and display POST parameters
print("\nPOST parameters:")
form = cgi.FieldStorage()
if form:
    print(form)
else:
    print("No POST data received.")

# Read and display uploaded files
print("\nUploaded files:")
for field in form.keys():
    if form[field].filename:  # Check if the field contains a file
        print(f"{field}: {form[field].filename}")

# Read and display cookies
print("\nCookies:")
cookie_string = os.environ.get("HTTP_COOKIE", "")
cookies = http.cookies.SimpleCookie(cookie_string)
for key, morsel in cookies.items():
    print(f"{key}: {morsel.value}")

# Simple session handling using shelve (for demo purposes, not production-ready)
session_cookie_name = "session_id"
session_id = cookies.get(session_cookie_name)
session_data = {}

if session_id:
    # Open the session file
    with shelve.open('sessions') as session_store:
        session_data = session_store.get(session_id.value, {})
else:
    # Create a new session if one does not exist
    session_id = "new-session-id"  # In a real application, you would generate a unique ID
    session_data = {}
    with shelve.open('sessions') as session_store:
        session_store[session_id] = session_data

# Display session data
print("\nSession data:")
print(session_data)

# Optional: Print the session ID
print(f"\nSession ID: {session_id}")

# If you want to set a cookie for the session in the response
print(f'Set-Cookie: {session_cookie_name}={session_id}; Path=/')