#!/usr/bin/python3
#
import mysql.connector
import os
import sys
import time
from prettytable import from_db_cursor


delay = 5


if __name__ == '__main__':

    # connect to MySQL (also check for failures)
    try:
        cnx = mysql.connector.connect(user=os.environ['SVP_USER'],
                                      password=os.environ['SVP_PASS'],
                                      database='VanityFlare')
        cursor = cnx.cursor()

        done_query = ("SELECT @pool_size AS Total, COUNT(*) AS Found, "
                      "COUNT(*)/@pool_size*100 AS `Done`"
                      "FROM keywords WHERE pool > 0")

        stat_query = ("SELECT len, COUNT(*) AS Total, SUM(pool>0) AS Found, "
                      "COUNT(*) - SUM(pool>0) AS Missing, "
                      "100*SUM(pool>0)/COUNT(*) AS Done FROM keywords GROUP BY len")

        cursor.execute('SELECT COUNT(*) FROM keywords INTO @pool_size')

        old_Found = None
        while True:

            print(chr(27)+'[2j')
            print('\033c')
            print('\x1bc')

            cursor.execute(done_query)
            for (Total, Found, Done) in cursor:
                Delta = Found - old_Found if old_Found is not None else 0
                Eta = (Total - Found)/Delta*delay/3600/24 if Delta > 0 else -1
                vals = Total, Found, Delta, Done, Eta
                print('Total: {} Found: {} Delta: {} Done: {:.3f}%, ETA: {:.2f} days'.format(*vals))
                old_Found = Found

            cursor.execute(stat_query)
            x = from_db_cursor(cursor)
            print(x)

            time.sleep(delay)

    # exit nicely if interrupted
    except mysql.connector.Error as err:
        if err.errno == mysql.connector.errorcode.ER_ACCESS_DENIED_ERROR:
            print("Something is wrong with your user name or password")
        elif err.errno == mysql.connector.errorcode.ER_BAD_DB_ERROR:
            print("Database does not exist")
        else:
            print(err)

    except KeyboardInterrupt:
        cursor.close()
        cnx.close()
        print("\nBye.")
        sys.exit(0)
