% Hollow beam thickness calculator
% Calculates the bending moment resistance as function of inner diameter and outer diameter,
% while cross section area is constant.
%Numerical study
% Credit goes to Nils Björkman
%Axial forces only gilty for walls bigger than D/t > 750
clear all
clc
%Input material data
rho = 7880; %[kg/m2] density of material, e.g. steel
sigma = 167*10^6; % [MPa] Yield strength of high quality steel
my = 0.3; %Poison’s ratio
E = 210e9; % [Pa] Young's modulus of elasticity
%length of pipe, free length
L = 2; % [m]
n =1; % start loop
while n > 0 %infinity loop - restarts program after output
% Beam profile
m = input ('EXIT CTRL+C; Mass of beam [kg/m] = ');
V = m/rho; %[m3] Volume of one meter beam
A = V; % [m2] cross section area of a solid cylinder with weight m
D_solid_circular_cylinder = (4*A/pi).^0.5; % [m] solid cylinder diameter
disp ('Outer diameter of solid circular cylinder [mm]: ' )
disp (D_solid_circular_cylinder * 1000); %display in mm
Wb_solid_circular_cylinder = pi*D_solid_circular_cylinder.^3/32 %Bending
resistance of solid cylinder [m3]
%% Calculator
i = 0;
D = zeros; % [m] outer diameter of pipe, inital value
d = zeros; %[m] inner diameter of pipe, inital value
W_b = zeros;
W_v = zeros;
t = zeros;
n = input ('Max diameter of pipe in analysis [m] : ');
for i = 1:n*1000
%% 1. circular pipe
d(i) = i/1000; %the hole in the pipe increses with 1 mm for each loop,
the outer diameter increses too, so that the cross-section area is
constant.
D(i) = (4*A/pi + d(i).^2).^0.5; %outer diameter of the pipe based on
constant area A
W_b(i) = (pi/32)*(D(i).^4-d(i).^4)/D(i); %Bending resitance of hollow
cylinder [m3]
W_v(i) = (pi/(16*D(i)))*(D(i).^4-d(i).^4); %Torque resitance of hollow
cylinder [m3]
t(i) = (D(i)-d(i))/2;
% yield strength of material
sigma_shell_yield(i) = sigma; %Constant given by material
properies
%Buckling of pipe's walls
if (D(i)/2)/t(i) <= 1500 % In this range, the pipe's walls will
buckle
fi = 1/16*((D(i)/2)/t(i)).^0.5 ; %Only gilty for r/t <
1500, see NASA report 1968 page 5
ypsilon(i) = 1-0.901*(1-2.718282^-fi); %Experimental values, see
NASA report 1968
sigma_shell(i) = ypsilon(i) * E / (3*(1-my.^2)).^0.5 *
(t(i)/(D(i)/2));
%sigma_shell(i) = 0.6* ypsilon(i) * E *(t(i)/(D(i)/2));
else
sigma_shell(i) = 0; %Theory is missing for thinner shells
end
%Standard single buckling of columns based on length of column
n2 = 2*L/D(i);
sigma_single_buckle(i) = E/n2.^2;
% % Select the smallest of the three allowed stresses
list_of_sigma_values (1) = sigma_shell_yield(i);
list_of_sigma_values (2) = sigma_shell(i);
list_of_sigma_values (3) = sigma_single_buckle(i);
sigma_min(i) = min(list_of_sigma_values); %Find the minimum allowed stress
end
%Calculate maximum, i.e. the largest allowed diameter
D_max = n; %[m] outer diameter max allowed pipe diameter
d_max = (-4*A/pi + D_max.^2).^0.5; %inner diameter of the pipe based on
constant area A
W_b_max = (pi/32)*(D_max.^4-d_max.^4)/D_max; %Bending resitance of
hollow cylinder [m3]
W_v_max = (pi/(16*D_max))*(D_max.^4-d_max.^4); %Torque resitance of
hollow cylinder [m3]
t_max = (D_max - d_max)/2 ;
e_max = D_max/2; % cant distance [m]
I_max = W_b_max * e_max; % moment of interia [m4]
disp (' Max bending moment circular pipe [m3] : ')
disp ( W_b_max) %type result, maximum bending moment for the given weight
and given cross section
disp (' Moment of inertia [m4] : ')
disp (I_max) %type result
disp (' @ outer diameter [m] : ')
disp (D_max) %type results
disp (' inner diameter [m] : ')
disp (d_max) %type results
disp (' pipe wall thickness [mm] : ')
disp (t_max*1000) %type results, in [mm]
disp (' -------------------- ')
figure (1)
subplot(2,1,1), plot (W_b, D, W_b, d)
hold on
title (['Maximum bending moment resistance for a circular beam, m =
',num2str(m),' kg/m'])
ylabel ('Tube diameter [m]')
xlabel ('Bending resistance, Wb [m3]')
legend('Outer diameter', 'Inner diameter')
hold off
subplot(2,1,2), plot (W_b, t)
ylabel ('Tube wall thickness [m]')
xlabel ('Bending resistance, Wb [m3]')
hold off
figure (2)
subplot(3,1,1), plot (W_v, D, W_v, d)
%plot (W_b, d)
hold on
title (['Maximum torsion resistance for a circular beam, m = ',num2str(m),'
kg/m'])
ylabel ('Tube diameter [m]')
xlabel ('Torsion resistance, Wv [m3]')
legend('Outer diameter', 'Inner diameter')
hold off
subplot(3,1,2), plot (W_v, t)
ylabel ('Tube wall thickness [m]')
xlabel ('Torsion resistance, Wv [m3]')
% ----------- Buckling strength calculation ----------------
%Area_circel_sector = (1/360)* pi*(D.^2/4 - d.^2/4);
%Area_testpiece = t.^2; % [m2] Sqaure test stripe of the pipe's wall
Maximum_force_vertical = sigma_min*A; %Force at which the test-stripe
compresses plastical vertically; F=p*A
Maximum_force_vertical_yield = sigma_shell_yield*A;
Maximum_force_vertical_shell = sigma_shell*A;
Maximum_force_vertical_buckle = sigma_single_buckle*A;
subplot(3,1,3), plot (W_v , Maximum_force_vertical)
title (['Allowed compression force (sigma_a_l_l_o_w_e_d =
',num2str(sigma/1e6),' MPa)'])
ylabel ('Allowed compression force, F [N]')
xlabel ('Torsion resistance, Wv [m3]')
figure (3)
semilogy (t, Maximum_force_vertical_yield,t, Maximum_force_vertical_shell,t
,Maximum_force_vertical_buckle)
legend( 'F_a_x_i_a_l (Yielding)', 'F_a_x_i_a_l (Local buckling)',
'F_a_x_i_a_l (Simple buckling)')
title (['Allowed axial force; (sigma_a_l_l_o_w_e_d = ',num2str(sigma/1e6),'
MPa); Pipe length ',num2str(L),' m'])
ylabel ('Allowed axial force, F [N]')
xlabel ('Tube wall thickness [m]')
figure(4)
plot (t, Maximum_force_vertical)
title (['Allowed axial force; (sigma_a_l_l_o_w_e_d = ',num2str(sigma/1e6),'
MPa)'])
ylabel ('Allowed axial force, F [N]')
xlabel ('Tube wall thickness [m]')
%% 2. Square pipe
for i = 1:n*1000 %Max diameter 10 m
d(i) = i/1000; %the hole in the pipe increses with 1 mm for each loop,
the outer diameter increses too, so that the cross-section area is
constant.
D(i) = (A+d(i).^2).^0.5; %outer diameter of the pipe based on constant
area A
W_b(i) = (D(i).^4-d(i).^4)/(6*D(i)); %Bending resistance of hollow
sqaure pipe [m3]
end
disp (' Maximum bending moment resistance [m3] : ')
MAXBENDINGMOMENT_SQUARE = max(W_b) %type result, maximum bending moment for
the given weight and given cross section
figure (5)
plot (W_b, D, W_b, d)
%plot (W_b, d)
hold on
title (['Maximum bending moment resistance for a sqaure hollow beam, m =
',num2str(m),' kg/m'])
ylabel ('Tube side [m]')
xlabel ('Bending resistance, Wb [m3]')
legend('Outer chord length', 'Hole chord length')
hold off
end
