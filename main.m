clear all;
close all;
clc;

% Configura a conexão com o Arduino
arduinoObj = serial('COM6', 'BaudRate', 9600); % Substitua COM10 pela porta correta
fopen(arduinoObj);

% Cria a interface gráfica
f = figure('Name', 'Controle de Posição dos Motores', 'NumberTitle', 'off', ...
           'Position', [100, 100, 800, 450]);

% Sliders para controlar a posição dos motores
sliders = gobjects(1, 6); % Armazena os sliders
for i = 1:6
    uicontrol('Style', 'text', 'Position', [50, 450 - i * 60, 100, 20], ...
              'String', ['Motor ' num2str(i)], 'FontSize', 12);
    sliders(i) = uicontrol('Style', 'slider', 'Min', -360, 'Max', 360, 'Value', 0, ...
                           'Position', [150, 450 - i * 60, 600, 20], ...
                           'Callback', @(src, ~) updateMotor(arduinoObj, src.Value, i));
end

% Botão "Home" para resetar todos os motores
uicontrol('Style', 'pushbutton', 'String', 'Home', ...
          'Position', [50, 450 - 7 * 60, 80, 40], ...
          'ForegroundColor', [0.5 0 0.5], 'FontSize', 20, ...
          'Callback', @(src, ~) resetAllMotors(arduinoObj, sliders));

% Botão "Zerar" para redefinir todas as posições como zero
uicontrol('Style', 'pushbutton', 'String', 'Zerar', ...
          'Position', [150, 450 - 7 * 60, 80, 40], ...
          'ForegroundColor', 'r', 'FontSize', 20, ...
          'Callback', @(src, ~) zeroAllMotors(arduinoObj, sliders));
     
% Configura o fechamento seguro da comunicação serial
f.CloseRequestFcn = @(~, ~) closeSerial(arduinoObj);

% Função para atualizar a posição do motor com base no valor do slider
function updateMotor(arduinoObj, position, motorIndex)
    % Verifica se o objeto serial está aberto
    if strcmp(arduinoObj.Status, 'closed')
        fopen(arduinoObj);
    end

    % Cria a string de comando
    command = sprintf('P%d,%d\n', motorIndex, round(position));
    sprintf('Motor: %d, Posição: %d', motorIndex, round(position))

    % Envia o comando para o Arduino
    try
        fprintf(arduinoObj, command);
    catch
        warning('Falha ao enviar comando ao Arduino.');
    end
end

% Função para resetar todos os motores
function resetAllMotors(arduinoObj, sliders)
    for i = 1:6
        updateMotor(arduinoObj, 0, i); % Envia posição zero para cada motor
    end
    
    % Reseta todos os sliders para 0
        for i = 1:length(sliders)
            set(sliders(i), 'Value', 0);
        end
    
end

% Função para enviar o comando de zerar e resetar sliders
function zeroAllMotors(arduinoObj, sliders)
    % Verifica se o objeto serial está aberto
    if strcmp(arduinoObj.Status, 'closed')
        fopen(arduinoObj);
    end

    % Envia o comando "ZERO" para o Arduino
    try
        fprintf(arduinoObj, "ZERO\n");
        disp("Posições zeradas!");

        % Reseta todos os sliders para 0
        for i = 1:length(sliders)
            set(sliders(i), 'Value', 0);
        end
    catch
        warning('Falha ao enviar o comando ZERO ao Arduino.');
    end
end

% Função para fechar a comunicação serial
function closeSerial(arduinoObj)
    fclose(arduinoObj); % Fecha a comunicação serial
    delete(arduinoObj); % Remove o objeto serial
    clear arduinoObj; % Limpa a variável do espaço de trabalho
    delete(gcf); % Fecha a interface gráfica
end
