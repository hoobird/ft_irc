import requests
from bs4 import BeautifulSoup
import csv

URL = "https://defs.ircdocs.horse/defs/numerics"
resp = requests.get(URL)
resp.raise_for_status()

soup = BeautifulSoup(resp.text, "html.parser")

# Locate the first table on the page
table = soup.find("table")

rows = []
headers = [th.get_text(strip=True) for th in table.find_all("th")]
rows.append(headers)

for tr in table.find_all("tr"):
    cols = tr.find_all(["td", "th"])
    if not cols:
        continue
    text = [c.get_text(strip=True) for c in cols]
    if text != headers:
        rows.append(text)

# Write to CSV
with open("numerics.csv", "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    writer.writerows(rows)

print("Saved numerics.csv with", len(rows)-1, "entries")
