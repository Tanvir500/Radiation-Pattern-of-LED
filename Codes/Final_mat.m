clc; close all;
clear;

% MATLAB script to read serial data from Arduino and plot lux vs angle

% Parameters
serialPort = 'COM4';  % Replace with your Arduino's serial port
baudRate = 9600;
expectedSteps = 100;  % Total number of steps expected

% Open the serial port
arduino = serialport(serialPort, baudRate);
configureTerminator(arduino, 'LF');  % Ensure the data is newline-terminated
flush(arduino);  % Clear any old data in the buffer

% Wait for Arduino to send "READY"
disp('Waiting for Arduino to initialize...');
while true
    line = readline(arduino);
    disp(line); % Display for debugging
    if contains(line, "READY")
        disp('Arduino is ready.');
        break;
    end
end

% Send the "RUN" command to start the motor and measurements
writeline(arduino, "RUN");
disp('RUN command sent to Arduino.');

% Initialize data arrays
angles = [];
luxValues = [];

% Read data from serial port
try
    while numel(angles) < expectedSteps
        line = readline(arduino);  % Read a line of data
        disp(line);  % Display the line for debugging

        % Skip lines that don't contain valid data
        if contains(line, "Step") || contains(line, "Measurement complete")
            continue;
        end

        % Parse the CSV data (Step, Angle, Lux)
        data = split(line, ',');
        if numel(data) == 3
            angle = str2double(data{2});  % Convert angle to numeric
            lux = str2double(data{3});    % Convert lux to numeric
            
            % Append to arrays if angle is within 0–180 degrees
            if angle <= 180
                angles(end+1) = angle; %#ok<SAGROW>
                luxValues(end+1) = lux; %#ok<SAGROW>
            end
        end
    end
catch
    % Stop reading when an error occurs (e.g., user interruption)
    disp('Stopped reading data from Arduino.');
end

% Close the serial port
clear arduino;

% Convert angles to radians for polar plotting
anglesRad = deg2rad(angles);

% Plot the data in a radial plot
figure;
polarplot(anglesRad, luxValues, '-o');
title('Light Intensity vs Angle (0–180 degrees)');
ax = gca;
ax.ThetaLim = [0 180];  % Limit the angle to 0–180 degrees
ax.RLim = [0 max(luxValues)+10];
ax.RGrid = 'on';
ax.ThetaTick = 0:30:180;  % Set angular ticks
