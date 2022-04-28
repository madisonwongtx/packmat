import requests
import json
import sys
sys.path.append('/var/jail/home/yechengz/packmat2')
from database import create_table, create_accounts_table, login

def request_handler(request):
	create_table()
	create_accounts_table()
	if request['method'] == 'POST':
		if 'username' not in request['form'] or 'password' not in request['form']:
			return json.dumps({'message': 'You must specify a username and password'})
		try:
			username = request['form']['username']
			password = request['form']['password']

			return json.dumps(login(username, password))

		except Exception as e:
			return json.dumps({'message': str(e)})
				
	else:
		return json.dumps({'message': 'Request method not supported.'})
