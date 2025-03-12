// WebSocket terminal page

const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html lang="it">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Terminale WebSocket</title>
      <style>
        body { font-family: monospace; background: black; color: lime; }
        #terminal { height: 300px; overflow-y: auto; border: 1px solid lime; padding: 10px; }
        #input { width: 100%; padding: 5px; border: none; background: black; color: lime; }
      </style>
    </head>
    <body>
      <h2>Terminale WebSocket</h2>
      <div id="terminal"></div>
      <input type="text" id="input" placeholder="Invia comando..." autofocus>
      <script>
        const socket = new WebSocket('ws://' + location.hostname + ':81/');
        const terminal = document.getElementById('terminal');
        const input = document.getElementById('input');
        socket.onopen = () => terminal.innerHTML += '<p>Connesso al WebSocket</p>';
        socket.onmessage = event => {
          terminal.innerHTML += `<p>&gt; ${event.data}</p>`;
          terminal.scrollTop = terminal.scrollHeight;
        };
        socket.onerror = () => terminal.innerHTML += '<p style="color: red;">Errore di connessione</p>';
        socket.onclose = () => terminal.innerHTML += '<p style="color: orange;">Connessione chiusa</p>';
        input.addEventListener('keypress', event => {
          if (event.key === 'Enter') {
            const message = input.value;
            socket.send(message);
            terminal.innerHTML += `<p><span style="color: cyan;">Utente:</span> ${message}</p>`;
            input.value = '';
          }
        });
      </script>
    </body>
    </html>
    )rawliteral";