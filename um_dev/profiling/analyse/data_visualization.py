# Load and visualize the profiling results
# Yuting Xie
# 2022.3.15

import numpy as np
import matplotlib.pyplot as plt
import os

COLORS = ['blue', 'yellowgreen', 'brown',
          'dimgray', 'deeppink', 'gold',
          'black', 'salmon', 'violet',
          'coral', 'red', 'peru',
          'plum', 'orchid', 'mediumturquoise',]

MARKERS = ['.', '^', '*', 'D', 'o', ',', '_', 's', 'p']

# Set matplotlib to use SciencePlots
plt.style.use(['science','ieee'])

def load_data_from_file(file_path, results):
  with open(file_path) as f:
    for line in f.readlines():
      line = line.strip()
      if len(line) == 0:
        continue
      _, task, time = line.split()
      task = task.strip(':')
      time = float(time.strip('ms'))
      if task in results:
        results[task].append(time)
      else:
        results[task] = [time]

def load_data_from_dir(dir_path):
  results = {}
  for root, dirs, files in os.walk(dir_path):
    for file in files:
      file = os.path.join(root, file)
      load_data_from_file(file, results)
  return results

def plot_data(results):
  idx = 0
  for task_name, data_list in results.items():
    data = np.array(data_list)
    # data = np.log(data)
    plt.plot(range(len(data_list)), data,
             # color=COLORS[idx % len(COLORS)],
             label=task_name,
             # marker=MARKERS[idx % len(MARKERS)],
             # linewidth=0.8,
             # markersize=0.8
             )
    plt.ylabel("Latency(ms)")
    plt.xlabel("No.")
    idx += 1
  plt.legend()
  plt.axis([0, 100, 0, 80])
  plt.savefig("result.png")
  plt.show()

if __name__ == "__main__":
  results = load_data_from_dir("/home/tt/Codes/apollo/um_dev/profiling/results/lgsvl_radar")
  plot_data(results)
