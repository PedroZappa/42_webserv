#!/usr/bin/env python3

import os, hashlib, time, shelve
import http.cookies as Cookie

print("Content-Type: text/html")

path = os.environ.get('DOCUMENT_ROOT', '.') + '/tmp'
if not os.path.exists(path):
    os.makedirs(path)

# Inspired in https://cgi.tutorial.codepoint.net/a-session-class
str_cookie = os.environ.get('HTTP_COOKIE', '')
cookie = Cookie.SimpleCookie(str_cookie)
if cookie.get('sid'):
	sid = cookie['sid'].value
else:
	timestamp = repr(time.time()).encode()
	sid = hashlib.sha1(timestamp).hexdigest()
	print(f"Set-Cookie: sid={sid}; HttpOnly; SameSite=Strict; Max-Age=3600")
		
cookie.clear()
cookie['sid'] = sid

data = shelve.open(
	'%s/sess_%s' % (path, sid), 
	writeback=True
)
os.chmod('%s/sess_%s' % (path, sid), 0o777)

if not data.get('cookie'):
    data['cookie'] = {'expires':''}

lastvisit = data.get('lastVisit')
if lastvisit:
    message = 'on ' + time.asctime(time.gmtime(float(lastvisit)))
else:
    message = 'never'
visitCount = data.get('visitCount')
if not visitCount:
    visitCount = 0

# Save the current time in the session
data['lastVisit'] = repr(time.time())
data['visitCount'] = visitCount + 1
data.sync()

print("\r\n\r\n")

html = f"""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Session</title>
</head>
<body>
	<h2>Your last visit was {message} </h2>
    <h2>You visited this page {visitCount} times </h2>
</body>
</html>
"""

print(html)
data.close()