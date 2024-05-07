// ----------- CONSTANT & VARIABLE ------------
const labels_temp = [];

var current_state = "2"
// ----------- INITIALIZATION ------------
function chart_layout() {
  Plotly.newPlot('myChart', [], {
    title: '',
    xaxis: {
      title: 'Time'
    },
    yaxis: {
      title: 'Alarm',
      range: [0,1], // Imposta il range dell'asse y
      tickvals: [0.50, 1], // Valori delle linee sull'asse y
      ticktext: ['', ''], // Testo vuoto per nascondere le etichette
    },
    showlegend: false, // Nascondi la legenda
    margin: { t: 50 }, // Imposta il margine superiore per la visualizzazione del titolo
  });
}
// ----------- HELPER FUNCTION -----------
function handleTimeStamp(timestamp){
  // Generate timestamp from $(timestamp()) of aws.
  var seconds = Math.floor((timestamp / 1000) % 60);
  var minutes = Math.floor((timestamp / (1000 * 60)) % 60);
  var hours = Math.floor((timestamp / (1000 * 60 * 60)) % 24);
  return hours.toString().padStart(2, '0') + ":" +
          minutes.toString().padStart(2, '0') + ":" +
          seconds.toString().padStart(2, '0');
}

// Lambda Function : read_value
async function read_valueLambdaF(){
  try {
    const lambdaFunctionURL = 'https://mxaiiigvbv6lxzgp6knwae3jim0hxgcc.lambda-url.eu-west-1.on.aws/'; // Replace with the actual Lambda Function URL
  
    const response = await fetch(lambdaFunctionURL, {
      method: 'GET'
    });
  
    if (response.ok) {
      const data = await response.json();

      console.log('Lambda function, read_value from DynnamoDB, invocation successful!');
      //console.log(data);
      //console.log(data[3].payload.M.Distance)
      data.forEach((elem)=>{
        //console.log(elem)
        
        // Transform the response in a pretty way (This change based on the specific of AWS IoT Core Rule used to write on DynamoDB )
        const transformedPayload = {
          id: elem.id.N,
          ...Object.fromEntries(
            Object.entries(elem.payload.M).map(([key, value]) => [key, value.S])
          )
        };
        console.log(transformedPayload)
        var timestamp = transformedPayload.id
        var formattedTime = handleTimeStamp(timestamp);
        //console.log(formattedTime)
        //console.log(transformedPayload.id)
        
        
        var trace = {
          x: [formattedTime], // Tempo sull'asse x
          y: [0.5], // Valore fisso sull'asse y per il pallino rosso
          mode: 'markers', // Modalità punti
          marker: {
            color: transformedPayload["State"] === "1" ? 'rgb(255,0,0)' : 'rgb(0,0,0)', // Colore del pallino (rosso se lo stato è 1, altrimenti nero)
            size: 20 // Dimensione del pallino
          },
        };

        labels_temp.push(formattedTime); // Aggiungi il tempo all'array di etichette
        Plotly.addTraces('myChart', [trace]); // Aggiungi il punto al grafico
        //Plotly.addTraces('myChart', data);
        //my_chart.update();

      });
      labels_temp.sort(); // Ordina le etichette temporalmente
      Plotly.relayout('myChart', { 'xaxis.categoryorder': 'array', 'xaxis.categoryarray': labels_temp }); // Ordina l'asse x
      
      /*
      labels_temp.sort((a, b) => {
        if (a > b) {
          return -1;
        }
        if (a < b) {
          return 1;
        }
        return 0;
      });
      */

   
    } else {
      console.error('Lambda function invocation failed with status:', response.status);
    }
  } catch (error) {
    console.error('Error invoking Lambda function:', error.message);
  }

}
function update_status(current){
  switch(current){
    case "0":
      current_state="SMART ALARM"
      break; 
    case "1":
      current_state="SMART LOCK"
      break;
    case "2":
      current_state="OFF"
      break;
  }
  document.getElementById("current_state").innerHTML = "Current State : " + current_state 
}

// ----------- MAIN FUNCTION ------------
function main(){

  
  console.log("Start - Create chart layout")
  chart_layout();
  console.log("Start - Retrieve info for the graph")
  read_valueLambdaF();
  console.log(current_state)
  update_status(current_state)
  /*
  // Starting campling data - ogni 10 secondi.
  setInterval(read_valueLambdaF, 10000)
  */

}

document.addEventListener('DOMContentLoaded', function() {
  main()
});

// ----------- FUNCTION TRIGGERED FROM WEBSITE ------------

// Lambda Function : publish_to_iotcore	
async function postLambdaFunction(content) {
  update_status(content)
  await fetch("https://mzmk5uzj2fjrah5cqlcynm3ome0swley.lambda-url.eu-west-1.on.aws/", {
    method: 'POST',
    body: content,
  })
  .then(data => {
    // Gestisci la risposta dalla funzione Lambda
    console.log("publish_to_iotcore successfully - see on bridge");
  })
  .catch(error => {
    console.error('Error calling Lambda function:', error);
  });

}



  

  

