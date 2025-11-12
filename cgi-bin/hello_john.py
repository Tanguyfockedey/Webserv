#!/usr/bin/python3

import cgi

form = cgi.FieldStorage()
name = form.getvalue('name')

print("<html>")
print("<head>")
print("<title>webserv - CGI</title>")
print("</head>")
print("<body>")
print("<center><h2>Hello %s !</h2></center>" % (name))
print("</body>")
"""print("<body><center>")
print("<p><a href=\"page1.html\">Go to page 1</a></p>") # broken?
print("<p><a href=\"/\">Go back to the index page</a></p>")
print("</body></center>") """
print("</html>")
