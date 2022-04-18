import sqlite3
import datetime
packmat = '/var/jail/home/yechengz/packmat/packmat.db'

def create_table():
	conn = sqlite3.connect(packmat)
	c = conn.cursor()
	c.execute('''CREATE TABLE IF NOT EXISTS state_db (rec_time timestamp, user text, is_active BIT, passcode text, alarm_status BIT);''')
	conn.commit()
	conn.close()

def post_state(user, is_active, passcode, alarm_status):
	conn = sqlite3.connect(packmat)
	c = conn.cursor()
	c.execute('''INSERT into state_db VALUES (?, ?, ?, ?, ?);''', (datetime.datetime.now(), user, is_active, passcode, alarm_status))
	conn.commit()
	conn.close()

def get_latest_state(user):
	conn = sqlite3.connect(packmat)
	c = conn.cursor()
	row = c.execute('''SELECT * FROM state_db WHERE user = ? ORDER BY rec_time DESC;''', (user,)).fetchone()
	if row == None:
		return json.dumps({'message': 'user not found'})
	is_active = row[2]
	passcode = row[3]
	alarm_status = row[4]
	conn.commit()
	conn.close()
	result = {'is_active': is_active, 'passcode': passcode, 'alarm_status': alarm_status, 'message': 'success'}
	return result