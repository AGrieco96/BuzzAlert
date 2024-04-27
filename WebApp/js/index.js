// ----------- CONSTANT & VARIABLE ------------
const labels_temp = [];
const my_data= {
    labels: labels_temp,
    datasets: [{
        label: 'Value',
        pointBackgroundColor: 'rgb(0,0,0)',
        borderWidth: 1,
        fill: false,
        tension: 0.3,
        data: [],
    }]  
}
var my_chart;

// ----------- INITIALIZATION ------------
function chart_layout(){

  // New graph with Char js
  my_chart = new Chart(document.getElementById("myChart"), {
      type: 'bar',
      data: my_data
  });

}
// ----------- HELPER FUNCTION -----------
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
      console.log(data);
      console.log(data[3].payload.M.Distance)
      data.forEach((elem)=>{
        // Transform the response in a pretty way (This change based on the specific of AWS IoT Core Rule used to write on DynamoDB )
        const transformedPayload = {
          id: elem.id.N,
          ...Object.fromEntries(
            Object.entries(elem.payload.M).map(([key, value]) => [key, value.S])
          )
        };
        var timestamp = transformedPayload.id
        //var formattedTime = handleTimeStamp(timestamp);
        console.log(trasnformedPayload.id)
        
        
        if(!(data_temp.labels.includes(formattedTime))){
          var temp = parseFloat(transformedPayload["value"])
          
          
          labels_temp.push(formattedTime);
          data_temp.datasets[0].data.push(temp);
        }


      });

      labels_temp.sort((a, b) => {
        if (a > b) {
          return -1;
        }
        if (a < b) {
          return 1;
        }
        return 0;
      });
      tempChart.update();
      exec_measurements();
      
   
    } else {
      console.error('Lambda function invocation failed with status:', response.status);
    }
  } catch (error) {
    console.error('Error invoking Lambda function:', error.message);
  }

}

function exec_measurements(){

  const values = data_temp.datasets[0].data;
  /*
  const max_temp = Math.max(...values);

  const min_temp = Math.min(...values);

  const sum_temp = values.reduce((acc, val) => acc + val, 0);
  const avg_temp = sum_temp / values.length;

  const variation = max_temp - min_temp;
  */

  //display to html
  document.getElementById("AVG_temp").innerHTML = "AVG : inserire valore "
  document.getElementById("MAX_temp").innerHTML = "MAX : inserire valore"
  document.getElementById("MIN_temp").innerHTML = "MIN : inserire valore" 
  document.getElementById("delta_temp").innerHTML = "Variation in last Hour: inserire valore"// + variation + "&degC"

}
// ----------- MAIN FUNCTION ------------
function main(){

  
  console.log("Start - Create chart layout")
  chart_layout();
  console.log("Start - Retrieve info for the graph")
  read_valueLambdaF();

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



  

  

