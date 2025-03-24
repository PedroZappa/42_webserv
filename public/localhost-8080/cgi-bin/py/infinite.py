#!/usr/bin/env python3

import cgi
import cgitb
import random

def main():
    while (1): # Infinite loop
        pass

    print("Content-Type: text/html\n")
    print("\r\n\r\n")

    html = f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="icon" href="../../favicon.ico" type="image/png">
        <title>Infinite Loop</title>
    </head>
    <body>
        <h2>Infinite Loop</h2>
    </body>
    </html>
    """

    print(html)

if __name__ == "__main__":
    cgitb.enable()
    main()