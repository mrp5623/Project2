import time, json, requests, random
from playwright.sync_api import sync_playwright
from urllib.parse import urlparse, parse_qs, quote
from bs4 import BeautifulSoup

#rather than new request every time
session = requests.Session()

#get fresh API key
def fetch_key():
    print("Fetching Algolia key")

    #use playwright session
    with sync_playwright() as p:
        #open real chromium tab to avoid blovk
        browser = p.chromium.launch(headless=False)
        page = browser.new_page()

        key = None

        #parse network tab of dev tools for key
        def on_request(r):
            nonlocal key
            if "algolia.net" in r.url and "x-algolia-api-key" in r.url:
                key = parse_qs(urlparse(r.url).query).get("x-algolia-api-key", [None])[0]

        page.on("request", on_request)
        page.goto("https://www.fragrantica.com/search/", timeout=30000, wait_until="domcontentloaded")
        page.wait_for_timeout(2000)
        browser.close()

    if key:
        return key

       

def algolia_post(body, key):
    #via dev tools
    params = {
        "x-algolia-agent": "Algolia for JavaScript",
        "x-algolia-api-key": key,
        "x-algolia-application-id": "FGVI612DFZ"
    }
    headers = {
        "content-type": "application/x-www-form-urlencoded",
        "origin": "https://www.fragrantica.com",
        "referer": "https://www.fragrantica.com/",
        "user-agent": "Mozilla/5.0",
        "connection": "keep-alive"
    }

    while True:
    
        r = session.post(
            "https://fgvi612dfz-dsn.algolia.net/1/indexes/*/queries",
            params=params,
            data=body,
            headers=headers,
            timeout=15
        )

        if r.status_code == 200:
            return r.json(), key
        elif r.status_code == 429:
            print("Rate limited or key expired")
            key = fetch_key()
            params["x-algolia-api-key"] = key
        else:
            print("Retrying:", r.status_code)

        

def make_body(note, company=None, page=0):
    #filter by note, split by company if over 1000 perfumes
    filter = f'[["ingredients.EN:{note}"' + (f',"designer_meta.parent_company:{company}"' if company else "") + "]]"

    inner = "&".join([
        "attributesToRetrieve=%5B%22naslov%22%2C%22slug%22%2C%22id%22%5D",
        f"facetFilters={quote(filter)}", #encode automatically
        "facets=%5B%22designer_meta.parent_company%22%5D",
        "hitsPerPage=1000",
        "maxValuesPerFacet=1000",
        f"page={page}",
        "query="
    ])

    return json.dumps({
        "requests": [{
            "indexName": "fragrantica_perfumes",
            "params": inner
        }]
    })


def collect_hits(note, key, company=None):
    results = set()
    page = 0

    while True:
        data, key = algolia_post(make_body(note, company, page), key)
        result_block = data.get("results", [{}])[0]
        hits = result_block.get("hits", [])

        for hit in hits:
            #first half of url
            slug = hit.get("slug")
            #second half of url
            pid = hit.get("id") or hit.get("objectID")
            if slug and pid:
                #build url
                url = f"https://www.fragrantica.com/perfume/{slug}-{pid}.html"
                #name
                results.add((hit.get("naslov", "UNKNOWN"), url))
        
        #go until no more perfumes
        if not hits:
            break
        page += 1

    #return key in case it was changed
    return results, key

#capped at 1000 but lowk that might be enough, company splitting causing too many 429s
def get_perfumes_for_note(note, key):
    print(f"\n{note}:")

    data, key = algolia_post(make_body(note), key)
    result_block = data.get("results", [{}])[0]
    nb_hits = result_block.get("nbHits", "?")
    print(f"Total hits: {nb_hits}")

    #if hits less than 1000
    results, key = collect_hits(note, key)

    print(f"Collected {len(results)} perfumes for {note}")

    return results, key


def get_notes():
    print("Fetching notes")

    with sync_playwright() as p:
        browser = p.chromium.launch(headless=False)
        page = browser.new_page()
        page.goto("https://www.fragrantica.com/notes/", wait_until="domcontentloaded")
        page.wait_for_selector("a.rounded-lg p", timeout=15000)
        html = page.content()
        browser.close()

    soup = BeautifulSoup(html, "html.parser")
    notes = {}
    #every note where class contains rounded-lg except last which isnt a note for some reason
    for a in soup.select("a.rounded-lg")[:-1]:
        #find text within tag
        p_tag = a.find("p")
        if not p_tag:
            continue
        name = p_tag.get_text(strip=True)

        #empty set for now
        notes[name] = set()

    print(f"Found {len(notes)} notes.")
    return notes


def main():
    key = fetch_key()

    notes = get_notes()

    full_map = {}

    #create map to be turned into json
    for note in notes:
        perfumes, key = get_perfumes_for_note(note, key)
        if not perfumes:
            continue
        full_map[note] = [[name, url] for (name, url) in perfumes]

    #create json file to be parsed in cpp code
    with open("perfume_map.json", "w") as f:
        json.dump(full_map, f, indent=2)


if __name__ == "__main__":
    main()