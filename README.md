# Laberinto de Mármol

diseño e implementación hardware-software de un prototipo de puzzle tipo
laberinto de Mármol usando la plataforma de Arduino. Usando una matriz de LEDs RGB WS2812B y sensor giroscopio/acelerómetro MPU-6050 


Archivo de configuracion de `arduino.json` para **vscode**
```json
{
  "sketch": "src\\main.ino",
  "board": "arduino:avr:uno",
  "output": "build", // Mejora los tiempos de compilado
  "buildPreferences": [
    [
      // Le indica al compiladora que advertencias ignorar
      "compiler.cpp.extra_flags",
      "-Wno-register -Wno-deprecated-declarations -Wno-misleading-indentation"
    ]
  ]
}
```
