#!/usr/bin/python3
#
import mysql.connector
import json
import os
import sys


max_pool = 16
already_in = dict()
autocommit = 1000


if __name__ == '__main__':

    # connect to MySQL (also check for failures)
    try:
        cnx = mysql.connector.connect(user=os.environ['SVP_USER'],
                                      password=os.environ['SVP_PASS'],
                                      database='VanityFlare')
        cursor = cnx.cursor()

        decoder = json.JSONDecoder()

        ins_query = ("INSERT IGNORE INTO SVP "
                     "(len, suffix, xiffus, address, seed) "
                     "VALUES (%s, %s, %s, %s, %s)")
        upd_query = 'UPDATE keywords SET pool = %s WHERE keyword = %s'

        # get list of non-full keyword pools
        cursor.execute('SELECT keyword, pool FROM keywords')
        for (keyword, pool) in cursor:
            already_in[keyword] = int(pool)

        # process while generation is running
        i = 0
        while True:

            # read line from pipe; it should contain JSON data
            line = sys.stdin.readline()

            # we did not get anything
            if line == '':
                continue

            # let's see what we got
            data = decoder.decode(line)
            l = data['len']
            suff = data['suffix']
            xiff = suff[::-1]
            addr = data['address']
            seed = data['secret']

            # ok, we got something, check the pool if needed
            if already_in[suff] >= max_pool:
                continue

            # it is needed, insert into the database
            cursor.execute(ins_query, (l, suff, xiff, addr, seed))
            cursor.execute(upd_query, (already_in[suff] + 1, suff))
            already_in[suff] = already_in[suff] + 1
            i = i + 1

            if i == 1000:
                cnx.commit()
                i = 0

    # exit nicely if interrupted
    except mysql.connector.Error as err:
        if err.errno == mysql.connector.errorcode.ER_ACCESS_DENIED_ERROR:
            print("Something is wrong with your user name or password")
        elif err.errno == mysql.connector.errorcode.ER_BAD_DB_ERROR:
            print("Database does not exist")
        else:
            print(err)


    except KeyboardInterrupt:
        cnx.commit()
        cursor.close()
        cnx.close()
        print("\nBye.")
        sys.exit(0)
