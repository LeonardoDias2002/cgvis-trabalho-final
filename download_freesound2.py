import urllib.request
import json
import re

def search_freesound(query, filename):
    url = f"https://freesound.org/search/?q={urllib.parse.quote(query)}"
    req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
    try:
        html = urllib.request.urlopen(req).read().decode('utf-8')
        match = re.search(r'(https://cdn\.freesound\.org/previews/[^"]+\.mp3)', html)
        if match:
            mp3_url = match.group(1)
            print(f"Downloading {query} from {mp3_url} to {filename}")
            urllib.request.urlretrieve(mp3_url, filename)
            return True
    except Exception as e:
        print(f"Error for {query}: {e}")
    return False

import urllib.parse
search_freesound("open field birds wind", "data/ambiente.mp3")
