import sys
import os
import subprocess
import time
from PySide6.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton,
    QFileDialog, QLineEdit, QTextEdit, QTableWidget, QTableWidgetItem,
    QMessageBox, QHeaderView, QSplitter
)
from PySide6.QtGui import QFont, QTextCursor, QColor
from PySide6.QtCore import Qt, QTimer


class VMMGui(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Virtual Memory Manager (VMM-II)")
        self.setGeometry(100, 100, 1200, 800)
        self.setMinimumSize(900, 600)
        self.init_ui()

    def init_ui(self):
        main_layout = QVBoxLayout(self)
        main_layout.setSpacing(10)
        main_layout.setContentsMargins(15, 15, 15, 15)

        # StyleSheet
        self.setStyleSheet("""
            QWidget {
                background-color: #1e1e1e;
                color: #ffffff;
                font-family: 'Segoe UI', sans-serif;
            }
            QPushButton {
                background-color: #0d6efd;
                border: none;
                border-radius: 6px;
                color: white;
                padding: 8px 16px;
                font-weight: bold;
            }
            QPushButton:hover {
                background-color: #0b5ed7;
            }
            QLineEdit {
                padding: 8px;
                background-color: #2d2d2d;
                border: 1px solid #3d3d3d;
                border-radius: 4px;
                color: white;
            }
            QLabel {
                color: #e1e1e1;
            }
            QHeaderView::section {
                background-color: #2d2d2d;
                border: 1px solid #3d3d3d;
                font-weight: bold;
                color: white;
                padding: 5px;
            }
            QTableWidget {
                background-color: #1e1e1e;
                alternate-background-color: #252526;
                border: 1px solid #3d3d3d;
                color: white;
            }
        """)

        # Header
        header = QLabel("Virtual Memory Manager")
        header.setAlignment(Qt.AlignCenter)
        header.setStyleSheet("""
            font-size: 28px;
            font-weight: bold;
            color: #0d6efd;
            background-color: #252526;
            padding: 15px;
            border-radius: 8px;
        """)
        main_layout.addWidget(header)

        # Splitter
        content_splitter = QSplitter(Qt.Horizontal)

        # Left panel
        left_panel = QWidget()
        left_layout = QVBoxLayout(left_panel)

        input_group = QWidget()
        input_group.setStyleSheet("background-color: #252526; border-radius: 8px; padding: 15px;")
        input_layout = QVBoxLayout(input_group)

        self.file_label = QLabel("Input File: None selected")
        browse_btn = QPushButton("📂 Browse Input File")
        browse_btn.clicked.connect(self.browse_file)

        frames_label = QLabel("Number of Frames:")
        self.frames_input = QLineEdit()
        self.frames_input.setPlaceholderText("e.g., 3")

        run_btn = QPushButton("▶ Run Simulation")
        run_btn.setStyleSheet("background-color: #28a745; font-size: 14px; padding: 10px;")
        run_btn.clicked.connect(self.run_vmm)

        input_layout.addWidget(self.file_label)
        input_layout.addWidget(browse_btn)
        input_layout.addWidget(frames_label)
        input_layout.addWidget(self.frames_input)
        input_layout.addWidget(run_btn)
        left_layout.addWidget(input_group)

        # Table
        table_group = QWidget()
        table_group.setStyleSheet("background-color: #252526; border-radius: 8px; padding: 15px;")
        table_layout = QVBoxLayout(table_group)

        table_header = QLabel("Performance Results")
        table_header.setStyleSheet("font-size: 16px; font-weight: bold; color: #0d6efd;")
        table_layout.addWidget(table_header)

        self.table = QTableWidget(3, 3)
        self.table.setHorizontalHeaderLabels(["Page Faults", "Hits", "Hit Ratio"])
        self.table.setVerticalHeaderLabels(["FIFO", "LRU", "Optimal"])
        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.table.verticalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.table.setAlternatingRowColors(True)
        self.table.setFixedHeight(240)
        table_layout.addWidget(self.table)

        left_layout.addWidget(table_group)

        # Right panel
        right_panel = QWidget()
        right_layout = QVBoxLayout(right_panel)

        output_label = QLabel("Simulation Output")
        output_label.setStyleSheet("font-size: 16px; font-weight: bold; color: #0d6efd;")
        right_layout.addWidget(output_label)

        self.output_box = QTextEdit()
        self.output_box.setReadOnly(True)
        self.output_box.setStyleSheet("""
            QTextEdit {
                background-color: #1e1e1e;
                border: 1px solid #3d3d3d;
                border-radius: 8px;
                padding: 10px;
                font-family: 'Consolas', monospace;
                font-size: 13px;
                color: #d4d4d4;
            }
        """)
        right_layout.addWidget(self.output_box)

        content_splitter.addWidget(left_panel)
        content_splitter.addWidget(right_panel)
        content_splitter.setStretchFactor(0, 1)
        content_splitter.setStretchFactor(1, 2)
        main_layout.addWidget(content_splitter)

    def browse_file(self):
        path, _ = QFileDialog.getOpenFileName(self, "Select Trace File", "", "Text Files (*.txt)")
        if path:
            self.file_label.setText(f"Input File: {path}")
            self.file_path = path

    def run_vmm(self):
        if not hasattr(self, "file_path"):
            QMessageBox.warning(self, "Error", "Please select an input file first!")
            return

        frames = self.frames_input.text().strip()
        if not frames.isdigit() or int(frames) <= 0:
            QMessageBox.warning(self, "Error", "Please enter a valid positive number of frames.")
            return

        base_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
        binary_path = os.path.join(base_dir, "vmm")

        if not os.path.exists(binary_path):
            QMessageBox.critical(self, "Error", f"Cannot find binary:\n{binary_path}\n\nBuild it using 'make' first.")
            return

        result = subprocess.run(
            [binary_path, self.file_path, frames],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        full_output = result.stdout.strip()
        self.parse_summary(full_output)

        # Animate line-by-line output
        self.lines = full_output.splitlines()
        self.output_box.clear()
        self.current_line = 0
        self.timer = QTimer()
        self.timer.timeout.connect(self.animate_output)
        self.timer.start(80)  # 80ms per line

    def animate_output(self):
        if self.current_line < len(self.lines):
            line = self.lines[self.current_line]
            cursor = self.output_box.textCursor()
            cursor.movePosition(QTextCursor.End)
            cursor.insertText(line + "\n")
            self.output_box.setTextCursor(cursor)
            self.output_box.ensureCursorVisible()
            self.current_line += 1

            # Highlight algorithm phases
            if "--- FIFO" in line:
                self.output_box.setTextColor(QColor("#00bfff"))
            elif "--- LRU" in line:
                self.output_box.setTextColor(QColor("#ffb700"))
            elif "--- Optimal" in line:
                self.output_box.setTextColor(QColor("#00ff9c"))
        else:
            self.timer.stop()

    def parse_summary(self, text):
        for line in text.splitlines():
            if line.startswith("FIFO"):
                self.fill_table(0, line)
            elif line.startswith("LRU"):
                self.fill_table(1, line)
            elif line.startswith("Optimal"):
                self.fill_table(2, line)

    def fill_table(self, row, line):
        parts = [p.strip() for p in line.split("|") if p.strip()]
        if len(parts) >= 4:
            self.table.setItem(row, 0, QTableWidgetItem(parts[1]))
            self.table.setItem(row, 1, QTableWidgetItem(parts[2]))
            self.table.setItem(row, 2, QTableWidgetItem(parts[3]))


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = VMMGui()
    window.show()
    sys.exit(app.exec())
