#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

const int dirPins[6] = {13, 12, 11, 10, 9, 8}; // Pinos DIR para os motores 1 a 6
const int stepPins[6] = {7, 6, 5, 4, 3, 2};    // Pinos STEP para os motores 1 a 6

const int stepsPerRevolution = 3200; // Ajuste conforme seu motor e redução
float currentPositions[6] = {0, 0, 0, 0, 0, 0}; // Posições atuais

void setup() {
    for (int i = 0; i < 6; i++) {
        pinMode(stepPins[i], OUTPUT);
        pinMode(dirPins[i], OUTPUT);
    }
    Serial.begin(9600); // Inicia a comunicação serial
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n'); // Lê o comando até o '\n'
        parseCommand(command);
    }
}

// Função para interpretar os comandos
void parseCommand(String command) {
    if (command.charAt(0) == 'S') { // Comando para executar os pontos
        executeSimultaneousMovement(command.substring(1));
        return;
    }

    int commaIndex = command.indexOf(',');
    if (command.charAt(0) != 'P' || commaIndex == -1) {
        return; // Comando inválido
    }
    int motorIndex = command.substring(1, commaIndex).toInt() - 1; // Índice do motor
    float targetPosition = command.substring(commaIndex + 1).toFloat(); // Posição alvo

    if (motorIndex >= 0 && motorIndex < 6) {
        moveToPosition(motorIndex, targetPosition);
    }
}

// Função para executar movimento simultâneo
void executeSimultaneousMovement(String positions) {
    int targetSteps[6];
    int maxSteps = 0;

    // Calcula os passos alvo para cada motor
    for (int i = 0; i < 6; i++) {
        int commaIndex = positions.indexOf(',');
        float targetPosition = positions.substring(0, commaIndex).toFloat();
        positions = positions.substring(commaIndex + 1);

        float stepsPerDegree = stepsPerRevolution / 360.0;
        targetSteps[i] = round(targetPosition * stepsPerDegree);
        int currentSteps = round(currentPositions[i] * stepsPerDegree);

        targetSteps[i] -= currentSteps; // Passos relativos
        maxSteps = max(maxSteps, abs(targetSteps[i])); // Calcula o maior número de passos
    }

    // Define as direções
    for (int i = 0; i < 6; i++) {
        digitalWrite(dirPins[i], targetSteps[i] > 0 ? HIGH : LOW);
        targetSteps[i] = abs(targetSteps[i]); // Torna os passos absolutos
    }

    // Algoritmo para movimento simultâneo
    for (int step = 0; step < maxSteps; step++) {
        for (int i = 0; i < 6; i++) {
            if (step < targetSteps[i]) {
                digitalWrite(stepPins[i], HIGH);
            }
        }
        delayMicroseconds(50); // Ajuste para velocidade
        for (int i = 0; i < 6; i++) {
            if (step < targetSteps[i]) {
                digitalWrite(stepPins[i], LOW);
            }
        }
        delayMicroseconds(50);
    }

    // Atualiza as posições atuais
    for (int i = 0; i < 6; i++) {
        currentPositions[i] += targetSteps[i] * (360.0 / stepsPerRevolution);
    }
}

// Função para mover um único motor (caso necessário)
void moveToPosition(int motorIndex, float targetPosition) {
    float stepsPerDegree = stepsPerRevolution / 360.0;
    int targetSteps = round(targetPosition * stepsPerDegree);
    int currentSteps = round(currentPositions[motorIndex] * stepsPerDegree);

    int stepsToMove = targetSteps - currentSteps;
    if (stepsToMove == 0) return; // Sem movimento necessário

    digitalWrite(dirPins[motorIndex], stepsToMove > 0 ? HIGH : LOW);
    stepsToMove = abs(stepsToMove);

    for (int i = 0; i < stepsToMove; i++) {
        digitalWrite(stepPins[motorIndex], HIGH);
        delayMicroseconds(50); // Ajuste para velocidade
        digitalWrite(stepPins[motorIndex], LOW);
        delayMicroseconds(50);
    }

    currentPositions[motorIndex] = targetPosition;
}

