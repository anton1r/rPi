from lxml import html
import requests

page = requests.get('http://www.mybustracker.co.uk/?module=mobile&mode=1&busStopCode=36232457&subBusStop=Display+Departures')

clean = page.text.replace("&nbsp","")
tree = html.fromstring(clean);

times = tree.xpath('//td[@class="time"]//a/text()')
print(times)

