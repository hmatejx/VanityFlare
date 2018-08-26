#!/usr/bin/python3
#
import sys
import mysql.connector


if __name__ == '__main__':

    # connect to MySQL (also check for failures)
    try:
        cnx = mysql.connector.connect(user=os.environ['SVP_USER'],
                                      password=os.environ['SVP_PASS'],
                                      database='VanityFlare')
        cursor = cnx.cursor()

        query = ("INSERT IGNORE INTO keywords "
                 "(keyword, type, pool, len) "
                 "VALUES (%s, %s, %s, %s)")

        with open('NAMES.txt', 'r') as f:
            for line in f:

                key = line.rstrip()
                if len(key) < 5 or len(key) > 16:
                    continue

                # Insert new employee
                cursor.execute(query, (key, 'WORD', 0, len(key)))

        with open('WORDS.txt', 'r') as f:
            for line in f:

                key = line.rstrip()
                if len(key) < 5 or len(key) > 16:
                    continue

                # Insert new employee
                cursor.execute(query, (key, 'WORD', 0, len(key)))

    except mysql.connector.Error as err:
        if err.errno == mysql.connector.errorcode.ER_ACCESS_DENIED_ERROR:
            print("Something is wrong with your user name or password")
        elif err.errno == mysql.connector.errorcode.ER_BAD_DB_ERROR:
            print("Database does not exist")
        else:
            print(err)

    else:
        cnx.commit()
        cursor.close()
        cnx.close()

