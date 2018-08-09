// This Function fetches a random email from an external API and inserts 
// a key-value pair {"email":"emailValue"} in the message
// For testing: you can use the following test payload: {"foo":"bar"}

// Below is the code with inline explanations

// Declare the Function with the export syntax. The incoming message is called request
export default (request) => {

    // Require XHR to make external calls
    var xhr = require("xhr");
    // Require console to print debug information
    var console = require('console');

    try {
        // Fetch a random email from an external API
        /*
        return xhr.fetch("http://api.randomuser.me/?inc=email", {})
        .then(function(response){
            // Parse the JSON result into a JavaScript object
            var body = JSON.parse(response.body);
            var email = body['results'][0]["email"];
            console.log("email "+ email);

            // Augment the message with the email
            request.message.email = email;
            return request.ok();
      });
      */

    const xhr = require("xhr");
    const query = require('codec/query_string');
    const query_params = {
        "user-id": "geremy@pubnub.com",
        "api-key": "HwX6r15P58A9VJCD47vG4cSoCM0OBraiuf6B9jyi2LYUCGi7",
        "address": request.message.address,
        "country-code": request.message.country
    };
    
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
    
    const http_options = {
        "method": "POST",
        "headers": {
            'Content-Type': 'application/x-www-form-urlencoded',
        },
        // "body": "cisterna=http://marcelosc.pythonanywhere.com/api/cisternas/1/&time_stamp=2018-08-08T13:41:17&litros=65"
        "body": "cisterna=" + "http://marcelosc.pythonanywhere.com/api/cisternas/1/" + "&time_stamp=" + request.message.eon.time_stamp + "&litros=" + request.message.eon.litros
    };
 
    //const url = "https://neutrinoapi.com/geocode-address" + "?" + query.stringify(query_params);
    // const url = "http://marcelosc.pythonanywhere.com/api/medicoes/ cisterna=\"http://marcelosc.pythonanywhere.com/api/cisternas/1/\" time_stamp=\"2018-08-08T13:41:17\" litros=\"65\"";
    // const url = "http://marcelosc.pythonanywhere.com/api/medicoes/1/";
    const url = "http://marcelosc.pythonanywhere.com/api/medicoes/";
 
    // return xhr.fetch(url).then((x) => {
    //     const body = JSON.parse(x.body);
    //     const result = body.locations[0];
    //     request.message.country = result.country;
    //     request.message.zip = result["postal-code"];
    //     request.message.address = result.address;
    //     request.message.city = result.city;
    //     request.message.lat = result.latitude;
    //     request.message.lon = result.longitude;
    //     return request.ok();
    // });
    /*
    xhr.fetch(url).then((x) => {
        console.log(x);
        const body = JSON.parse(x.body);
        console.log("body");
        console.log(body);
        // console.log("cisterna");
        // console.log(body.cisterna);
        // console.log("time_stamp");
        // console.log(body.time_stamp);
        // console.log("litros");
        // console.log(body.litros);
    });
    return request.ok()
    */
    
    return xhr.fetch(url, http_options).then((x) => {
        const body = JSON.parse(x.body);
        console.log(body);
        return request.ok();
    });  
    // return xhr.fetch(url).then((x) => {
    //     const body = JSON.parse(x.body);
    //     // const result = body.locations[0];
    //     const result = body[0];
    //     request.message.cisterna = result.cisterna;
    //     request.message.time = result["time_stamp"];
    //     request.message.litros = result.litros;
    //     return request.ok();
    // });

  } catch (e) {
    console.error('Uncaught exception:', e);
  }
};

