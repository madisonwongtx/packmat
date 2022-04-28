import requests
import json
import sys
sys.path.append('/var/jail/home/yechengz/packmat2')
from database import create_table, get_alarm_list, create_accounts_table, check_user_id

user_id = 'yechengz'

def request_handler(request):
	create_table()
	create_accounts_table()
	if request['method'] == 'GET':
		if 'user_id' not in request['args']:
			return json.dumps({'message': 'You are not logged in'})

		try:
			user_id = request['values']['user_id']
			if (not check_user_id(user_id)):
				return json.dumps({'message': 'You are not logged in'})
			
			return json.dumps(get_alarm_list(user_id))

		except Exception as e:
			return json.dumps({'message': str(e)})

	else:
		return json.dumps({'message': 'Request method not supported.'})

