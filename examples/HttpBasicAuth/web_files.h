const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Monitor Sensori</title>
  <script src="https://cdn.tailwindcss.com"></script>
  <script src="https://cdn.jsdelivr.net/npm/gaugeJS/dist/gauge.min.js"></script>
</head>
<body class="bg-gray-900 text-white flex items-center justify-center min-h-screen">
  <div class="text-center grid grid-cols-2 gap-8">
    <div>
      <h1 class="text-2xl font-bold mb-4">Sensore 101</h1>
      <canvas id="gaugeCanvas101" width="200" height="100"></canvas>
      <p id="sensorValue101" class="text-4xl font-bold text-green-400 mt-4">--</p>
    </div>
    <div>
      <h1 class="text-2xl font-bold mb-4">Sensore 102</h1>
      <canvas id="gaugeCanvas102" width="200" height="100"></canvas>
      <p id="sensorValue102" class="text-4xl font-bold text-blue-400 mt-4">--</p>
    </div>
  </div>
  <script src="index.js"></script>
</body>
</html>
)rawliteral";


const char index_js[] PROGMEM = R"rawliteral(
const opts = {
    angle: 0,
    lineWidth: 0.2,
    radiusScale: 1,
    pointer: {
      length: 0.6,
      strokeWidth: 0.03,
      color: '#ffffff'
    },
    limitMax: false,
    limitMin: false,
    colorStart: '#34D399',
    colorStop: '#34D399',
    strokeColor: '#f5f5f5',
    generateGradient: true,
    highDpiSupport: true
  };
  const gauge101 = new Gauge(document.getElementById("gaugeCanvas101")).setOptions(opts);
  gauge101.maxValue = 100;
  gauge101.setMinValue(0);
  gauge101.animationSpeed = 32;
  gauge101.set(0);
  const gauge102 = new Gauge(document.getElementById("gaugeCanvas102")).setOptions(opts);
  gauge102.maxValue = 100;
  gauge102.setMinValue(0);
  gauge102.animationSpeed = 32;
  gauge102.set(0);
  function fetchSensorData(sensorId, gauge, valueElement) {
    fetch(`/sensor?sensorId=${sensorId}`)
      .then(response => response.json())
      .then(data => {
        const value = data.value;
        document.getElementById(valueElement).textContent = value;
        gauge.set(value);
      })
      .catch(error => console.error(`Errore nel recupero dei dati per il sensore ${sensorId}:`, error));
  }
  function updateSensors() {
    fetchSensorData(101, gauge101, "sensorValue101");
    fetchSensorData(102, gauge102, "sensorValue102");
  }
  setInterval(updateSensors, 2000);
)rawliteral";

