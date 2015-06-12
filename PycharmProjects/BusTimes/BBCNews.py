import feedparser
import re

WORDS = ["News"]

PRIORITY = 3

def handle(text, mic, profile):
    newsToRead = getNews()
    
    for x in newsToRead:
        mic.say(x)

def getNews() :
    d = feedparser.parse("http://feeds.bbci.co.uk/news/rss.xml?edition=uk")

    newsToRead = []
    for x in range(0, 5):
         newsToRead.append(d['entries'][x]['title'])

    return newsToRead

def isValid(text):
    return bool(re.search(r'\News\b', text, re.IGNORECASE))
