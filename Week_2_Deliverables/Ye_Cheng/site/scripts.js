var statusInterval = 1000;
var alarmInterval = 3000;
setTimeout(getStatus(), statusInterval);
setTimeout(getAlarmTimes(), alarmInterval);

$('#passcode-form').submit(function(event) { 
    checkPasscode();
    event.preventDefault();
});

function getStatus() {
    $.ajax({
        type: "GET",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat/request_handler.py",
        dataType: "json",
        success: function (data) {
            if (data['is_active'] === 0) {
                $('#passcode').text('System not active');
                $('#passcode-form').hide();
            } else {
                $('#passcode').text('Passcode: ' + data['passcode']);
                $('#passcode-form').show();
            }
            if (data['alarm_status'] === 0) {
                $('#alarm_status').text('Alarm status: off');
            } else {
                $('#alarm_status').text('Alarm status: on');
            }
        },
        complete: function (data) {
            setTimeout(getStatus, statusInterval);
        }
    });
}

function getAlarmTimes() {
    $.ajax({
        type: "GET",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat/alarms.py",
        dataType: "json",
        success: function (data) {
            $('#alarms-list').empty();
            for (let i = 0; i < data['times'].length; i++) {
                let dateString = Date.parse(data['times'][i] + ' UTC');
                let date = new Date(dateString);
                let time = date.toLocaleString();
                $('#alarms-list').append('<li>' + time +'</li>');
            }
        },
        complete: function (data) {
            setTimeout(getAlarmTimes, alarmInterval);
        }
    });
}

function checkPasscode() {
    $.ajax({
        type: "POST",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat/check_passcode.py",
        data: "passcode=" + $('#passcode-input').val(),
        contentType: 'application/x-www-form-urlencoded',
        dataType: "json",
        success: function (data) {
            if (data['is_active'] === 1) {
                $('#message').text('Passcode did not match. Please try again');
            } else {
                $('#message').text('System deactivated.');
            }
        },
    });
}

