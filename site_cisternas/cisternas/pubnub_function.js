// Declare the Function with the export syntax. The incoming message is called request
export default (request) => {

    // Require XHR to make external calls
    var xhr = require("xhr");
    // Require console to print debug information
    var console = require('console');

    try {
        // Para debug
        /*
        console.log("request.message");
        console.log(request.message);
        
        console.log("eon");
        console.log(request.message.eon);
        
        console.log("cisterna para POST");
        console.log(request.message.eon.cisterna);
        // console.log("time_stamp para POST");
        // console.log(request.message.time_stamp);
        console.log("litros para POST");
        console.log(request.message.eon.litros);
        */
        const http_options = {
            "method": "POST",
            "headers": {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            "body": "cisterna=" + "http://marcelosc.pythonanywhere.com/api/cisternas/" + request.message.cisterna + "/" + "&time_stamp=" + request.message.time_stamp + "&litros=" + request.message.eon.litros
        };
        const url = "http://marcelosc.pythonanywhere.com/api/medicoes/";
    
        
        return xhr.fetch(url, http_options).then((x) => {
            const body = JSON.parse(x.body);
            //console.log(body);
            return request.ok();
        });  

    } catch (e) {
        console.error('Uncaught exception:', e);
    }
};

