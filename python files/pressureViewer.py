import sys
import glob
import os
import pandas as pd

from PyQt5.QtWidgets import QApplication, QMainWindow, QComboBox, QWidget, QVBoxLayout, QLabel
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

# Canvas class to embed a matplotlib plot in the PyQt window.
class PlotCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        super(PlotCanvas, self).__init__(fig)

# Main application window.
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("CSV Pressure Data Viewer")
        self.setGeometry(100, 100, 800, 600)
        
        # Set up the central widget and layout.
        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)
        self.layout = QVBoxLayout(self.central_widget)
        
        # Label and dropdown for CSV file selection.
        self.layout.addWidget(QLabel("Select a CSV file:"))
        self.dropdown = QComboBox(self)
        self.layout.addWidget(self.dropdown)
        
        # Create a matplotlib canvas for the plot.
        self.canvas = PlotCanvas(self, width=5, height=4)
        self.layout.addWidget(self.canvas)
        
        # Get the list of CSV files in the current directory.
        self.csv_files = self.get_csv_files()
        if self.csv_files:
            self.dropdown.addItems(self.csv_files)
            self.dropdown.currentTextChanged.connect(self.update_plot)
            # Display the first CSV file on startup.
            self.update_plot(self.dropdown.currentText())
        else:
            self.dropdown.addItem("No CSV files found")
    
    def get_csv_files(self):
        """Return a list of CSV filenames (not full paths) in the script directory."""
        script_dir = os.path.dirname(os.path.abspath(__file__))
        full_paths = glob.glob(os.path.join(script_dir, "*.csv"))
        # Just return the filenames, not the full path
        return [os.path.basename(path) for path in full_paths]
    
    def update_plot(self, filename):
        """Reads the CSV file and updates the plot."""
        self.canvas.axes.clear()
        script_dir = os.path.dirname(os.path.abspath(__file__))
        full_path = os.path.join(script_dir, filename)
        
        if os.path.exists(full_path):
            try:
                data = pd.read_csv(full_path)

                xlabel = "Time"
                ylabel = "Pressure"
                
                if "time" in data.columns and "pressure" in data.columns:
                    x = data["time"]
                    y = data["pressure"]
                elif len(data.columns) >= 2:
                    x = data.iloc[:, 0]
                    y = data.iloc[:, 1]
                    xlabel = data.columns[0]
                    ylabel = data.columns[1]
                else:
                    raise ValueError("CSV must have at least two columns")
                
                self.canvas.axes.plot(x, y, marker='o')
                self.canvas.axes.set_title(f"{filename} (Pressure vs Time)")
                self.canvas.axes.set_xlabel(xlabel)
                self.canvas.axes.set_ylabel(ylabel)
            except Exception as e:
                self.canvas.axes.text(0.5, 0.5, f"Error reading CSV:\n{e}",
                                        horizontalalignment='center', verticalalignment='center',
                                        transform=self.canvas.axes.transAxes)
        else:
            self.canvas.axes.text(0.5, 0.5, "File not found", 
                                    horizontalalignment='center', verticalalignment='center',
                                    transform=self.canvas.axes.transAxes)
        self.canvas.draw()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
