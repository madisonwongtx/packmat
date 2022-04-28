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
		if 'passcode' not in request['form']:
			return json.dumps({'message': 'Bad Request. Must specify passcode'})
		try:
			user_id = request['form']['user_id']
			if (not check_user_id(user_id)):
				return json.dumps({'message': 'You are not logged in'})

			passcode = request['form']['passcode'] #stays as a string
			int(passcode) #check for correct passcode type (must be numerals only)

			return json.dumps(check_passcode(user_id, passcode))

		except Exception as e:
			return json.dumps({'message': str(e)})
			
	else:
		return json.dumps({'message': 'Request method not supported.'})

def check_passcode(user_id, passcode):
	latest = get_latest_state(user_id)

	if passcode == latest['passcode']:
		post_state(user_id, 0, passcode, 0)
	elif latest['is_active'] == 1:
		post_state(user_id, 1, latest['passcode'], 1)

	latest = get_latest_state(user_id)
	return {'alarm_status': latest['alarm_status'], 'is_active': latest['is_active'], 'message': 'successfully checked passcode'}

	