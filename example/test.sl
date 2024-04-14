var texto = readFile("index.html");
catnet.startServer(500);
catnet.send(texto);