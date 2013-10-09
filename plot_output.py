import numpy as np
import matplotlib.pyplot as plt
import os

def get_data(filename, header_rows=1, **kwargs):
    path_to_file = os.path.realpath(filename)
    data = np.genfromtxt(path_to_file, skip_header=header_rows, **kwargs)
    if header_rows > 0:
        f = open(path_to_file, "r")
        params_str = f.readline()
        params = get_header_data(params_str)
        f.close()
        print params
        return data, params
    else:
        return data

def make_filename(base_name, params, unique_id=''):
    filename = base_name
    for key in params.keys():
        filename = filename + '_' + key + '_' + str(params[key])
    if not unique_id:
        filename = filename + '_' + unique_id
    return filename

def plot_grid(data, params):
    x_min = params['x_min']
    x_max = params['x_max']
    y_min = params['y_min']
    y_max = params['y_max']
    n = params['n']
    x_inc = (x_max-x_min)/(n+1)
    y_inc = (y_max-y_min)/(n+1)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.set_xlim((x_min, x_max))
    ax.set_ylim((y_min, y_max))
    x_range = np.linspace(x_min, x_max, n)
    y_range = np.linspace(y_min, y_max, n)
    xgrid, ygrid = np.meshgrid(x_range, y_range)
    ax.contourf(xgrid, ygrid, data)
    plt.show()

if __name__=="__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('input_files', nargs='+')
    args = parser.parse_args()
    #change after properly including header in data files
    for file in args.input_files:
        data = get_data(file, header_rows=0)
        print data
        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.scatter(data[:,2], np.reciprocal(data[:,3]))
        ax.set_xlabel("total number threads")
        ax.set_ylabel("reciprocal time to completion")
        plt.savefig(file[:-3] + "png")
