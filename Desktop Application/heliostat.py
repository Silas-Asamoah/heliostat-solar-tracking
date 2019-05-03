from tkinter import Tk, Menu, Label, Frame, Button
import math
import serial
import time 
import csv

# Constants
PROGRAM_NAME = 'Heliostat Sun Tracking Control Program'
TARGET_GROUP = 1
MACHINE_NUMBER = 1
MAXIMUM_MACHINE_ALTITUDE = 180
MAXIMUM_MACHINE_AZIMUTH = 180
SIMULATE_BUTTON_CLICKED = 'green'

ser_port = serial.Serial('COM9', 9600)
ser_port.flushInput()

while True:
    try:
        ser_bytes = ser_port.readline()
        decoded_bytes = float(ser_bytes[0: len(ser_bytes) - 2].decode('utf-8'))
        print(decoded_bytes)
        with open('heliostat_data.csv', 'a') as f:
            writer = csv.writer(f, delimiter=',')
            writer.writerow([time.time(), decoded_bytes])
    except:
        print('Keyboard Interrupt')
        break



class Heliostat:

    def __init__(self, root):
        self.root = root
        self.root.title(PROGRAM_NAME)
        self.target_group = TARGET_GROUP
        self.machine_number = MACHINE_NUMBER

    def init_all_positions(self):
        self.all_positions = [
            {
                'sun_altitude',
                'sun_azimuth',
                'machine_altitude',
                'machine_azimuth',
                'target_altitude',
                'target_azimuth'
            }
            
        ]

    
    def arduino_com(self):
        pass



    def create_heliostat_position(self):
        helio_frame = Frame(self.root, height=25)
        helio_frame.grid(row=0, column=12, rowspan=4, padx=2, pady=2)
        #Distinct Label
        Label(helio_frame, text='Machine Number').grid(row = 0, column = 1)
        Label(helio_frame, text='Target Group').grid(row=0, column=2)
        Label(helio_frame, text='Sun Altitude').grid(row=1, column=1)
        Label(helio_frame, text='Sun Azimuth').grid(row=1, column=2)
        Label(helio_frame, text='Machine Altitude').grid(row=2, column=1)
        Label(helio_frame, text='Machine Azimuth').grid(row=2, column=2)
        Label(helio_frame, text='Target Altitude').grid(row=3, column=1)
        Label(helio_frame, text='Target Azimuth').grid(row=3, column=2)
        Label(helio_frame, text='Select COM Port').grid(row=5, column=1)
        
    def create_graphical_display(self):
        print("This is the body of the heliostat")

    def create_simulation_controls(self):
        simu_frame = Frame(self.root, height = 25)
        simu_frame.grid(row =0, column=13, rowspan=4, padx=2, pady=2)
        #Simulate Label
        # 2 Buttons 
        Label(simu_frame, text='Heliostat Alt Misalignment').grid(row = 0, column=1)
        Label(simu_frame, text='Heliostat Azi Misalignment').grid(row = 0, column = 1)
        Label(simu_frame, text='Distance to Target').grid(row = 0, column = 1)
        Label(simu_frame, text='Target Alt Orientation').grid(row = 0, column = 1)
        Label(simu_frame, text='Target Azi Orientation').grid(row = 0, column = 1)

        

    def create_top_menu(self):
        self.menu_bar = Menu(self.root)
        self.file_menu = Menu(self.menu_bar, tearoff = 0)
        self.file_menu.add_command(label='Tools')
        self.file_menu.add_command(label='Help')
        self.about_menu = Menu(self.menu_bar, tearoff=0)
        self.about_menu.add_command(label='About')
        self.root.config(menu=self.menu_bar)


    def init_gui(self):
        self.create_top_menu()
        self.create_heliostat_position()
        self.create_graphical_display()
        self.create_simulation_controls()


if __name__ == '__main__':
    root = Tk()
    Heliostat(root)
    root.mainloop()