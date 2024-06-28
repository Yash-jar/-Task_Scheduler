import csv
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt

def visualize_schedule_from_csv(file_path):
    # Read the schedule data from the CSV file
    with open(file_path, 'r') as csvfile:
        schedule_reader = csv.reader(csvfile)
        assign_tasks = [[int(task) for task in row] for row in schedule_reader]

    # Create a heatmap of task assignments using Seaborn
    sns.set(style="whitegrid")
    plt.figure(figsize=(10, 6))
    sns.heatmap(assign_tasks, cmap='Blues', annot=True, fmt='d', cbar=True)

    plt.title('Task Assignments to Machines Over Time')
    plt.xlabel('Time Slots')
    plt.ylabel('Machines')

    plt.tight_layout()
    plt.show()

# Example usage
visualize_schedule_from_csv('schedule.csv')