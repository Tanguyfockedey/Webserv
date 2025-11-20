#!/usr/bin/python3

import warnings
import cgi

warnings.filterwarnings("ignore", category=DeprecationWarning)
form = cgi.FieldStorage()
name = form.getvalue('name')

print("<html>")
print("<head>")
print("<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\">")
print("<link rel=\"stylesheet\" type=\"text/css\" href=\"/styles.css\">")
print("<title>webserv - CGI</title>")
print("</head>")
print("<body>")
print("<center>")
print("<h2>Hello %s !</h2>" % (name))
# print("<p><a href=\"page_cgi.html\">Go back to the CGI page</a></p>") # broken?
print("<p><a href=\"/\">Go back to the index page</a></p>")
print("</center>")
print("</body>")
print("</html>")
