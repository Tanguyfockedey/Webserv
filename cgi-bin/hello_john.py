#!/usr/bin/python3

import cgi
import sys
import os

form = cgi.FieldStorage()
name = form.getvalue('name')
print(os.environ["QUERY_STRING"], file=sys.stderr)
print("<html>")
print("<head>")
print("<title>webserv - CGI</title>")
print("</head>")
print("<body>")
print("<center><h2>Hello %s !</h2></center>" % (name))
print("</body>")
print("</html>")
