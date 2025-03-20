import requests

url = "http://example.com/upload"
files = {'image': ('image.png', open('path/to/image.png', 'rb'), 'image/png')}

response = requests.post(url, files=files)

print(response.status_code)

