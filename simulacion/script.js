

src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js"

let ingreso = false;
let baseTime = 0;
let pauseTime = 0;
inPause = false;
let startTime = null; // Variable to store the start time when green button is pressed
let endTime_pausa = null; // Variable to store the end time when red button is pressed
let empty_display = "________-_";
let rut = "";
registred = false;

let begin = true;
let greenButtonPressed = false;
let ingresoRUT = false;
let gatoInicio = false;
let orangeButtonPressed = false;
let ingresoPausa = false;
let gatoPausa = false;

var brokerUrl = "broker.hivemq.com";
var brokerPort = 8000;
var clientId = "1556898409";
var client = new Paho.MQTT.Client(brokerUrl, brokerPort, clientId);

client.onConnectionLost = onConnectionLost;
client.onMessageArrived = onMessageArrived;

// Connect to the MQTT broker
client.connect({ onSuccess: onConnect, onFailure: onFailure });

let sensorCount = 0;
changeText();



function appendToDisplay(value) {
    const display = document.getElementById('display');

    if(orangeButtonPressed){
        if (value == '#' && display.value[0] == 'A' || display.value[0] == 'B' || display.value[0] == 'C' || display.value[0] == 'D' || display.value[0] == '0'){
            pauseTime = new Date();
            hour = pauseTime.getHours();
            minutes = pauseTime.getMinutes();
            if (minutes < 10){
                display.value = `Hora pausa: ${hour}:0${minutes}`;
                
            }
            else{display.value = `Hora pausa: ${hour}:${minutes}`;
                 }
            
            var horasTrabajadas = pauseTime - startTime;
            baseTimeHoras = baseTime + pauseTime - startTime;
            orangeButtonPressed = false;
            gatoPausa = true;

            mm =  `Pausa. Horas trabajadas: ${Math.floor(horasTrabajadas/3600000)} hrs:${Math.floor(horasTrabajadas/60000)} min`;
            var message = new Paho.MQTT.Message(mm);
            message.destinationName = "test_sexy";
            client.send(message);

            setTimeout(function() {display.value = `Horas trabajadas: ${Math.floor(horasTrabajadas/3600000)} hrs:${Math.floor(horasTrabajadas/60000)} min`}, 3000);

        }
        
        
        if(value == 'A' || value == 'B' || value == 'C' || value == 'D' || value == '0'){
            
                if (value == 'A'){display.value = 'A: Falta material';}

                if (value == 'B'){display.value = 'B: Colación';}

                if (value == 'C'){display.value = 'C: Baño';}

                if (value == 'D'){display.value = 'D: Emergencia';}

                if (value == '0'){display.value = '0: Descanso';}
            
        }
        
    }
    
    if (greenButtonPressed | ingresoRUT) {
        
        if (value == '#'){
            gatoInicio = true
            greenButtonPressed = false;
            ingresoRUT = false;
            startTime = new Date();
            hour = startTime.getHours();
            minutes = startTime.getMinutes();
            
            if (minutes < 10){
                display.value = `Hora entrada: ${hour}:0${minutes}`;
                mm =  `Hora entrada: ${hour}:0${minutes}`;
                var message = new Paho.MQTT.Message(mm);
                message.destinationName = "test_sexy";
                client.send(message);
            }
            else{display.value = `Hora entrada: ${hour}:${minutes}`;
                mm =  `Hora entrada: ${hour}:${minutes}`;
                var message = new Paho.MQTT.Message(mm);
                message.destinationName = "test_sexy";
                client.send(message);}
            
            
            
        }
        else{
            ingresoRUT = true;
            greenButtonPressed = false;
            //display.value = display.value.length;
            
            if(rut.length < 10 & value != 'A' && value != 'B' && value != 'C' && value != 'D')

                {                  
                    if(rut.length < 9){
                        display.value = " ";
                        rut += value;
                    
                    for(var i = 0; i <= empty_display.length; i++){
                        if (i < rut.length){
                            if (i < 8){
                                display.value += rut[i];
                            }
                            else{
                                display.value += "-" + rut[8];
                            }                            
                        }
                        else{
                            if(i < 10 & rut.length < 9){
                                display.value += empty_display[i] + " ";
                            }
                        }
                            
                    } }

                    }
            
            
        }
        
    }
}



function clearDisplay() {
    const display = document.getElementById('display');
    display.value = '';
}


function handleAction(action) {
    const display = document.getElementById('display');
    var green_buttons = document.getElementsByClassName('green-button');
    var orange_buttons = document.getElementsByClassName('orange-button');
    var red_buttons = document.getElementsByClassName('red-button');
    green_buttons[0].style.boxShadow =  "0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19)";
    orange_buttons[0].style.boxShadow =  "0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19)";
    red_buttons[0].style.boxShadow =  "0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19)";
    


    
        // Green button is not pressed, allow only orange or red button actions
        
        
            
            if(action == 'INICIO'){
                if (begin | ingresoRUT | gatoPausa){
                    display.value = 'Ingreso: escriba su rut/RFID';
                    greenButtonPressed = true;
                    begin = false;
                    ingresoRUT = false;
                    gatoPausa = false;
                    display.value = '_ _ _ _ _ _ _ _ - _';
                    rut = "";
                    green_buttons[0].style.boxShadow =  "0 0 15px 10px green, 0 0 20px 15px green";
                    
                }}

            if(action == 'PAUSA'){
                if(gatoInicio){
                console.log(action);
                display.value = 'Razón de pausa?';
                orangeButtonPressed = true;
                gatoInicio = false;
                orange_buttons[0].style.boxShadow =  "0 0 15px 10px orange, 0 0 20px 15px orange";
                }}

            if(action == 'TERMINO'){
                if(gatoInicio){
                gatoInicio = false;
                begin = true;
                endTime = new Date(); // Record the end time when red button is pressed
                red_buttons[0].style.boxShadow =  "0 0 15px 10px red, 0 0 20px 15px red";
                totalTime = endTime - startTime + baseTime;

                mm = `Término. Horas totales trabajadas:  ${Math.floor(totalTime/3600000)} hrs: ${Math.floor(totalTime/60000)} min`;
                var message = new Paho.MQTT.Message(mm);
                message.destinationName = "test_sexy";
                client.send(message);
                
                display.value = `Fin: ${endTime.getHours()}: ${endTime.getMinutes()} hrs`
                setTimeout(function() {display.value = `Horas totales: ${Math.floor(totalTime/3600000)} hrs: ${Math.floor(totalTime/60000)} min`;}, 3000);}
            }
            
            
        
     
    
    
}

function writeToDisplay(text) {
    const display = document.getElementById('display');
    
    if (!greenButtonPressed) {
        // Green button not pressed, display RFID action
        display.value = `Inicie primero`;
    } else {
        // Green button pressed, record the start time and allow number pads or RFID inputs
        //startTime = new Date(); // Record the start time when green button is pressed
        display.value = `19876098-1`;
        greenButtonPressed = true;
    }
}

// Simulation of logic for orange button action after a few seconds
function simulateOrangeButtonAction() {
    const display = document.getElementById('display');
    
    setTimeout(() => {
        const numberInput = prompt('Enter any number:');
        const differenceInSeconds = (endTime - startTime) / 1000;
        display.value = `Difference in seconds: ${differenceInSeconds.toFixed(2)}, Entered number: ${numberInput}`;
    }, 5000); // Wait for 5 seconds before prompting for number input
}

function resetSensor(){
    console.log("Let's reset");
}

function changeText() {

    var extra = Math.floor(Math.random()*10/2);
    sensorCount += extra; // Generate random text
    if(extra > 0){
        document.getElementById('info-box').style.backgroundColor = "yellow";
        document.getElementById('data-sensor').textContent = sensorCount; // Change text content
        setTimeout(function() {document.getElementById('info-box').style.backgroundColor = "lightgray";}, 1000);

    }
    
    var randomTime = Math.floor(Math.random() * 5000) + 1000; // Generate random time between 1 and 5 seconds
    setTimeout(changeText, randomTime); // Call the function recursively after random time
}

/// MQTT FUNCTIONS

function onConnect() {
    console.log("Connected to MQTT broker");
    // Subscribe to a topic
    client.subscribe("your/topic");
  }
  
  // Called when the client loses its connection
function onConnectionLost(responseObject) {
if (responseObject.errorCode !== 0) {
    console.log("Connection lost:", responseObject.errorMessage);
}
}
  
// Called when a message arrives
function onMessageArrived(message) {
console.log("Message arrived:", message.payloadString);
}


function sendMessage(message){
    var message_to_send = new Paho.MQTT.Message("Hello, MQTT!");
    message_to_send.destinationName = "your/topic";
    client.send(message_to_send)
}

function onConnect() {
    console.log("Connected to MQTT broker");
    client.subscribe("test_sexy");
  }

  // Function to send the number through MQTT
  function sendNumberThroughMQTT() {
    var displayValue = document.getElementById("display").value;
    var message = new Paho.MQTT.Message(displayValue);
    message.destinationName = "test_sexy";
    client.send(message);
    console.log("Sent number:", displayValue);
  }

  // Function to add numbers to the calculator display
  function addToDisplay(number) {
    document.getElementById("display").value += number;
  }


  // Called when the connection fails
  function onFailure(errorMessage) {
    console.error("Failed to connect to MQTT broker:", errorMessage.errorMessage);
  }
  
  function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
      console.log("Connection lost:", responseObject.errorMessage);
    }
  }

  function onMessageArrived(message) {
    console.log("Message arrived on topic:", message.destinationName);
    console.log("Payload:", message.payloadString);

    
  }

  // Subscribe to a topic
function subscribeToTopic(topic) {
    client.subscribe(topic);
}



