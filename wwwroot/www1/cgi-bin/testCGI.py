#!/usr/bin/env python3

import cgi
import cgitb

# Enable error reporting
cgitb.enable()

# Get form data
form = cgi.FieldStorage()

# Get the number of rows, columns, and cell color from the form data
rows = int(form.getvalue("rows", 5))
columns = int(form.getvalue("columns", 5))
color = form.getvalue("color", "#FF0000")  # Default to white if no color is provided

# Generate HTTP response headers
print("Content-Type: text/html")
print()

# Generate HTML content
print("<html>")
print("<head>")
print("<title>Generated Table</title>")
print("<style>")
print("table { border-collapse: collapse; width: auto; }")
print("th, td { border: 1px solid black; padding: 0; text-align: center; width: 50px; height: 50px;}")
# print("td { background-color: " + color + "; }")
print(".oddRow .oddCol { background-color: " + color + "; }")
print(".evenRow .evenCol { background-color: " + color + "; }")
print("</style>")
print("</head>")
print("<body>")
print("<h1>Generated Table</h1>")

# Generate the table
print("<table>")
for r in range(rows):
    if r % 2 == 1:
        print("<tr class=oddRow>")
    else:
        print("<tr class=evenRow>")
    for c in range(columns):
        if c % 2 == 1:
            print("<td class=oddCol></td>")
        else:
            print("<td class=evenCol></td>")
    print("</tr>")
print("</table>")

print("</body>")
print("</html>")