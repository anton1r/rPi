from lxml import html
import requests
import sys
import time
import re

WORDS = ["BUS"]

PRIORITY = 1

def handle(text, mic, profile):

    page = requests.get('http://www.mybustracker.co.uk/?module=mobile&mode=1&busStopCode=36232457&subBusStop=Display+Departures')

    clean = page.text.replace("&nbsp","")
    tree = html.fromstring(clean)

    times = tree.xpath('//td[@class="time"]//a/text()')

    if(times[0] == "DUE"):
        str = "The next bus is DUE"
    else :
        str =  "The next bus is in "+ times[0]+" minutes"

    mic.say(str + " and then in "+times[1]+" minutes")

def isValid(text):
    return bool(re.search(r'\Bus\b', text, re.IGNORECASE))