import requests
import json
import sys
sys.path.append('/var/jail/home/yechengz/packmat')
from database import create_table, post_state, get_latest_state

user = 'yechengz'

def request_handler(request):
	create_table()
	if request['method'] == 'POST':
		if 'passcode' not in request['form'] or 'alarm_status' not in request['form'] or 'is_active' not in request['form']:
			return json.dumps({'message': 'Bad Request. Must specify passcode, alarm_status, and is_active'})
		try:
			passcode = request['form']['passcode'] #stays as a string
			int(passcode) #check for correct passcode type (must be numerals only)

			is_active = int(request['form']['is_active'])
			if (is_active != 0 and is_active != 1):
				return json.dumps({'message': 'is_active must be 0 or 1'})

			alarm_status = int(request['form']['alarm_status'])
			if (alarm_status != 0 and alarm_status != 1):
				return json.dumps({'message': 'alarm_status must be 0 or 1'})

			post_state(user, is_active, passcode, alarm_status)

			return json.dumps({'message': 'success'})
		except Exception as e:
			return json.dumps({'message': str(e)})
	if request['method'] == 'GET':
		try:
			return json.dumps(get_latest_state(user))
		except Exception as e:
			return json.dumps({'message': str(e)})
	else:
		return json.dumps({'message': 'Request method not supported.'})