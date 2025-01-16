#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

const int dirPins[6] = {13, 12, 11, 10, 9, 8}; // Pinos DIR para os motores 1 a 6
const int stepPins[6] = {7, 6, 5, 4, 3, 2};    // Pinos STEP para os motores 1 a 6

const int stepsPerRevolution = 7600; // Ajuste conforme seu motor e redução
float currentPositions[6] = {0, 0, 0, 0, 0, 0}; // Posições atuais
const int maxDelay = 500; // Atraso inicial (mais lento)
const int minDelay = 50;  // Atraso mínimo (mais rápido)
const int rampSteps = 50; // Número de passos para completar a rampa

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

void parseCommand(String command) {
    // Comando para zerar todas as posições
    if (command == "ZERO") {
        for (int i = 0; i < 6; i++) {
            currentPositions[i] = 0; // Reseta a posição de todos os motores
        }
        Serial.println("Posições zeradas!");
        return;
    }

    // Comando padrão de posição
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


void moveToPosition(int motorIndex, float targetPosition) {
    float stepsPerDegree = stepsPerRevolution / 360.0;
    int targetSteps = round(targetPosition * stepsPerDegree);
    int currentSteps = round(currentPositions[motorIndex] * stepsPerDegree);

    int stepsToMove = targetSteps - currentSteps;
    if (stepsToMove == 0) return; // Sem movimento necessário

    digitalWrite(dirPins[motorIndex], stepsToMove > 0 ? HIGH : LOW); // Define a direção
    stepsToMove = abs(stepsToMove);

    int currentDelay = maxDelay;

    // Aceleração
    for (int i = 0; i < rampSteps && i < stepsToMove / 2; i++) {
        currentDelay = maxDelay - pow(i, 1.5) * (maxDelay - minDelay) / pow(rampSteps, 1.5);
        singleStep(motorIndex, currentDelay);
    }

    // Velocidade constante
    for (int i = rampSteps; i < stepsToMove - rampSteps; i++) {
        singleStep(motorIndex, minDelay);
    }

    // Desaceleração
    for (int i = rampSteps; i > 0 && stepsToMove > rampSteps; i--) {
        currentDelay = maxDelay - pow(i, 1.5) * (maxDelay - minDelay) / pow(rampSteps, 1.5);
        singleStep(motorIndex, currentDelay);
    }

    currentPositions[motorIndex] = targetPosition; // Atualiza a posição atual
}

void singleStep(int motorIndex, int delayTime) {
    digitalWrite(stepPins[motorIndex], HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepPins[motorIndex], LOW);
    delayMicroseconds(delayTime);
}
