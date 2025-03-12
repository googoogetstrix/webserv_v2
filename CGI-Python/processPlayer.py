#!/usr/bin/env python

import cgi
import cgitb
import os
import shutil
from pathlib import Path

cgitb.enable()

def saveUploadedFile(fileItem, uploadDir):
    if not fileItem.filename:
        return None
    
    filename = os.path.basename(fileItem.filename)
    filepath = os.path.join(uploadDir, filename)
    
    with open(filepath, 'wb') as f:
        shutil.copyfileobj(fileItem.file, f)
    
    return filename

print("Content-Type: text/html\n")

form = cgi.FieldStorage()

playerName = form.getvalue('playerName', 'Unknown')
selectedAvatar = form.getvalue('avatar', 'assets/avatar1.jpg')
customAvatar = form['customAvatar'] if 'customAvatar' in form else None

# Create uploads directory if it doesn't exist
uploadDir = os.path.join(os.path.dirname(__file__), 'uploads')
Path(uploadDir).mkdir(exist_ok=True)

# Handle file upload if custom avatar was provided
avatarPath = ''
if customAvatar and customAvatar.filename:
    avatarPath = saveUploadedFile(customAvatar, uploadDir)
else:
    avatarPath = selectedAvatar

print("""
<!DOCTYPE html>
<html>
<head>
    <title>Registration Complete</title>
    <link rel="stylesheet" href="CGI-style.css">
</head>
<body>
    <div class="success">
        <h2>Registration Successful!</h2>
""")
print("<p>Welcome, " + playerName + "</p>")
if customAvatar:
	print("<img src=" + '/uploads/' + avatarPath + " alt=" + playerName + " Avatar>")
else:
	print("<img src=" + '/assets/' + avatarPath + " alt=" + selectedAvatar + ">")
print("""    </div>
</body>
</html>
""")
