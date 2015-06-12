import feedparser
import re

WORDS = ["News"]

PRIORITY = 1

def handle(text, mic, profile):
    newsToRead = getNews()
    mic.say(newsToRead.extend)

def getNews() :
    d = feedparser.parse("http://feeds.bbci.co.uk/news/rss.xml?edition=uk")

    newsToRead = []
    count = len(d['entries'])
    for x in range(0, 5):
         newsToRead.append(d['entries'][x]['title']['summary'])

    return newsToRead

def isValid(text):
    return bool(re.search(r'\News\b', text, re.IGNORECASE))
