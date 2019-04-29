from tkinter import Tk, Frame, Label, Spinbox, Entry, Menu

PROGRAM_NAME = 'Explosion Drum Machine'
MAX_NUMBER_OF_PATTERNS = 10
MAX_NUMBER_OF_DRUM_SAMPLES = 5
INITIAL_NUMBER_OF_UNITS = 4
INITIAL_BPU = 4
INITIAL_BEATS_PER_MINUTE = 240 
MAX_NUMBER_OF_UNITS = 5
MAX_BPU = 5

class DrumMachine:

    def __init__(self, root):
        self.root = root
        self.root.title(PROGRAM_NAME)
        self.beats_per_minutes = INITIAL_BEATS_PER_MINUTE
        self.all_patterns = [None] * MAX_NUMBER_OF_PATTERNS
        self.current_pattern_index = 0
        self.init_all_patterns()
        self.init_gui()

    def init_all_patterns(self):
        self.all_patterns = [
            {
                'list_of_all_files': [None] * MAX_NUMBER_OF_DRUM_SAMPLES,
                'number_of_units': INITIAL_NUMBER_OF_UNITS,
                'bpu': INITIAL_BPU,
                'is_button_clicked_list':
                self.init_is_button_clicked_list(
                    MAX_NUMBER_OF_DRUM_SAMPLES,
                    INITIAL_NUMBER_OF_UNITS * INITIAL_BPU
                )
            }
            for k in range(MAX_NUMBER_OF_PATTERNS)
        ]

    def on_pattern_changed(self):
        pass

    def on_number_of_units_changed(self):
        pass

    def on_bpu_changed(self):
        pass

    def on_open_file_button_clicked(self):
        pass

    def on_play_button_clicked(self):
        pass

    def on_loop_button_toggled(self):
        pass 

    def on_beats_per_minute_changed(self):
        pass

    def init_is_button_clicked_list(self, num_of_rows, num_of_columns):
        return [[False] * num_of_columns for x in range(num_of_rows)]

    def create_top_bar(self):
        topbar_frame = Frame(self.root, height = 25)
        topbar_frame.grid(row=0, column=12, rowspan=10, padx=5, pady=5)

        Label(topbar_frame, text='Pattern Number:').grid(row=0, column=1)
        self.pattern_index_widget = Spinbox(topbar_frame, from_=0, to=MAX_NUMBER_OF_PATTERNS - 1, width=5, command=self.on_pattern_changed)
        self.pattern_index_widget.grid(row=0, column=2)
        self.current_pattern_name_widget = Entry(topbar_frame)
        self.current_pattern_name_widget.grid(row=0 , column=3, padx=7, pady=2)

        Label(topbar_frame, text='Number of Units:').grid(row=0, column=4)
        self.number_of_units_widget = Spinbox(topbar_frame, from_=1, to=MAX_NUMBER_OF_UNITS, width=5, command=self.on_number_of_units_changed)
        self.number_of_units_widget.delete(0, 'end')
        self.number_of_units_widget.insert(0, INITIAL_NUMBER_OF_UNITS)
        self.number_of_units_widget.grid(row=0, column=5)

        Label(topbar_frame, text='BPUs:').grid(row=0, column=6)
        self.bpu_widget = Spinbox(topbar_frame, from_=1, to=MAX_BPU, width = 5, command = self.on_bpu_changed)
        self.bpu_widget.grid(row=0, column=7)
        self.bpu_widget.delete(0, 'end')
        self.bpu_widget.insert(0, INITIAL_BPU)


    def create_top_menu(self):
        self.menu_bar = Menu(self.root)
        self.file_menu = Menu(self.menu_bar, tearoff = 0)
        self.file_menu.add_command(label='Load Project')
        self.file_menu.add_command(label='Save Project')
        self.file_menu.add_separator()
        self.file_menu.add_command(label='Exit')
        self.menu_bar.add_cascade(label='File', menu=self.file_menu)
        self.about_menu = Menu(self.menu_bar, tearoff = 0)
        self.about_menu.add_command(label='About')
        self.menu_bar.add_cascade(label='About', menu=self.about_menu)
        self.root.config(menu=self.menu_bar)


    def init_gui(self):
        self.create_top_menu()
        self.create_top_bar()
        self.create_left_drum_loader()
        self.create_right_button_matrix()
        self.create_play_bar()


if __name__ =='__main__':
    root = Tk()
    DrumMachine(root)
    root.mainloop()