#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

const int dirPins[6] = {13, 12, 11, 10, 9, 8}; // Pinos DIR para os motores 1 a 6
const int stepPins[6] = {7, 6, 5, 4, 3, 2};    // Pinos STEP para os motores 1 a 6

const int stepsPerRevolution = 128000; // Passos por volta (ajuste conforme seu motor)
float currentPositions[6] = {0, 0, 0, 0, 0, 0}; // Posições atuais em graus

void setup() {
    for (int i = 0; i < 6; i++) {
        pinMode(stepPins[i], OUTPUT);
        pinMode(dirPins[i], OUTPUT);
    }
    Serial.begin(9600); // Inicia a comunicação serial
}

void loop() {
    // Verifica se há dados disponíveis na serial
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n'); // Lê o comando até o '\n'
        parseCommand(command);
    }
}

// Função para interpretar o comando recebido
void parseCommand(String command) {
    if (command.charAt(0) == 'P') { // Comando de posição
        int commaIndex = command.indexOf(',');
        int motorIndex = command.substring(1, commaIndex).toInt() - 1; // Índice do motor (0 a 5)
        float targetPosition = command.substring(commaIndex + 1).toFloat(); // Posição alvo em graus

        if (motorIndex >= 0 && motorIndex < 6) {
            moveToPosition(motorIndex, targetPosition);
        }
    }
}

// Função para mover o motor até a posição desejada
void moveToPosition(int motorIndex, float targetPosition) {
    float stepsPerDegree = stepsPerRevolution / 360.0; // Passos por grau
    int targetSteps = round(targetPosition * stepsPerDegree); // Passos alvo
    int currentSteps = round(currentPositions[motorIndex] * stepsPerDegree); // Passos atuais

    int stepsToMove = targetSteps - currentSteps; // Diferença em passos

    if (stepsToMove != 0) {
        digitalWrite(dirPins[motorIndex], stepsToMove > 0 ? HIGH : LOW); // Define a direção
        stepsToMove = abs(stepsToMove);

        for (int i = 0; i < stepsToMove; i++) {
            digitalWrite(stepPins[motorIndex], HIGH);
            delayMicroseconds(50); // Ajuste para velocidade
            digitalWrite(stepPins[motorIndex], LOW);
            delayMicroseconds(50);
        }

        // Atualiza a posição atual
        currentPositions[motorIndex] = targetPosition;
    }
}




