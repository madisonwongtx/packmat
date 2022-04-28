var statusInterval = 1000;
var alarmInterval = 3000;

var loggedIn = false;
var user_id = '';
var username = '';
if (Cookies.get('user_id') !== undefined && Cookies.get('username') !== undefined) {
    loggedIn = true;
    user_id = Cookies.get('user_id');
    username = Cookies.get('username');
    setTimeout(getStatus(), statusInterval);
    setTimeout(getAlarmTimes(), alarmInterval);
    $('#login-form').hide();
    $('#logout').show();
    $('#passcode').show();
    $('#alarm-status').show();
    $('#alarm-history').show();
    $('#greeting').show();
    $('#greeting').text('Hello '+ username);
}

if (!loggedIn) {
    $('#login-form').show();
}

//EVENTS
$('#passcode-form').submit(function(event) { 
    if (loggedIn) {
        checkPasscode();
    }
    $(':input','#passcode-form').not(':button, :submit, :reset, :hidden').val('')
    event.preventDefault();
});

$('#login-form').submit(function(event) { 
    if (!loggedIn) {
        logIn();
    }
    $(':input','#login-form').not(':button, :submit, :reset, :hidden').val('')
    event.preventDefault();
});

$( "#logout" ).click(function() {
    loggedIn = false;
    Cookies.remove('user_id');
    Cookies.remove('username');
    user_id = '';
    username = '';
    $('#login-form').show();
    $('#logout').hide();
    $('#passcode').hide();
    $('#passcode-form').hide();
    $('#alarm-status').hide();
    $('#alarm-history').hide();
    $('#greeting').text('');
    $('#greeting').hide();
  });

//HTTP REQUESTS
function getStatus() {
    $.ajax({
        type: "GET",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat2/request_handler.py?user_id=" + user_id,
        dataType: "json",
        success: function (data) {
            if (loggedIn) {
                if (data['is_active'] === 0) {
                    $('#passcode').text('System not active');
                    $('#passcode-form').hide();
                } else {
                    $('#passcode').text('Passcode: ' + data['passcode']);
                    $('#passcode-form').show();
                }
                if (data['alarm_status'] === 0) {
                    $('#alarm-status').text('Alarm status: off');
                } else {
                    $('#alarm-status').text('Alarm status: on');
                }
            }
        },
        complete: function (data) {
            if (loggedIn) {
                setTimeout(getStatus, statusInterval);
            }
        }
    });
}

function getAlarmTimes() {
    $.ajax({
        type: "GET",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat2/alarms.py?user_id=" + user_id,
        dataType: "json",
        success: function (data) {
            if (loggedIn) {
                $('#alarms-list').empty();
                for (let i = 0; i < data['times'].length; i++) {
                    let dateString = Date.parse(data['times'][i] + ' UTC');
                    let date = new Date(dateString);
                    let time = date.toLocaleString();
                    $('#alarms-list').append('<li>' + time +'</li>');
                }
            }
        },
        complete: function (data) {
            if (loggedIn) {
                setTimeout(getAlarmTimes, alarmInterval);
            }
        }
    });
}

function checkPasscode() {
    $.ajax({
        type: "POST",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat2/check_passcode.py",
        data: "passcode=" + $('#passcode-input').val() +"&user_id=" + user_id,
        contentType: 'application/x-www-form-urlencoded',
        dataType: "json",
        success: function (data) {
            if (loggedIn) {
                if (data['is_active'] === 1) {
                    $('#message').text('Passcode did not match. Please try again');
                } else {
                    $('#message').text('System deactivated.');
                }
            }
        },
    });
}

function logIn() {
    $.ajax({
        type: "POST",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat2/login.py",
        data: "username=" + $('#username').val() +"&password=" + $('#password').val(),
        contentType: 'application/x-www-form-urlencoded',
        dataType: "json",
        success: function (data) {
            if (data['message'] !== 'success') {
                $('#login-message').text('Incorrect username or password');
            } else {
                loggedIn = true;
                Cookies.set('user_id', data['user_id'], { expires: 1 });
                Cookies.set('username', data['username'], { expires: 1 });
                user_id = data['user_id'];
                username = data['username'];
                setTimeout(getStatus(), statusInterval);
                setTimeout(getAlarmTimes(), alarmInterval);
                $('#login-form').hide();
                $('#logout').show();
                $('#passcode').show();
                $('#alarm-status').show();
                $('#alarm-history').show();
                $('#login-message').text('');
                $('#greeting').show();
                $('#greeting').text('Hello '+ username);
            }
        },
    });
}