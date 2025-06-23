const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Mood Checker</title>
  <style>
    body {
      margin: 0;
      font-family: 'Segoe UI', sans-serif;
      background: linear-gradient(135deg, #667eea, #764ba2);
      color: white;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100vh;
      text-align: center;
    }

    h1 {
      font-size: 2.5em;
      margin-bottom: 30px;
    }

    .btn-container {
      display: flex;
      flex-wrap: wrap;
      gap: 15px;
      justify-content: center;
    }

    .mood-btn {
      padding: 15px 25px;
      font-size: 1.1em;
      border: none;
      border-radius: 30px;
      cursor: pointer;
      transition: transform 0.2s, background 0.3s;
      background: white;
      color: #333;
      font-weight: bold;
    }

    .mood-btn:hover {
      transform: scale(1.05);
      background: #f0f0f0;
    }

    #status {
      margin-top: 20px;
      font-size: 1em;
      opacity: 0.8;
    }
  </style>
</head>
<body>
  <h1>Hi! How are you feeling today?</h1>
  <div class="btn-container">
    <button class="mood-btn" onclick="sendMood('normal')">😐 Normal</button>
    <button class="mood-btn" onclick="sendMood('happy')">😄 Happy</button>
    <button class="mood-btn" onclick="sendMood('sad')">☹️ Sad</button>
    <button class="mood-btn" onclick="sendMood('rock')">🎵 Let's Rock</button>
    <button class="mood-btn" onclick="sendMood('dangerous')">⚠️ Dangerous</button>
  </div>
  <div id="status"></div>

  <script>
    async function sendMood(mood) {
      try {
        const response = await fetch(`/mood?mood=${encodeURIComponent(mood)}`);
        if (response.ok) {
          document.getElementById("status").innerText = `Mood sent: ${mood}`;
        } else {
          document.getElementById("status").innerText = "Failed to send mood.";
        }
      } catch (err) {
        document.getElementById("status").innerText = "Error connecting to server.";
      }
    }
  </script>
</body>
</html>
)rawliteral";