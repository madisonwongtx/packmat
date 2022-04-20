import requests
import json
import sys
sys.path.append('/var/jail/home/yechengz/packmat')
from database import create_table, get_alarm_list

user = 'yechengz'

def request_handler(request):
	create_table()
	if request['method'] == 'GET':
		try:
			return json.dumps(get_alarm_list(user))
		except Exception as e:
			return json.dumps({'message': str(e)})
	else:
		return json.dumps({'message': 'Request method not supported.'})

