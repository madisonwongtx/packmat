import requests
import sqlite3
import datetime
import json
packmat = '/var/jail/home/yechengz/packmat/packmat.db'
user = 'yechengz'

def request_handler(request):
	create_table()
	if request['method'] == 'POST':
		if 'passcode' not in request['form'] or 'alarm_status' not in request['form'] or 'is_active' not in request['form']:
			return 'Bad Request. Must specify passcode, alarm_status, and is_active'
		try:
			passcode = request['form']['passcode'] #stays as a string
			int(passcode) #check for correct passcode type (must be numerals only)

			is_active = int(request['form']['is_active'])
			if (is_active != 0 and is_active != 1):
				return 'is_active must be 0 or 1'

			alarm_status = int(request['form']['alarm_status'])
			if (alarm_status != 0 and alarm_status != 1):
				return 'alarm_status must be 0 or 1'

			post_state(user, is_active, passcode, alarm_status)

			return 'Successfully posted'
		except Exception as e:
			return e
	if request['method'] == 'GET':
		try:
			return get_latest_state(user)
		except Exception as e:
			return e
	else:
		return 'Request method not supported.'

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
		return {'message': 'user not found'}
	is_active = row[2]
	passcode = row[3]
	alarm_status = row[4]
	conn.commit()
	conn.close()
	result = {'is_active': is_active, 'passcode': passcode, 'alarm_status': alarm_status, 'message': 'success'}
	return json.dumps(result)