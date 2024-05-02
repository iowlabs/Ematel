

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









function appendToDisplay(value) {
    const display = document.getElementById('display');

    if(orangeButtonPressed){
        if (value == '#' && display.value[0] == 'A' || display.value[0] == 'B' || display.value[0] == 'C' || display.value[0] == 'D' || display.value[0] == '0'){
            pauseTime = new Date();
            hour = pauseTime.getHours();
            minutes = pauseTime.getMinutes();
            display.value = `Hora pausa: ${hour}:${minutes}`;
            baseTimeHoras = baseTime + pauseTime - startTime;
            orangeButtonPressed = false;
            gatoPausa = true;
        }
        
        
        if(value == 'A' || value == 'B' || value == 'C' || value == 'D' || value == '0'){
            
                if (value == 'A'){display.value = 'A: Falta material';}

                if (value == 'B'){display.value = 'B: Colación';}

                if (value == 'C'){display.value = 'C: Baño';}

                if (value == 'D'){display.value = 'D: Emergencia';}

                if (value == '0'){display.value = '0: Descanso';}
            
        }
        
    }
    
    if (greenButtonPressed) {
        
        if (value == '#'){
            gatoInicio = true
            greenButtonPressed = false;
            startTime = new Date();
            hour = startTime.getHours();
            minutes = startTime.getMinutes();
            display.value = `Hora entrada: ${hour}:${minutes}`;
            
            
        }
        else{
            //display.value = display.value.length; 
            if(display.value.length < 10 && value != 'A' && value != 'B' && value != 'C' && value != 'D')
                {
                    console.log(value);
                    rut += value;
                    for(var i = 0; i <= empty_display.length; i++){
                        if (i < rut.length){
                            display.value += rut[i];
                        }
                        else{
                            display.value += empty_display[i] + " ";
                        }
                            
                    } 

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
        switch (action) {
            case 'INICIO':
                if (begin | ingresoRUT | gatoPausa){
                    display.value = 'Ingreso: escriba su rut/RFID';
                    greenButtonPressed = true;
                    begin = false;
                    ingresoRUT = false;
                    gatoPausa = false;
                    display.value = '_ _ _ _ _ _ _ _ - _';
                    rut = "";
                    green_buttons[0].style.boxShadow =  "0 0 15px 10px rgba(0, 0, 0, 0.3), 0 0 20px 15px rgba(0, 0, 0, 0.25)";
                    break;
                }

            case 'PAUSA':
                if(gatoInicio){
                display.value = 'Razón de pausa?';
                orangeButtonPressed = true;
                gatoInicio = false;
                endTime_pausa = new Date();
                orange_buttons[0].style.boxShadow =  "0 0 15px 10px rgba(0, 0, 0, 0.3), 0 0 20px 15px rgba(0, 0, 0, 0.25)";
                break;}

            case 'TERMINO':
                if(gatoInicio){
                gatoInicio = false;
                endTime = new Date(); // Record the end time when red button is pressed
                red_buttons[0].style.boxShadow =  "0 0 15px 10px rgba(0, 0, 0, 0.3), 0 0 20px 15px rgba(0, 0, 0, 0.25)";
                totalTime = endTime - startTime + baseTime;
                console.log(totalTime);
                display.value = `Fin: ${endTime.getHours()}: ${endTime.getMinutes()} hrs, ${Math.floor(totalTime/3600000)}: ${Math.floor(totalTime/60000)} hrs trabajadas`;
                break;   }
            
        
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
        display.value = `198760981`;
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