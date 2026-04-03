import cgi

print("Content-Type: text/html; charset=utf-8\n")
data = cgi.FieldStorage()
nom = data.getvalue('nom')

print(nom)