import tkinter as tk 
from tkinter import ttk 
from tkinter import Menu

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

toolMenu = Menu(menuBar)
toolMenu.add_command(label="Tools")
menuBar.add_cascade(label='File', menu=toolMenu)

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
sun_label = ttk.Label(win, text='Sun Altitude')
sun_altitude = tk.StringVar()
sunAltitudeChosen


#Arduino COM PORT
com_port_label = ttk.Label(win, text='Select COM Port')
com_port_number = tk.StringVar()
portChosen = ttk.Combobox(win, width=12, textvariable = com_port_number)
portChosen['values'] = ('COM9', 'COM10', 'COM11', 'COM12')
com_action = ttk.Button(win, text='Connect to Arduino', command='arduinoConnect')

win.mainloop()