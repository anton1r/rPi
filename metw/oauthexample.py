from twitter import *
import os

MY_TWITTER_CREDS = os.path.expanduser('~/.my_app_credentials')
MY_TWITTER_CREDS2 = os.path.expanduser('~/.my_app_credentials2')

CONSUMER_KEY = "4q99OGiOKxdyyQ3QliP1g"
CONSUMER_SECRET = "E7HvqhRqXptUiCMbML1LUh0F9HS6kVj48cq2Hb9Bwc"

if not os.path.exists(MY_TWITTER_CREDS):
   oauth_dance("metw", CONSUMER_KEY, CONSUMER_SECRET,
                MY_TWITTER_CREDS)

CONSUMER_KEY, CONSUMER_SECRET = read_token_file(MY_TWITTER_CREDS2)
oauth_token, oauth_secret = read_token_file(MY_TWITTER_CREDS)

twitter = Twitter(auth=OAuth(
    oauth_token, oauth_secret, CONSUMER_KEY, CONSUMER_SECRET))

# Now work with Twitter
twitter.statuses.update(status="Hello world!")
