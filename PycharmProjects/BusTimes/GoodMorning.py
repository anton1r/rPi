__author__ = 'tony.rafferty'
import BusTimesCore
import BBCNews
import datetime

def goodMorning(self, lastDate):

    # check if it's alarm time

    today7am = datetime.datetime.now().replace(hour=7, minute=0)
    isItMorningTime = False

    if datetime.datetime.now().time() == today7am :
        isItMorningTime = True

    if isItMorningTime :
        lastDate = str(datetime.datetime.now().time());


    newsToRead = BBCNews.getNews().extend
    busTimes = BusTimesCore.getBusTimes()

    # notifications read to user as provided
    def morningAlarm():
        return "Good Morning" + newsToRead + busTimes

    # put notifications in queue
    self.q.put(morningAlarm())

    # return timestamp of most recent email
    return lastDate
