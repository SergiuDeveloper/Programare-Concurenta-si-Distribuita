document.getElementById("joinSession").onclick = function() {
    const sessionIdInput = document.getElementById("sessionIdInput");
    const sessionId = sessionIdInput.value.trim();
    sessionIdInput.value = sessionId;

    if (sessionId.length == 0) {
        sessionIdInput.value = "";
        alert("Invalid session ID format!");
        return;
    }

    const getAlertRequest = new XMLHttpRequest();
    getAlertRequest.open("POST", "https://gy37j7ly20.execute-api.eu-west-1.amazonaws.com/get-session", true);
    getAlertRequest.onreadystatechange = function() {
        if (getAlertRequest.readyState != 4) {
            return;
        }
        if (getAlertRequest.status == 200) {
            const response = JSON.parse(getAlertRequest.responseText);
            const sessionExists = response["sessionExists"];

            if (!sessionExists) {
                alert("Session does not exist!");
                return;
            }

            const sessionSelector = document.getElementById("sessionSelector");
            sessionSelector.style.display = "none";

            setAlertCheckboxValue(sessionId);
        } else {
            alert("Failed to get session data");
        }
    }
    getAlertRequest.send(JSON.stringify({
        "sessionId": sessionId
    }));
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

            const sessionSelector = document.getElementById("sessionSelector");
            sessionSelector.style.display = "none";

            const alertVal = false;
            const alertEmailInput = document.getElementById("alertEmailInput");
            const email = alertEmailInput.value;

            const setAlertRequest = new XMLHttpRequest();
            setAlertRequest.open("POST", "https://gy37j7ly20.execute-api.eu-west-1.amazonaws.com/set-alert", true);
            setAlertRequest.onreadystatechange = function() {
                if (setAlertRequest.readyState != 4) {
                    return;
                }
                if (setAlertRequest.status == 200) {
                    setAlertCheckboxValue(sessionId);
                } else {
                    alert("Failed to set alert data");
                    return;
                }
            }
            setAlertRequest.send(JSON.stringify({
                "sessionId": sessionId,
                "alert": alertVal,
                "email": email
            }));
        } else {
            alert("Failed to create session!")
        }
    }
    createSessionRequest.send();
}

document.getElementById("alertCheckbox").onclick = function() {
    const sessionIdSpan = document.getElementById("sessionId");
    const alertCheckbox = document.getElementById("alertCheckbox");
    const alertEmail = document.getElementById("alertEmail");

    const sessionId = sessionIdSpan.innerHTML;
    const alertVal = alertCheckbox.checked;
    const email = alertEmail.innerHTML;

    const setAlertRequest = new XMLHttpRequest();
    setAlertRequest.open("POST", "https://gy37j7ly20.execute-api.eu-west-1.amazonaws.com/set-alert", true);
    setAlertRequest.onreadystatechange = function() {
        if (setAlertRequest.readyState != 4) {
            return;
        }
        if (setAlertRequest.status != 200) {
            alert("Failed to set alert data");
            return;
        }
    }
    setAlertRequest.send(JSON.stringify({
        "sessionId": sessionId,
        "alert": alertVal,
        "email": email
    }));
}

function setAlertCheckboxValue(sessionId) {
    const sessionIdSpan = document.getElementById("sessionId");
    const sessionContent = document.getElementById("sessionContent");

    const getAlertRequest = new XMLHttpRequest();
    getAlertRequest.open("POST", "https://gy37j7ly20.execute-api.eu-west-1.amazonaws.com/get-alert", true);
    getAlertRequest.onreadystatechange = function() {
        if (getAlertRequest.readyState != 4) {
            return;
        }
        if (getAlertRequest.status == 200) {
            const response = JSON.parse(getAlertRequest.responseText);
            const alert = response["alert"];
            const email = response["email"];

            const alertCheckbox = document.getElementById("alertCheckbox");
            const alertEmail = document.getElementById("alertEmail");

            alertCheckbox.checked = alert;
            alertEmail.innerHTML = email;

            sessionIdSpan.innerHTML = sessionId;
            sessionContent.style.display = "block";

            getFramesData(sessionId);
            window.setInterval(function() {
                getFramesData(sessionId)
            }, 5000);
        } else {
            alert("Failed to get alert data");
        }
    }
    getAlertRequest.send(JSON.stringify({
        "sessionId": sessionId
    }));
}

function getFramesData(sessionId) {
    const getCamerasRequest = new XMLHttpRequest();
    getCamerasRequest.open("POST", "https://gy37j7ly20.execute-api.eu-west-1.amazonaws.com/get-cameras", true);
    getCamerasRequest.onreadystatechange = function() {
        if (getCamerasRequest.readyState != 4) {
            return;
        }
        if (getCamerasRequest.status == 200) {
            const response = JSON.parse(getCamerasRequest.responseText);
            const cameraIds = response["cameraIds"];

            const cameraFrames = [...document.getElementsByClassName("cameraFrame")];
            const existingCameraIds = [];
            cameraFrames.forEach(cameraFrame => {
                existingCameraIds.push(cameraFrame);
            });

            const cameraContent = document.getElementById("cameraContent");

            cameraIds.forEach(cameraId => {
                if (existingCameraIds.includes(cameraId)) {
                    return;
                }
                cameraContent.innerHTML += "<div style=\"display: none\" id=\"cameraDiv_" + cameraId + "\"><strong>Camera " + cameraId + "</strong><br><img class=\"cameraFrame\" data-id=\"" + cameraId + "\" width=\"500\" id=\"camera_" + cameraId + "\"></div>";
            });

            cameraIds.forEach(cameraId => {
                const socket = new WebSocket("ws://ec2-3-251-85-101.eu-west-1.compute.amazonaws.com:8001");

                socket.onopen = function(e) {
                    socket.send(JSON.stringify({
                        "sessionId": sessionId,
                        "cameraId": cameraId
                    }));
                };

                socket.onmessage = function(e) {
                    const frameData = e.data;
                    if (frameData.length == 0) {
                        return;
                    }

                    const cameraElement = document.getElementById("camera_" + cameraId);
                    const cameraDivElement = document.getElementById("cameraDiv_" + cameraId);
                    cameraElement.src = "data:image/png;base64," + frameData;
                    if (cameraDivElement.style.display == "none") {
                        cameraDivElement.style.display = "block";
                    }

                    socket.close();
                };
            });
        } else {
            alert("Failed to get cameras");
        }
    }
    getCamerasRequest.send(JSON.stringify({
        "sessionId": sessionId
    }));
}