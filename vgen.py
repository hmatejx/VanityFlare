#!/usr/bin/python3
from stellar_base.keypair import Keypair
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure, OperationFailure
from collections import OrderedDict
import csv
import time
import os
import sys


# how frequently to output performance statistics
NBENCH = 1000


# global variables
keywords = set()
maxlen = 0


# load the list of interesting keywords (suffixes, prefixes)
def load_keywords(filename):

    global maxlen

    iFile = open(filename, 'rU')
    reader = csv.reader(iFile, delimiter=',')

    for row in reader:
        if len(row[0]) > 3 and len(row[0]) <= 10:
            keyword = row[0].encode('utf-8')
            keywords.add(keyword)

    maxlen = max([len(k) for k in keywords])
    iFile.close()

    return None


# find longest matching suffix using a Bloom filter
def first_match(address):

    for i in range(maxlen, -1, -1):
        if address[-i:] in keywords:
            return address[-i:]

    # else return none
    return None


# simple exponential average
class exp_avg():
    r0 = None
    def smooth(self, r, prec = 1, alpha = 0.05):
        self.r0 = r if self.r0 == None else  alpha * r + (1 - alpha) * self.r0
        return round(self.r0, prec)


# main function
if __name__ == '__main__':

    # load interesting keywords
    load_keywords('names/NAMES.txt')
    print('Loaded {} keywords.'.format(len(keywords)))

    # connect to MongoDB (also check for failures)
    cred = os.getenv("MONGO_PASS")
    client = MongoClient('mongodb://' + ('' if cred is None else cred + '@') + 'localhost:27017/',
                         document_class=OrderedDict)
    try:
        client.admin.command('ismaster')
    except ConnectionFailure:
        print("MongoDB server not available!")
        sys.exit(-1)
    col = client.mydb.StellarVanityPool
    try:
        col.find_one({})
    except OperationFailure:
        print("Wrong MongoDB credentials provided!")
        sys.exit(-1)

    # generate!
    print('Existing number of vanity addresses: {}'.format(col.count('{}')))
    print('Generating new ones...')
    i = hits = oldhits = 0
    eavg = { "total": exp_avg(), "hit": exp_avg() }
    oldTime = time.perf_counter()

    while True:

        try:

            # generate and check for hit
            kp = Keypair.random()
            address = kp.address()
            hit = first_match(address)

            # insert hit into database
            if hit is not None:
                hits = hits + 1
                col.insert_one(OrderedDict([("suffix", hit.decode()),
                                            ("public", address.decode()),
                                            ("secret", kp.seed().decode())]))

            # display performance stats
            i = i + 1
            if i % NBENCH == 0:
                newTime = time.perf_counter()
                dt = newTime - oldTime
                print('Generation rate:', eavg["total"].smooth(1.0 * NBENCH / dt), 'addr/sec\t',
                      'Hit rate: ',  eavg["hit"].smooth((hits - oldhits) / dt), 'addr/sec')
                oldTime = newTime
                oldhits = hits

        # exit nicely if interrupted
        except KeyboardInterrupt:

            client.close()
            print('\nBye.')
            sys.exit(0)
