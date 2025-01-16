clear all;
close all;
clc;

% Configura a conexão com o Arduino
arduinoObj = serial('COM10', 'BaudRate', 9600); % Substitua COM6 pela porta correta
fopen(arduinoObj);

% Cria a interface gráfica
f = figure('Name', 'Controle de Posição dos Motores', 'NumberTitle', 'off', ...
           'Position', [100, 100, 800, 450]);

% Sliders para controlar a posição dos motores
sliders = gobjects(1, 6); % Armazena os sliders
for i = 1:6
    uicontrol('Style', 'text', 'Position', [50, 450 - i * 60, 100, 20], ...
              'String', ['Motor ' num2str(i)], 'FontSize', 12);
    sliders(i) = uicontrol('Style', 'slider', 'Min', -1000, 'Max', 1000, 'Value', 0, ...
                           'Position', [150, 450 - i * 60, 600, 20], ...
                           'Callback', @(src, ~) updateMotor(arduinoObj, src.Value, i));
end

% Botão home
uicontrol('Style', 'text', 'Position', [50, 450-7*60, 100, 20], 'ForegroundColor', [0.5 0 0.5], 'FontSize', 15);
home_bottun = uicontrol('Style', 'pushbutton', 'String', 'Home', ...
                        'Position', [50, 450-7*60, 80, 40], ...
                        'ForegroundColor', [0.5 0 0.5], 'FontSize', 20, ...
                        'Callback', @(src, ~) updateMotor(arduinoObj, 0, 6));                    
%     
% function home()
%     updateMotor(arduinoObj, 0, 6)
%     updateMotor(arduinoObj, 0, 5)
%     updateMotor(arduinoObj, 0, 4)
%     updateMotor(arduinoObj, 0, 3)
%     updateMotor(arduinoObj, 0, 2)
%     updateMotor(arduinoObj, 0, 1)
% end

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



% Função para fechar a comunicação serial
function closeSerial(arduinoObj)
    fclose(arduinoObj); % Fecha a comunicação serial
    delete(arduinoObj); % Remove o objeto serial
    clear arduinoObj; % Limpa a variável do espaço de trabalho
    delete(gcf); % Fecha a interface gráfica
end


