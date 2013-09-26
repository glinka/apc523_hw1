#include "ValuedGrid.h"

template <typename T>
ValuedGrid<T>::ValuedGrid(const double max_x, const double min_x, const double max_y, const double min_y, const int n_xpts, const int n_ypts): x_inc((max_x-min_x)/n_xpts), y_inc((max_y-min_y)/n_ypts), n_xpts(n_xpts), n_ypts(n_ypts) {
    for(int i = 0; i < n_ypts; i++) {
	values.push_back(std::vector<T>());
    }
	    
};

template <typename T>
std::vector<double> ValuedGrid<T>::next_point() {
    std::vector<double>
	current_pt = 1;
    }
    int x = (current_pt - current_pt%n) / n;
    int y = current_pt%n;
    current_pt++;
    return {x, y};
}

//don't forget to init static vars in main.cc with the following terrible syntax:
/**
template <typename T> 
int ValuedGrid<T>::current_pt = 0;
**/
