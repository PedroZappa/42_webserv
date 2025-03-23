import cgi
import cgitb
import random

def main():
    print("Content-Type: text/html\n")
    print("\r\n\r\n")

    clownList = [
        "elonmusk", "realDonaldTrump",
        "benshapiro", "RepMTG",
        "miguelarrudapt", "ritamariamatias",
        "Antunes1", "Numeiro"
    ]

    clown = clownList[random.randint(0, len(clownList))]

    html = f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="icon" href="../../favicon.ico" type="image/png">
        <title>Clown Roulette</title>
    </head>
    <body>
        <h2>Clown Roulette</h2>
        <a class="twitter-timeline" href="https://twitter.com/{clown}?ref_src=twsrc%5Etfw">
            Tweets by {clown}
        </a>
        <script async src="https://platform.twitter.com/widgets.js" charset="utf-8">
        </script>
    </body>
    </html>
    """

    print(html)

if __name__ == "__main__":
    cgitb.enable()
    main()