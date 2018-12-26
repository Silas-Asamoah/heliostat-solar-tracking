%MIRROR MODULE ASSEMBLY ANGLES FOR HELIOSTAT
%Credit goes to Nils Björkman,  
%The program uses the PARABOLOID canting method.
%The program also uses the fresnel method. I.e. the paraboloid is divided
into
%flat elements, and each element’s respectively angles are maintained when
they are grounded on the planar frame.
clear all
clc
%shape = 'Spherical' ; %Select shape of mirror assembly. input
'paraboloid' or 'spherical'
shape = 'Paraboloid' ; %Select shape of mirror assembly. input
'paraboloid' or 'spherical'
h_tower = 120; %tower height [m]
h_heliostat = 5; % height of heliostat (from ground to center support) [m]
qty_y = 7; % quantity of square-shaped mirrors at each heliostat, one
vertical row (y-direction), enter odd number, e.g. 1, 3, 5, 7, 9, 11, ...
mirror_size = 1 ; %size of one cant at a square-shaped mirror, [m]
gap = 0.001 ; %The gap is defined as distance between each mirror module if
arranged as a parabola [m]
mirror_size_total = mirror_size + gap*2; %Adding gap distance to mirror
size
dist = 300; % [m] ('heliostat distance from central tower [m]: '); %
%length on the ground - or used the input below
% dist = input ('heliostat distance from central tower [m]: '); %length on
the ground
h = h_tower - h_heliostat;
focallength = (dist.^2 + h.^2)^0.5; %pythagoras
Rim_Diam = mirror_size_total * qty_y - gap*2;
Rim_Radius = Rim_Diam/2; %Radius of the mirror assembly
d = Rim_Radius.^2/(4*focallength); %hight of the disc
%% mirror distribution
X = zeros;
Y = zeros;
points = zeros;
%rcy = Rim_Radius - mirror_size/2; %point at center of the most outer
mirror in y-dir.
%points = -rcy : mirror_size_total : rcy; %makes a symmetrical vector with
the points to the mirrors' center points
minival = 10^-12; %a minimal distance for constructing a contnious curve,
for the plots. Is 0 in reality.
for i = 1: ((qty_y/2+0.5)*4)-2 %number of mirrors * 2 (i.e. number of
mounting points) + number of gaps; from center to edge
if i == 1
point = mirror_size/2; %inital value, one half mirror (center
mirror)
elseif mod(i-2,4) == 0 %number is 2, 6, 10, 14, ..
point = point + minival; %Add minimal distance for making
continious plot
elseif mod(i-3,4) == 0 %number is 3, 7, 11, ...
point = point + gap; % add gap
elseif mod(i-4, 4) == 0 % number is 4 , 8 , 12, ...
point = point + minival; %Add minimal distance for making
continious plot
elseif mod(i-5, 4) == 0 % number is 5 , 9 , 13, ...
point = point + mirror_size; % add space for a mirror
end
points(i) = point;
end
points = [-fliplr(points), points]; %makes symmetrical array of delta X_Y
[X, Y] = meshgrid(points, points); %makes 2 quadratic matrixes (mesh)
if strcmp(shape,'Paraboloid')
Z = X.^2 / (4*focallength) + Y.^2 / (4*focallength); % parabolic mirror
equ. will be a matrix. Z is hight of an parabola with spec. focallength and
spec. width
elseif strcmp(shape,'Spherical')
Z = 2*focallength - sqrt(4*focallength.^2 - X.^2 - Y.^2); %spherical
mirror equation (???)
else
disp ('error! Select shape!')
end
%mesh (Y_list', Y_list , X_big_matrix)
surf (X, Y, Z);
hold on
figure (1)
plot (Y, X)
plot (X, Y)
stem3 (Y, X, Z)
title ([shape, ' mirror assembly [m]; dist from tower: ', num2str(dist), '
m'])
xlabel('X')
ylabel('Y')
zlabel('Z')
hold off
%% Adjust the Z's so it becomes a fresnel mirror
brakepoint_0 = (((qty_y/2+0.5)*4 - 2) - 1)-1; %Point where the flat
center mirror starts
brakepoint_1 = (((qty_y/2+0.5)*4 - 2) - 1)+2; %Point where the flat
center mirror ends
checkpoints_array = [1: 4 : brakepoint_0, brakepoint_1-2 : 4 : length(Z)];
% this is the right corner in the ( 3x3 or 3x4 or 4x3) matrix, part of the
% Z-matrix, that specifys Z-values for a specific mirror
Z2 = zeros;
for i = checkpoints_array %Rows
for j = checkpoints_array %Columns
if j == brakepoint_0 && i == brakepoint_0 % 5 x 5 matrix
(center mirror)
group_Zvalues = [Z(i,j), Z(i+1,j), Z(i+2,j), Z(i+3,j),
Z(i+4,j); Z(i,j+1), Z(i+1,j+1), Z(i+2,j+1), Z(i+3,j+1),Z(i+4,j+1);
Z(i,j+2), Z(i+1,j+2), Z(i+2,j+2), Z(i+3,j+2),Z(i+4,j+2); Z(i,j+3),
Z(i+1,j+3), Z(i+2,j+3), Z(i+3,j+3), Z(i+4,j+3); Z(i,j+4), Z(i+1,j+4),
Z(i+2,j+4), Z(i+3,j+4), Z(i+4,j+4) ];
elseif j == brakepoint_0 && i ~= brakepoint_0 % 5 x 4 matrix
group_Zvalues = [Z(i,j), Z(i+1,j), Z(i+2,j),Z(i+3,j);
Z(i,j+1), Z(i+1,j+1), Z(i+2,j+1), Z(i+3,j+1); Z(i,j+2), Z(i+1,j+2),
Z(i+2,j+2),Z(i+3,j+2); Z(i,j+3), Z(i+1,j+3), Z(i+2,j+3),Z(i+3,j+3);
Z(i,j+4), Z(i+1,j+4), Z(i+2,j+4),Z(i+3,j+4) ];
elseif i == brakepoint_0 && j ~= brakepoint_0 % 4 x 5
matrix
group_Zvalues = [Z(i,j), Z(i+1,j), Z(i+2,j), Z(i+3,j),
Z(i+4,j); Z(i,j+1), Z(i+1,j+1), Z(i+2,j+1), Z(i+3,j+1),Z(i+4,j+1);
Z(i,j+2), Z(i+1,j+2), Z(i+2,j+2), Z(i+3,j+2),Z(i+4,j+2); Z(i,j+3),
Z(i+1,j+3), Z(i+2,j+3), Z(i+3,j+3), Z(i+4,j+3) ];
else % 4 x 4 matrix
group_Zvalues = [Z(i,j), Z(i+1,j), Z(i+2,j), Z(i+3,j);
Z(i,j+1), Z(i+1,j+1), Z(i+2,j+1), Z(i+3,j+1); Z(i,j+2), Z(i+1,j+2),
Z(i+2,j+2), Z(i+3,j+2); Z(i,j+3), Z(i+1,j+3), Z(i+2,j+3), Z(i+3,j+3) ];
end
% operations
Z_group_min = min (min (group_Zvalues)); %extract the total group
min
group_Zvalues = group_Zvalues - Z_group_min ; %Subtract the lowest
value from all elements. I.e. the mirror will rest on its lowest corner,
and angles maintained.
%Put values into Z2
[M , N] = size(group_Zvalues); %M for rows, N for columns
for k = 1: N
for l = 1:M
Z2 (j+l-1, i+k-1) = group_Zvalues (l, k);
 
end
end
end
end
[N1, M1] = size(Z2); %size of the matrix, rows, columns
% Set so gap is on 0-level
%
for i = 1 : 4 : brakepoint_0
for j = 1 : 4 : brakepoint_0 %first half
Z2 (1:N1, j) = 0; %whole row = 0
Z2 (i, 1:M1) = 0; %whole column = 0
Z2 (1:N1, i) = 0; %whole row = 0
Z2 (j, 1:M1) = 0; %whole column = 0
end
for j = brakepoint_1+1 : 4 : N1 %second half
Z2 (1:N1, j) = 0;
Z2 (i, 1:M1) = 0;
Z2 (1:N1, i) = 0;
Z2 (j, 1:M1) = 0;
end
end
for i = 4 : 4 : brakepoint_0+4
for j = 4 : 4 : brakepoint_0+4
Z2 (1:N1, j) = 0;
Z2 (i, 1:M1) = 0;
Z2 (1:N1, i) = 0;
Z2 (j, 1:M1) = 0;
end
for j = brakepoint_1-2 : 4 : N1-2 %brakepoint_1+1 : 4 : N1
Z2 (1:N1, j) = 0;
Z2 (i, 1:M1) = 0;
Z2 (1:N1, i) = 0;
Z2 (j, 1:M1) = 0;
end
end
% Make a matrix X2, easier to read for design purposes
X2 = X; %Copy matrix X.
%
for i = 1 : 4 : brakepoint_0
for j = 1 : 4 : brakepoint_0 %first half
X2 (1:N1, j) = 0; %whole row = 0
X2 (i, 1:M1) = 0; %whole column = 0
X2 (1:N1, i) = 0; %whole row = 0
X2 (j, 1:M1) = 0; %whole column = 0
end
for j = brakepoint_1+1 : 4 : N1 %second half
X2 (1:N1, j) = 0;
X2 (i, 1:M1) = 0;
X2 (1:N1, i) = 0;
X2 (j, 1:M1) = 0;
end
end
for i = 4 : 4 : brakepoint_0+4
for j = 4 : 4 : brakepoint_0+4
X2 (1:N1, j) = 0;
X2 (i, 1:M1) = 0;
X2 (1:N1, i) = 0;
X2 (j, 1:M1) = 0;
end
for j = brakepoint_1-2 : 4 : N1-2 %brakepoint_1+1 : 4 : N1
X2 (1:N1, j) = 0;
X2 (i, 1:M1) = 0;
X2 (1:N1, i) = 0;
X2 (j, 1:M1) = 0;
end
end
Y2 = X2';
%% plots fresnel
s = Rim_Diam *1.3 ; %Scale factor
figure (2);
surf (X, Y, Z2);
hold on
plot3 ([0 0], [0 0], [0 (s/900)], 'LineWidth',3, 'Color','black') %plot a
normal curve at center mirror
plot (Y, X) %plot the ground grid
plot (X, Y)
stem3 (Y, X, Z2) %plot stems
title (['Fresnel mirror assembly [m]; based on the shape of ', shape, '
mirror; dist from tower: ', num2str(dist), ' m'])
xlabel('X')
ylabel('Y')
zlabel('Z2')
hold off
% -------------
figure (3); %3d-plot in scale
surf (X, Y, Z2);
axis equal
hold on
plot3 ([0 0], [0 0], [0 (s/900)], 'LineWidth',3, 'Color','black') %plot a
normal curve at center mirror
plot (Y, X) %plot the ground grid
plot (X, Y)
stem3 (Y, X, Z2) %plot stems
title (['Fresnel mirror assembly [m]; based on the shape of ', shape, '
mirror; dist from tower: ', num2str(dist), ' m; equal scale of the axes'])
xlabel('X')
ylabel('Y')
zlabel('Z2')
hold off
% -------------
%
figure (4) % 2D-PLOT
plot (Y(:,1), Z2(:,2), 'x -') %plot outer profile
hold on
plot (Y(:,1), Z2(:,(brakepoint_0+2)), 'o -', 'color' , 'red')
%
%axis ([-s s 0 s/500])
title (['Horizonthal profile [m]; dist from tower: ', num2str(dist), '
m'])
xlabel('X')
ylabel('Z2')
hold off
