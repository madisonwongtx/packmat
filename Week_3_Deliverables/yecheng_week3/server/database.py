import sqlite3
import datetime
import random
packmat = '/var/jail/home/yechengz/packmat2/packmat.db'
USER_ID_LENGTH = 30
alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

def create_table():
	conn = sqlite3.connect(packmat)
	c = conn.cursor()
	c.execute('''CREATE TABLE IF NOT EXISTS state_db (rec_time timestamp, user_id text, is_active BIT, passcode text, alarm_status BIT);''')
	conn.commit()
	conn.close()

def post_state(user_id, is_active, passcode, alarm_status):
	conn = sqlite3.connect(packmat)
	c = conn.cursor()
	c.execute('''INSERT into state_db VALUES (?, ?, ?, ?, ?);''', (datetime.datetime.now(), user_id, is_active, passcode, alarm_status))
	conn.commit()
	conn.close()

def get_latest_state(user_id):
	conn = sqlite3.connect(packmat)
	c = conn.cursor()
	row = c.execute('''SELECT * FROM state_db WHERE user_id = ? ORDER BY rec_time DESC;''', (user_id,)).fetchone()
	if row == None:
		conn.commit()
		conn.close()
		return {'message': 'user not found'}
	is_active = row[2]
	passcode = row[3]
	alarm_status = row[4]
	conn.commit()
	conn.close()
	result = {'is_active': is_active, 'passcode': passcode, 'alarm_status': alarm_status, 'message': 'success'}
	return result

def get_alarm_list(user_id):
	conn = sqlite3.connect(packmat)
	c = conn.cursor()
	rows = c.execute('''SELECT * FROM state_db WHERE user_id = ? AND alarm_status = 1 AND is_active = 1 ORDER BY rec_time DESC;''', (user_id,)).fetchall()
	result = []
	for r in rows:
		result.append(r[0])
	conn.commit()
	conn.close()
	return {'times': result, 'message': 'successfully got times'}

#ACCOUNTS

def create_accounts_table():
	conn = sqlite3.connect(packmat)
	c = conn.cursor()
	c.execute('''CREATE TABLE IF NOT EXISTS accounts_db (username text, password text, user_id text);''')
	conn.commit()
	conn.close()

def create_account(username, password):
	conn = sqlite3.connect(packmat)
	c = conn.cursor()

	row = c.execute('''SELECT * FROM accounts_db WHERE username = ?;''', (username,)).fetchone()
	if row != None:
		conn.commit()
		conn.close()
		return {'message': 'user already exists'}

	user_id = ''
	for i in range(USER_ID_LENGTH):
		user_id += alphabet[random.randint(0, 25)]

	c.execute('''INSERT into accounts_db VALUES (?, ?, ?);''', (username, password, user_id))
	conn.commit()
	conn.close()
	return {'message': 'success'}

def login(username, password):
	conn = sqlite3.connect(packmat)
	c = conn.cursor()

	row = c.execute('''SELECT * FROM accounts_db WHERE username = ?;''', (username,)).fetchone()
	conn.commit()
	conn.close()

	if row == None:
		return {'message': 'incorrect password or username'}

	if (password != row[1]):
		return {'message': 'incorrect password or username'}
	else:
		return {'user_id': row[2], 'username': username, 'message': 'success'}

def check_user_id(user_id):
	conn = sqlite3.connect(packmat)
	c = conn.cursor()

	row = c.execute('''SELECT * FROM accounts_db WHERE user_id = ?;''', (user_id,)).fetchone()
	conn.commit()
	conn.close()

	if row == None:
		return False
	else:
		return True