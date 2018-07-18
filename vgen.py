#!/usr/bin/python3
from stellar_base.keypair import Keypair
from pybloomfilter import BloomFilter
from pymongo import MongoClient
import csv
import time
import sys


nBench = 1000

# global variables (list of keywords and associated Bloom filter)
bf = BloomFilter(1e6, 1e-4)
keywords = []
maxlen = 0


# load the list of interesting keywords (suffixes, prefixes)
def load_keywords(filename):

    global keywords
    global maxlen

    ifile = open(filename, 'rU')
    reader = csv.reader(ifile, delimiter=',')

    for row in reader:
        keyword = row[0].encode('utf-8')
        bf.add(keyword)
        keywords.append(keyword)

    ifile.close()

    maxlen = len(keywords[0])

    return None


# find first matching suffix
def first_match(address):

    # check if we get a hit in the Bloom filter (longest to shortest)
    for i in range(maxlen, -1, -1):
        # we have a potential hit...
        if address[-i:] in bf:
            if address[-i:] in keywords:
                return address[-i:]

    # else return none
    return None


# simple exponential average
class exp_avg():
    r0 = None
    def smooth(self, r, alpha = 0.15):
        self.r0 = r if self.r0 == None else  alpha * r + (1 - alpha) * self.r0
        return self.r0


# main function
if __name__ == '__main__':

    # load interesting keywords
    load_keywords('names/NAMES.txt')

    # connect to MongoDB
    client = MongoClient('mongodb://localhost:27017/')
    db = client.mydb
    col = db.StellarVanityPool
    print('Existing number of vanity addresses: {}'.format(col.count('{}')))

    print('Generating new ones...')
    # generate!
    i = hits = oldhits = 0
    e1 = exp_avg()
    e2 = exp_avg()
    oldTime = time.perf_counter()
    while True:

        try:

            kp = Keypair.random()
            address = kp.address()

            hit = first_match(address)
            if hit is not None:
                hits = hits + 1
                col.insert_one({"suffix": hit.decode(), "public": address.decode(), "secret": kp.seed().decode()})

            i = i + 1
            if i % nBench == 0:
                newTime = time.perf_counter()
                dt = newTime - oldTime
                print('Generation rate:', round(e1.smooth(1.0 * nBench / dt), 1), 'addr/sec\t',
                      'Hit rate: ',  round(e2.smooth((hits - oldhits)/dt), 1), 'addr/sec')
                oldTime = newTime
                oldhits = hits

        except KeyboardInterrupt:

            client.close()
            print('\nBye.')
            sys.exit()
