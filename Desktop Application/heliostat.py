from tkinter import Tk, Menu, Label
import math

# Constants
PROGRAM_NAME = 'Heliostat Sun Tracking Control Program'
TARGET_GROUP = 1
MACHINE_NUMBER = 1
MAXIMUM_MACHINE_ALTITUDE = 180
MAXIMUM_MACHINE_AZIMUTH = 180


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

    


    def create_heliostat_position(self):
        print("Various positions of the Heliostat")


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