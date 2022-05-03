var statusInterval = 1000;
var alarmInterval = 3000;

var showPasscodeForm = false;
var loggedIn = false;
var user_id = '';
var username = '';
if (Cookies.get('user_id') !== undefined && Cookies.get('username') !== undefined) {
    loggedIn = true;
    user_id = Cookies.get('user_id');
    username = Cookies.get('username');
    setTimeout(getStatus(), statusInterval);
    setTimeout(getAlarmTimes(), alarmInterval);
    $('#login-page').hide();
    $('#main-page').show();
    $('#greeting').text('Hello '+ username + '!');
}

if (!loggedIn) {
    $('#login-page').show();
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

$('#deactivate').click(function(event) { 
    if (!showPasscodeForm) {
        showPasscodeForm = true;
        $('#passcode-form').show();
        $('#deactivate').text("Hide Form");
    } else {
        showPasscodeForm = false;
        $('#passcode-form').hide();
        $('#deactivate').text("Deactivate System");
    }
});

$( "#logout" ).click(function() {
    loggedIn = false;
    Cookies.remove('user_id');
    Cookies.remove('username');
    user_id = '';
    username = '';
    $('#login-page').show();
    $('#main-page').hide();
    $('#greeting').text('');
  });

//HTTP REQUESTS
function getStatus() {
    $.ajax({
        type: "GET",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat2/request_handler.py?user_id=" + user_id,
        dataType: "json",
        success: function (data) {
            if (loggedIn && data['message'] !== 'user not found') {
                if (data['is_active'] === 0) {
                    $('#active').text(' not active');
                    $('#passcode').text(' ' + data['passcode']);
                    $('#deactivate').hide();
                    $('#passcode-form').hide();
                    $('#passcode-message').text('');
                    $('#deactivate').text("Deactivate System");
                    showPasscodeForm = false;
                } else {
                    $('#active').text(' active');
                    $('#passcode').text(' ' + data['passcode']);
                    $('#deactivate').show();
                }
                if (data['alarm_status'] === 0) {
                    $('#alarm-status').text(' off');
                } else {
                    $('#alarm-status').text(' on');
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
                if (data['is_active'] === 1 || data['message'] !== "successfully checked passcode") {
                    $('#passcode-message').text('Passcode did not match. Please try again');
                } else {
                    $('#passcode-message').text('System deactivated.');
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
                $('#login-page').hide();
                $('#main-page').show();
                $('#login-message').text('');
                $('#greeting').text('Hello '+ username + '! Welcome to Packmat!');
            }
        },
    });
}