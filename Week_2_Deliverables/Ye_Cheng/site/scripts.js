var interval = 1000;
function getStatus() {
    $.ajax({
        type: "GET",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat/request_handler.py",
        dataType: "json",
        success: function (data) {
            if (data['is_active'] === 0) {
                $('#passcode').text('System not active');
            } else {
                $('#passcode').text('Passcode: ' + data['passcode']);
            }
            if (data['alarm_status'] === 0) {
                $('#alarm_status').text('Alarm status: off');
                $('#passcode-form').hide();
            } else {
                $('#alarm_status').text('Alarm status: on');
                $('#passcode-form').show();
            }
        },
        complete: function (data) {
            setTimeout(getStatus, interval);
        }
    });
}
setTimeout(getStatus(), interval);

function checkPasscode() {
    $.ajax({
        type: "POST",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat/check_passcode.py",
        data: "passcode=" + $('#passcode-input').val(),
        contentType: 'application/x-www-form-urlencoded',
        dataType: "json",
        success: function (data) {
            if (data['alarm_status'] === 1) {
                $('#message').text('Passcode did not match. Please try again');
                $('#passcode-form').show();
            } else {
                $('#message').text('Alarm stopped');
                $('#passcode-form').hide();
            }
        },
    });
}

$('#passcode-form').submit(function(event) { 
    checkPasscode();
    event.preventDefault();
});