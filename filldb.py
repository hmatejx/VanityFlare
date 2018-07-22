#!/usr/bin/python3
#
from pymongo import MongoClient
from pymongo.errors import ConnectionFailure, OperationFailure
from collections import OrderedDict
import json
import os
import sys

if __name__ == '__main__':

    # connect to MongoDB (also check for failures)
    cred = os.getenv("MONGO_PASS")
    client = MongoClient('mongodb://' + ('' if cred is None else cred + '@') + 'localhost:27017/',
                         document_class=OrderedDict)
    try:
        client.admin.command('ismaster')
    except ConnectionFailure:
        print("MongoDB server not available!")
        sys.exit(-1)
    collection = client.mydb.StellarVanityPool
    try:
        collection.find_one({})
    except OperationFailure:
        print("Wrong MongoDB credentials provided!")
        sys.exit(-1)

    decoder = json.JSONDecoder(object_pairs_hook=OrderedDict)

    # process while generation is running
    while True:

        # store the data in the Mongo DB
        try:
            line = sys.stdin.readline()
            if line == '': break
            data = decoder.decode(line)
            collection.insert(data)

        # exit nicely if interrupted
        except KeyboardInterrupt:
            client.close()
            print('\nBye.')
            sys.exit(0)