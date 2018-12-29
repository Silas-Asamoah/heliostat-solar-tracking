% Beam deflection calculator - Euler beam theory
%Numerical study
% Supported at one end resp. supported at both ends. Distributed load.
%find the length of beam given a specific load and a specific deflection
%requirment
clear all
clc
n =1;
while n > 0 %infinity loop - restarts program after output
% Beam profile
% I = 1.71e-6; % IPE100 moment of interia [m4]
I = input ('EXIT CTRL+C; I [cm4] = ');
I = I*10^-8; % cm4 to m4
%Material data
E = 210e9; % Young's modulus [Pa], e.g. steel 210e9
% g = 6; %[kg/m]
g = input (' g [kg/m] = ');
%Load per meter beam. The load is distributed over the beam in Z' direction
Fz_prim = 250; % Wind load [N/m]
Fzg_prim = 10.1*9.81; % Ev. mirror facet gravity load or gravity of other
support structure [N/m]
%Deflection requirement (= limit)
def_req = 8e-3; %[m]
%% Calculator
L_II = 0; %reset variables
L_V = 0;
i = 0;
for i = 1:100000 %Max length 100 m
Q = (Fz_prim + Fzg_prim + g) * i/1000 ; %Add forces, i: step up L with
1 mm per loop
L_II = i/1000 ; % step up L with 1 mm per loop
def_1 = (Q*L_II.^3)/(8*E*I); %A standard euler beam II calculation
% Supported at one end
if def_1 >= def_req
break %brake the loop, save the result
end
 
end
L_II %type result
i = 0;
for i = 1:100000 %Max length 100 m
Q = (Fz_prim + Fzg_prim + g) * i/1000 ; %Add forces, i: step up L with
1 mm per loop
L_V = i/1000 ; % step up L with 1 mm per loop
def_2 = (5*Q*L_V.^3)/(384*E*I); %A standard euler beam V calculation
if def_2 >= def_req
break %breake the loop, save the result
end
end
L_V %type result
end