from tkinter import Tk 

PROGRAM_NAME = 'Explosion Drum Machine'

class DrumMachine:

    def __init__(self, root):
        self.root = root
        self.root.title(PROGRAM_NAME)


if __name__ =='__main__':
    root = Tk()
    DrumMachine(root)
    root.mainloop()