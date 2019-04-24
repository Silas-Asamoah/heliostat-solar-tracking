import tkinter as tk 
from tkinter import ttk 
from tkinter import Menu

import calendar

#Create Instance
win = tk.Tk()
win.title("Sun Tacking Heliostat Programme")

# a_label = ttk.Label(win, text='Heliostat Calculations').grid(column=0, row=0)

'''
Machine Number Target Group === DropDown of Numbers
Sun's Altitude Sun's Azimuth
Machine's Altitude Machine's Azimuth
Target's ALtitude Target's Azimuth

Select COM Port -- COM 9
Connect to Arduino 

'''
# Menu Bar
menuBar = Menu(win)
win.config(menu = menuBar)


#Tool Menu
toolMenu = Menu(menuBar)
toolMenu.add_command(label="Tools")

#Help Menu
helpMenu = Menu(menuBar)
helpMenu.add_command(label='Help')

#Adding a button

# action.grid(column=1, row=1)

def arduinoConnect():
    print("Connection to Arduino Successful")


def clickMe():
    action.configure(text='Hello ' + name.get())


action = ttk.Button(win, text="Click Me!", command = clickMe)

#Textbox Entry Widget
name = tk.StringVar()
nameEntered = ttk.Entry(win, width=12, textvariable=name)
nameEntered.grid(column=0, row =1)
nameEntered.focus()

action.grid(column=2, row=1)

#Adding a Label
ttk.Label(win, text="Enter a name:").grid(column=0, row = 0)

ttk.Label(win, text='Choose a number:').grid(column=1, row=0)
number = tk.StringVar()
numberChosen = ttk.Combobox(win, width=12, textvariable=number)
numberChosen['values'] = (1, 2, 4, 42, 100)
numberChosen.grid(column=1, row=1)
numberChosen.current(3)


#Machine Number
machine_label = ttk.Label(win, text='Machine Number')
machine_number = tk.StringVar()
machineChosen = ttk.Combobox(win, width=12, textvariable=machine_number)
machineChosen['values'] = (1, 2, 3 ,4, 5)
machineChosen.grid(column=3, row = 0)
machineChosen.current(0)

#Target Group
target_label = ttk.Label(win, text='Target Group')
target_group = tk.StringVar() 
targetChosen = ttk.Combobox(win, width=12, textvariable=target_group)
targetChosen['values'] = (1, 2, 3, 4, 5)
targetChosen.grid(column = 4, row = 1)

#Sun Altitude
sun_label_one = ttk.Label(win, text='Sun Altitude')
sun_altitude = tk.StringVar()
# sunAltitudeChosen


#Sun Azimuth
sun_label_two = ttk.Label(win, text="Sun Azimuth")
sun_azimuth = tk.StringVar()

# Machine Altitude
mach_alt_label = ttk.Label(win, text="Machine Altitude")
machine_one = tk.StringVar()

# Machine Azimuth
mach_azi_label = ttk.Label(win, text="Machine Azimuth")
machine_two = tk.StringVar()

# Target Altitude
target_alt_label = ttk.Label(win, text="Target Altitude")
target_one = tk.StringVar()

# Target Azimuth
target_azi_label = ttk.Label(win, text='Target Azimuth')
target_two = tk.StringVar()



#Arduino COM PORT
com_port_label = ttk.Label(win, text='Select COM Port')
com_port_number = tk.StringVar()
portChosen = ttk.Combobox(win, width=12, textvariable = com_port_number)
portChosen['values'] = ('COM9', 'COM10', 'COM11', 'COM12')
portChosen.grid(row = 0, column = 5)
com_action = ttk.Button(win, text='Connect to Arduino', command='arduinoConnect')


# --------------------------------------------------------------------------------------------------------
# RHS

def startSimulation():
    print("Start Simulation of the Heliostat")

# Year Month Day Hour Longitude latitude time zone 


#Simulation button
simulStart = ttk.Button(win, text="Activate Simualation Mode", command = startSimulation)

#CALENDAR
# YEAR
year_label = ttk.Label(win, text='Year')
year_number = tk.StringVar()
yearChosen = ttk.Combobox(win, width=12, textvariable= year_number)
yearChosen['values'] = (2015, 2016, 2017 ,2018, 2019)
yearChosen.grid()
yearChosen.current(0)


# MONTH
month_label = ttk.Label(win, text='Month')
month_number = tk.StringVar()
monthChosen = ttk.Combobox(win, width=12, textvariable = month_number)

# Getting month names
for month_idx in range(0, 12):
    heliostat_month = calendar.month_name[month_idx]

monthChosen['values'] = heliostat_month
monthChosen.grid()
monthChosen.current(heliostat_month[0])

# DAY
day_label = ttk.Label(win, text='Day')
day_number = tk.StringVar()
dayChosen = ttk.Combobox(win, width=12, textvariable=day_number)
dayChosen['values']
dayChosen.grid()
dayChosen.current()


# Heliostat MisAlignment
helio_azi_error = ttk.Label(win, text="Heliostat Azimuth MisAlignment")
helio_alt_error = ttk.Label(win, text='Heliostat Altitude MisAlignment')

#Target Orientation
target_distance = ttk.Label(win, text='Distance to Target')
alt_orientation = ttk.Label(win, text='Targets Alt Orientation')
azi_orientation = ttk.Label(win, text='Targets Azi Orientation')

move_target = ttk.Button(win, text='Move/Rotate Target')

#Longtiude Latitude TimeZone


win.mainloop()