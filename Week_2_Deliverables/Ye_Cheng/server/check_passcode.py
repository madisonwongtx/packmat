import requests
import json
import sys
sys.path.append('/var/jail/home/yechengz/packmat')
from database import create_table, post_state, get_latest_state

user = 'yechengz'

def request_handler(request):
	create_table()
	if request['method'] == 'POST':
		if 'passcode' not in request['form']:
			return json.dumps({'message': 'Bad Request. Must specify passcode'})
		try:
			passcode = request['form']['passcode'] #stays as a string
			int(passcode) #check for correct passcode type (must be numerals only)

			return json.dumps(check_passcode(user, passcode))
		except Exception as e:
			return json.dumps({'message': str(e)})
	else:
		return json.dumps({'message': 'Request method not supported.'})

def check_passcode(user, passcode):
	latest = get_latest_state(user)

	if passcode == latest['passcode']:
		post_state(user, 0, passcode, 0)

	latest = get_latest_state(user)
	return {'alarm_status': latest['alarm_status'], 'is_active': latest['is_active'], 'message': 'successfully checked passcode'}

	