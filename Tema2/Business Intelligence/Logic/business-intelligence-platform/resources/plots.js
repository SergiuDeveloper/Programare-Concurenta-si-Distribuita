window.onload = function(e) {
    data = * DATA * ;

    const sessionsPlot = document.getElementById('sessionsPlot');
    Plotly.newPlot(sessionsPlot, [{
        x: data['sessions']['date'],
        y: data['sessions']['traffic']
    }], {
        margin: { t: 1 }
    });

    const camerasPlot = document.getElementById('camerasPlot');
    Plotly.newPlot(camerasPlot, [{
        x: data['cameras']['date'],
        y: data['cameras']['traffic']
    }], {
        margin: { t: 1 }
    });
    console.log(data['cameras'])

    const timestampsPlot = document.getElementById('timestampsPlot');
    Plotly.newPlot(timestampsPlot, [{
        x: data['timestamps']['date'],
        y: data['timestamps']['traffic']
    }], {
        margin: { t: 1 }
    });
}