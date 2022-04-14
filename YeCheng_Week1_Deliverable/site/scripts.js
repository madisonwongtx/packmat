var interval = 1000;
function getStatus() {
    $.ajax({
        type: "GET",
        url: "https://608dev-2.net/sandbox/sc/yechengz/packmat/request_handler.py",
        data: $(this).serialize(),
        dataType: "json",
        success: function (data) {
            if (data['is_active'] === 0) {
                $('#passcode').text('System not active');
            } else {
                $('#passcode').text('Passcode: ' + data['passcode']);
            }
            if (data['alarm_status'] === 0) {
                $('#alarm_status').text('Alarm status: off');
            } else {
                $('#alarm_status').text('Alarm status: on');
            }
        },
        complete: function (data) {
            setTimeout(getStatus, interval);
        }
    });
}
setTimeout(getStatus, interval);