function csv_plot(filename, plot_title)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

data = csvread(filename);
col1 = data(:,1);
col2 = data(:,2);

figure(1)
plot(col1, col2(1:size(col1)), title(plot_title);
end

