#!/usr/bin/env python3
import cgi 

form = cgi.FieldStorage()
print("Content-type: text/html; charset=utf-8\n")
# print(form.getvalue("name"))
print()

html = """<!DOCTYPE html>
<head>
    <title>Mon programme</title>
</head>Z
<body>
    <form action="main.py" method="post">
        <input type="checkbox" name="led_checkbox"/>
        <input type="button" name="button" value="Send"/>
    </form>
<!--    <form action="main.py" method="post">
        <input type="text" name="name" placeholder="Votre nom" />
        <input type="submit" name="send" value="Envoyer information au serveur"/>
        <
    </form>
    <box>

    </box>
-->
</body>
</html>
"""

print(html)