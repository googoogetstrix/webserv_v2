#!/usr/bin/env python3

import os
import http.cookies
import cgi
import cgitb
from datetime import datetime, timedelta, timezone

cgitb.enable()

cookies = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))
player_name_cookie = cookies.get("player_name")
avatar_cookie = cookies.get("avatar")
print(f"Debug: HTTP_COOKIE={os.environ.get('HTTP_COOKIE')}")
print(f"Debug: player_name_cookie={player_name_cookie}, avatar_cookie={avatar_cookie}")

form = cgi.FieldStorage()
player_name = form.getvalue("playerName", None)
avatar = form.getvalue("avatar", None)

if player_name == "" and avatar == "":
    # Delete cookies by setting their expiration date to the past
    expire_time = (datetime.now(timezone.utc) - timedelta(days=1)).strftime("%a, %d %b %Y %H:%M:%S GMT")
    cookies["player_name"] = ""
    cookies["player_name"]["path"] = "/"
    cookies["player_name"]["expires"] = expire_time

    cookies["avatar"] = ""
    cookies["avatar"]["path"] = "/"
    cookies["avatar"]["expires"] = expire_time

    print(f"Debug: Deleting cookies - player_name and avatar")
    print(cookies.output())  # Set cookies in the HTTP response header

    # Reset player_name and avatar to empty
    player_name = ""
    avatar = ""
# Set cookies if the form is submitted
elif player_name and avatar:
    expire_time = (datetime.now(timezone.utc) + timedelta(minutes=5)).strftime("%a, %d %b %Y %H:%M:%S GMT")
    cookies["player_name"] = player_name
    cookies["player_name"]["path"] = "/"
    cookies["player_name"]["expires"] = expire_time

    cookies["avatar"] = avatar
    cookies["avatar"]["path"] = "/"
    cookies["avatar"]["expires"] = expire_time
    print(f"Debug: Setting cookies - player_name={player_name}, avatar={avatar}")
    print(cookies.output()) 

# Use form data if available, otherwise fall back to cookies
if not player_name:
    player_name = player_name_cookie.value if player_name_cookie else ""
if not avatar:
    avatar = avatar_cookie.value if avatar_cookie else ""

# is_session_active = player_name and avatar
is_session_active = bool(player_name) and bool(avatar)
if player_name == "" and avatar == "":
    is_session_active = False

print("Content-Type: text/html\n")
print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cookie Player Test</title>
    <link rel="stylesheet" href="CGI-style.css">
</head>
<body>
""")

if is_session_active:
    # Show welcome message if cookies are set
    print(f"""
        <div class="welcome">
            <h1>Welcome, {player_name}!</h1>
            <img src="assets/{avatar}" alt="Avatar" style="width: 150px; height: 150px; border-radius: 50%;">
            <form action="cookieTest.py" method="POST">
                <input type="hidden" name="playerName" value="">
                <input type="hidden" name="avatar" value="">
                <button type="submit">Change player</button>
            </form>
        </div>
    """)
else:
    # Show registration form if cookies are not set
    print(f"""
        <div class="form-container">
        <h1>Player Registration</h1>
        <form action="cookieTest.py" method="POST">
            <label for="playerName">Player Name:</label>
            <input type="text" id="playerName" name="playerName" required 
                   placeholder="Enter your player name" minlength="2" maxlength="20">

            <h3>Choose Your Avatar</h3>
            <div class="avatar-options">
                <div class="avatar-option">
                    <input type="radio" id="avatar1" name="avatar" value="avatar1.jpg" checked>
                    <label for="avatar1">
                        <img src="assets/avatar1.jpg" alt="Avatar 1">
                    </label>
                </div>
                <div class="avatar-option">
                    <input type="radio" id="avatar2" name="avatar" value="avatar2.jpg">
                    <label for="avatar2">
                        <img src="assets/avatar2.jpg" alt="Avatar 2">
                    </label>
                </div>
                <div class="avatar-option">
                    <input type="radio" id="avatar3" name="avatar" value="avatar3.jpg">
                    <label for="avatar3">
                        <img src="assets/avatar3.jpg" alt="Avatar 3">
                    </label>
                </div>
            </div>
            <input type="submit" value="Register Player">
        </form>
        </div>
    """)

print("""
</body>
</html>
""")