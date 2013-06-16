from twitter import *
import os

mewt_creds = os.path.expanduser('~/.my_app_credentials')
mewt_creds2 = os.path.expanduser('~/.my_app_credentials2')

oauth_token, oauth_secret = read_token_file(mewt_creds)
consumer_key, consumer_secret = read_token_file(mewt_creds2)

twitter = Twitter(auth=OAuth(
    oauth_token, oauth_secret, consumer_key, consumer_secret))

tweets = twitter.statuses.user_timeline()

print tweets[0]['text']
print tweets[0]['id']

# Now work with Twitter
#twitter.statuses.update(status="Hello world!")