document.getElementById("joinSession").onclick = function() {
    const sessionIdInput = document.getElementById("sessionIdInput");
    const sessionId = sessionIdInput.value.trim();
    sessionIdInput.value = sessionId;

    if (sessionId.length == 0) {
        sessionIdInput.value = "";
        alert("Invalid session ID format!");
        return;
    }

    const sessionSelector = document.getElementById("sessionSelector");
    sessionSelector.style.display = "none";

    window.setInterval(function() {
        get_frames_data()
    }, 1000);
}

document.getElementById("createSession").onclick = function() {
    const createSessionRequest = new XMLHttpRequest();
    createSessionRequest.open("POST", "https://gy37j7ly20.execute-api.eu-west-1.amazonaws.com/create-session", true);
    createSessionRequest.onreadystatechange = function() {
        if (createSessionRequest.readyState != 4) {
            return;
        }
        if (createSessionRequest.status == 200) {
            const response = JSON.parse(createSessionRequest.responseText);
            const sessionId = response["sessionId"];

            const sessionIdSpan = document.getElementById("sessionId");
            const sessionContent = document.getElementById("sessionContent");

            sessionIdSpan.innerHTML = sessionId;
            sessionContent.style.display = "block";

            const sessionSelector = document.getElementById("sessionSelector");
            sessionSelector.style.display = "none";

            window.setInterval(function() {
                get_frames_data(sessionId)
            }, 1000);
        } else {
            alert("Failed to create session!")
        }
    }
    createSessionRequest.send();
}

document.getElementById("alertCheckbox").onclick = function() {

}

function get_frames_data(sessionId) {
    const getCamerasRequest = new XMLHttpRequest();
    getCamerasRequest.open("GET", "https://gy37j7ly20.execute-api.eu-west-1.amazonaws.com/get-cameras", true);
    getCamerasRequest.onreadystatechange = function() {
        if (getCamerasRequest.readyState != 4) {
            return;
        }
        if (getCamerasRequest.status == 200) {
            const response = JSON.parse(getCamerasRequest.responseText);
            const cameraIds = response["cameraIds"];

            cameraIds.forEach(cameraId => {
                const socket = new WebSocket("ws://ec2-3-251-85-101.eu-west-1.compute.amazonaws.com:8001");

                socket.addEventListener("open", function(event) {
                    socket.send(sessionId.concat(cameraId));
                });
                socket.addEventListener("message", function(event) {
                    const frameData = event.data;

                    socket.close();
                });
            });
        } else {
            alert("Failed to get cameras");
        }
    }
    getCamerasRequest.send(JSON.stringify({
        'sessionId': sessionId
    }));
}