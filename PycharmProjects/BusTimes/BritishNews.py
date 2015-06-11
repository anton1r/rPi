__author__ = 'tony.rafferty'
import feedparser

d = feedparser.parse("http://feeds.bbci.co.uk/news/rss.xml?edition=uk", )

count = len(d['entries'])
for x in range(0, 5):
    print(d['entries'][x]['title'])
    print(d['entries'][x]['summary'])

