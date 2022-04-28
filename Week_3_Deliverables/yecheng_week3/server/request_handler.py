import requests
import json
import sys
sys.path.append('/var/jail/home/yechengz/packmat2')
from database import create_table, post_state, get_latest_state, create_accounts_table, check_user_id

def request_handler(request):
	create_table()
	create_accounts_table()
	if request['method'] == 'POST':
		if 'user_id' not in request['form']:
			return json.dumps({'message': 'You are not logged in'})

		if 'passcode' not in request['form'] or 'alarm_status' not in request['form'] or 'is_active' not in request['form']:
			return json.dumps({'message': 'Bad Request. Must specify passcode, alarm_status, and is_active'})

		try:
			user_id = request['form']['user_id']
			if (not check_user_id(user_id)):
				return json.dumps({'message': 'You are not logged in'})

			passcode = request['form']['passcode'] #stays as a string
			int(passcode) #check for correct passcode type (must be numerals only)

			is_active = int(request['form']['is_active'])
			if (is_active != 0 and is_active != 1):
				return json.dumps({'message': 'is_active must be 0 or 1'})

			alarm_status = int(request['form']['alarm_status'])
			if (alarm_status != 0 and alarm_status != 1):
				return json.dumps({'message': 'alarm_status must be 0 or 1'})

			post_state(user_id, is_active, passcode, alarm_status)

			return json.dumps({'message': 'success'})

		except Exception as e:
			return json.dumps({'message': str(e)})

	if request['method'] == 'GET':
		if 'user_id' not in request['args']:
			return json.dumps({'message': 'You are not logged in'})

		try:
			user_id = request['values']['user_id']
			if (not check_user_id(user_id)):
				return json.dumps({'message': 'You are not logged in'})
				
			return json.dumps(get_latest_state(user_id))

		except Exception as e:
			return json.dumps({'message': str(e)})

	else:
		return json.dumps({'message': 'Request method not supported.'})