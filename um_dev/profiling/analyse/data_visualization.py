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

def load_data_from_file(file_path):
  result = {}
  with open(file_path) as f:
    for line in f.readlines():
      line = line.strip()
      if len(line) == 0:
        continue
      _, task, time = line.split()
      task = task.strip(':')
      time = float(time.strip('ms'))
      if task in result:
        result[task].append(time)
      else:
        result[task] = [time]
  return result

def load_data_from_dir(dir_path):
  results = []
  for root, dirs, files in os.walk(dir_path):
    for file in files:
      file = os.path.join(root, file)
      result = load_data_from_file(file)
      if len(result) > 0:
        results.append(result)
  return results

def plot_data(results):
  idx = 0
  for result in results:
    for task_name, data_list in result.items():
      data = np.array(data_list)
      # data = np.log(data)
      plt.plot(range(len(data_list)), data,
               color=COLORS[idx % len(COLORS)],
               label=task_name,
               marker=MARKERS[idx % len(MARKERS)],
               linewidth=0.8,
               markersize=0.8)
      plt.ylabel("Latency(ms)")
      plt.xlabel("No.")
      idx += 1
  plt.legend()
  plt.axis([0, 300, 0, 80])
  plt.savefig("result.png")
  plt.show()

if __name__ == "__main__":
  results = load_data_from_dir("/home/tt/Codes/apollo/um_dev/profiling/results/lgsvl")
  plot_data(results)
