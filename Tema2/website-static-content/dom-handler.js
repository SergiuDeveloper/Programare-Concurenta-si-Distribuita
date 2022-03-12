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
        } else {
            alert("Failed to create session!")
        }
    }
    createSessionRequest.send();
}