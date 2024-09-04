import PySimpleGUI as sg

text = sg.popup_get_text("What's your name?")
sg.popup(f'You entered {text}')

filename = sg.popup_get_file('Please enter a filename')
sg.popup(f'You entered {filename}')

folder = sg.popup_get_folder('Choose a folder')
sg.popup(f'You entered {folder}')

date = sg.popup_get_date()
sg.popup(f'You entered {date}')
