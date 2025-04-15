import sys
import glob
import os
import pandas as pd
import matplotlib.pyplot as plt

from PyQt5.QtWidgets import (QApplication, QMainWindow, QComboBox, QWidget, QVBoxLayout, 
                           QLabel, QStatusBar, QFrame, QHBoxLayout, QSpacerItem, QSizePolicy,
                           QPushButton, QListWidget, QListWidgetItem, QInputDialog)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QFont, QPalette, QColor
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

# Set a modern style for matplotlib
plt.style.use('default')  # Use default style as base
plt.rcParams.update({
    'figure.facecolor': 'white',
    'axes.facecolor': 'white',
    'axes.grid': True,
    'grid.color': '#e0e0e0',
    'grid.linestyle': '--',
    'grid.alpha': 0.7,
    'axes.edgecolor': '#333333',
    'axes.linewidth': 1.5,
    'xtick.color': '#333333',
    'ytick.color': '#333333',
    'text.color': '#333333',
    'font.family': 'Arial'
})

# Canvas class to embed a matplotlib plot in the PyQt window.
class PlotCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        super(PlotCanvas, self).__init__(fig)

class FileItemWidget(QWidget):
    def __init__(self, filename, parent=None):
        super().__init__(parent)
        self.filename = filename
        self.layout = QHBoxLayout(self)
        self.layout.setContentsMargins(5, 2, 5, 2)
        
        # File name label
        self.name_label = QLabel(filename)
        self.name_label.setStyleSheet("""
            font-size: 12px;
            color: #000000;
        """)
        self.layout.addWidget(self.name_label)
        
        # Edit button
        self.edit_button = QPushButton("✎")
        self.edit_button.setStyleSheet("""
            QPushButton {
                padding: 4px 8px;
                border: 1px solid #cccccc;
                border-radius: 4px;
                background-color: white;
                color: #000000;
            }
            QPushButton:hover {
                background-color: #f0f0f0;
                color: #000000;
            }
        """)
        self.layout.addWidget(self.edit_button)
        
        # Set stretch factors
        self.layout.setStretch(0, 1)
        self.layout.setStretch(1, 0)

# Main application window.
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Pressure Data Visualization Tool")
        self.setGeometry(100, 100, 1200, 800)
        
        # Set the application style
        self.setStyleSheet("""
            QMainWindow {
                background-color: #f5f5f5;
            }
            QLabel {
                font-size: 14px;
                color: #000000;
            }
            QComboBox {
                padding: 8px;
                border: 1px solid #cccccc;
                border-radius: 4px;
                background-color: white;
                min-width: 200px;
                color: #000000;
            }
            QComboBox:hover {
                border: 1px solid #999999;
            }
            QComboBox QAbstractItemView {
                background-color: white;
                color: #000000;
                selection-background-color: #e0e0e0;
            }
            QPushButton {
                padding: 8px 16px;
                border: 1px solid #cccccc;
                border-radius: 4px;
                background-color: white;
                color: #000000;
            }
            QPushButton:hover {
                background-color: #f0f0f0;
                color: #000000;
            }
            QListWidget {
                border: 1px solid #cccccc;
                border-radius: 4px;
                background-color: white;
                color: #000000;
            }
            QListWidget::item {
                padding: 5px;
                color: #000000;
            }
            QListWidget::item:selected {
                background-color: #e0e0e0;
                color: #000000;
            }
            QStatusBar {
                color: #000000;
            }
        """)
        
        # Set up the central widget and layout
        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)
        self.layout = QVBoxLayout(self.central_widget)
        self.layout.setContentsMargins(20, 20, 20, 20)
        self.layout.setSpacing(15)
        
        # Create header section
        header = QFrame()
        header.setStyleSheet("""
            QFrame {
                background-color: #2c3e50;
                border-radius: 8px;
                padding: 15px;
            }
            QLabel {
                color: #ffffff;
                font-size: 18px;
                font-weight: bold;
            }
        """)
        header_layout = QHBoxLayout(header)
        title_label = QLabel("Pressure Data Visualization")
        title_label.setAlignment(Qt.AlignCenter)
        header_layout.addWidget(title_label)
        self.layout.addWidget(header)
        
        # Create main content area with horizontal layout
        content_widget = QWidget()
        content_layout = QHBoxLayout(content_widget)
        content_layout.setSpacing(20)
        
        # Left panel for file list
        left_panel = QFrame()
        left_panel.setStyleSheet("""
            QFrame {
                background-color: white;
                border-radius: 8px;
                padding: 15px;
            }
            QLabel {
                color: #000000;
            }
        """)
        left_layout = QVBoxLayout(left_panel)
        left_layout.setSpacing(10)
        
        # File list section
        file_list_label = QLabel("Available Files")
        file_list_label.setFont(QFont('Arial', 12, QFont.Bold))
        left_layout.addWidget(file_list_label)
        
        # File list
        self.file_list = QListWidget()
        self.file_list.setStyleSheet("""
            QListWidget {
                border: 1px solid #cccccc;
                border-radius: 4px;
                background-color: white;
                color: #000000;
            }
            QListWidget::item {
                padding: 5px;
                color: #000000;
            }
            QListWidget::item:selected {
                background-color: #e0e0e0;
                color: #000000;
            }
        """)
        left_layout.addWidget(self.file_list)
        
        content_layout.addWidget(left_panel, 1)  # 1/3 of the width
        
        # Right panel for plot
        right_panel = QFrame()
        right_panel.setStyleSheet("""
            QFrame {
                background-color: white;
                border-radius: 8px;
                padding: 15px;
            }
            QLabel {
                color: #000000;
            }
        """)
        right_layout = QVBoxLayout(right_panel)
        
        # File selection dropdown
        file_label = QLabel("Select Data File:")
        file_label.setFont(QFont('Arial', 12, QFont.Bold))
        right_layout.addWidget(file_label)
        
        self.dropdown = QComboBox(self)
        self.dropdown.setFont(QFont('Arial', 11))
        right_layout.addWidget(self.dropdown)
        
        # Plot canvas
        self.canvas = PlotCanvas(self, width=8, height=6)
        right_layout.addWidget(self.canvas)
        
        content_layout.addWidget(right_panel, 2)  # 2/3 of the width
        
        self.layout.addWidget(content_widget)
        
        # Add status bar
        self.statusBar = QStatusBar()
        self.setStatusBar(self.statusBar)
        self.statusBar.showMessage("Ready")
        
        # Initialize file list
        self.refresh_file_list()
        
        # Connect signals
        self.dropdown.currentTextChanged.connect(self.update_plot)
    
    def refresh_file_list(self):
        """Refresh the file list widget."""
        self.file_list.clear()
        self.dropdown.clear()

        # Get all CSV files
        script_dir = os.path.dirname(os.path.abspath(__file__))
        csv_files = glob.glob(os.path.join(script_dir, "*.csv"))
        
        for full_path in csv_files:
            # Add to dropdown
            filename = os.path.basename(full_path)
            self.dropdown.addItem(filename)
            
            # Create and add list item
            item = QListWidgetItem()
            file_widget = FileItemWidget(filename)
            
            # Connect edit button signal
            file_widget.edit_button.clicked.connect(
                lambda checked, f=filename: self.rename_file(f)
            )
            
            item.setSizeHint(file_widget.sizeHint())
            self.file_list.addItem(item)
            self.file_list.setItemWidget(item, file_widget)
        
        if csv_files:
            self.update_plot(os.path.basename(csv_files[0]))
        else:
            self.statusBar.showMessage("No CSV files found in the current directory")
    
    def rename_file(self, old_filename):
        """Rename a file."""
        new_name, ok = QInputDialog.getText(
            self, 'Rename File',
            'Enter new name (without .csv extension):',
            text=old_filename[:-4]  # Remove .csv extension
        )
        
        if ok and new_name:
            new_filename = f"{new_name}.csv"
            if new_filename != old_filename:
                try:
                    os.rename(old_filename, new_filename)
                    self.refresh_file_list()
                    self.statusBar.showMessage(f"Renamed {old_filename} to {new_filename}")
                except Exception as e:
                    self.statusBar.showMessage(f"Error renaming file: {str(e)}")
    
    def update_plot(self, filename):
        """Reads the CSV file and updates the plot."""
        self.canvas.axes.clear()
        script_dir = os.path.dirname(os.path.abspath(__file__))
        full_path = os.path.join(script_dir, filename)
        full_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), filename)
        if os.path.exists(full_path):
            try:
                # Read the CSV file using pandas
                data = pd.read_csv(full_path)
                
                # Check if the file has 'time' and 'pressure' columns
                if "time" in data.columns and "pressure" in data.columns:
                    time = data["time"]
                    pressure = data["pressure"]
                    xlabel = "Time"
                    ylabel = "Pressure"
                else:
                    # Fallback: use the first two columns
                    time = data.iloc[:, 0]
                    pressure = data.iloc[:, 1]
                    xlabel = data.columns[0] if len(data.columns) > 0 else "Time"
                    ylabel = data.columns[1] if len(data.columns) > 1 else "Pressure"
                
                # Plot the data with modern styling
                self.canvas.axes.plot(time, pressure, marker='o', linestyle='-', linewidth=2, markersize=6)
                self.canvas.axes.set_title(f"{filename} (Pressure vs Time)", fontsize=14, pad=15, color='#000000')
                self.canvas.axes.set_xlabel(xlabel, fontsize=12, color='#000000')
                self.canvas.axes.set_ylabel(ylabel, fontsize=12, color='#000000')
                self.canvas.axes.grid(True, linestyle='--', alpha=0.7)
                
                # Set tick colors
                self.canvas.axes.tick_params(colors='#000000')
                
                # Adjust layout
                self.canvas.figure.tight_layout()
                
                self.statusBar.showMessage(f"Successfully loaded {filename}")
            except Exception as e:
                self.canvas.axes.text(0.5, 0.5, f"Error reading CSV:\n{e}",
                                    horizontalalignment='center', verticalalignment='center',
                                    transform=self.canvas.axes.transAxes, fontsize=12, color='#000000')
                self.statusBar.showMessage(f"Error: {str(e)}")
        else:
            self.canvas.axes.text(0.5, 0.5, "File not found", 
                                horizontalalignment='center', verticalalignment='center',
                                transform=self.canvas.axes.transAxes, fontsize=12, color='#000000')
            self.statusBar.showMessage("File not found")
        self.canvas.draw()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
