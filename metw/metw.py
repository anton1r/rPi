from twitter import *
import os

mewt_creds = os.path.expanduser('~/.my_app_credentials')
mewt_creds2 = os.path.expanduser('~/.my_app_credentials2')

oauth_token, oauth_secret = read_token_file(mewt_creds)
consumer_key, consumer_secret = read_token_file(mewt_creds2)

twitter = Twitter(auth=OAuth(
    oauth_token, oauth_secret, consumer_key, consumer_secret))

# Now work with Twitter
twitter.statuses.update(status="Hello world!")