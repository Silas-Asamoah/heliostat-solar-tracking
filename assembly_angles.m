%SIMPLE MIRROR MODULE ASSEMBLY ANGLES FOR HELIOSTAT (Solar Power Tower)
% Gives assembly points for a 4 facet heliostat without gaps
%Credit goes to Nils Björkman
%The program uses the PARABOLOID canting method.
clear all
clc
h_tower = 120; %tower height [m]
h_heliostat = 3.5 ; % height of heliostat (from ground to center support)
[m]
mirror_size = 2.5 ; %size of one cant at a square-shaped mirror, [m]
dist = 300; % [m] ('heliostat distance from central tower [m]: '); %
%length on the ground - or used the input below
% dist = input ('heliostat distance from central tower [m]: '); %length on
the ground
h = h_tower - h_heliostat;
focallength = (dist.^2 + h.^2)^0.5; %pythagoras
%% mirror distribution
X = zeros;
Y = zeros;
points = [0, mirror_size/2];
points = [-fliplr(points), points]; %makes symmetrical array of delta X_Y
[X, Y] = meshgrid(points, points); %makes 2 quadratic matrixes (mesh)
Z = X.^2 / (4*focallength) + Y.^2 / (4*focallength); % parabolic mirror
equ. will be a matrix. Z is hight of an parabola with spec. focallength and
spec. width
hold on
figure (1)
plot (Y, X)
plot (X, Y)
surf (X, Y, Z);
stem3 (Y, X, Z)
title ([' Mirror assembly [m]; dist from tower: ', num2str(dist), ' m'])
xlabel('X')
ylabel('Y')
zlabel('Z')
hold off
