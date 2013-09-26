#ifndef VALUED_GRID_H_
#define VALUED_GRID_H_
#include <vector>

//google says i should
#define DISALLOW_COPY_AND_ASSIGN(typename) \
    typename(const typename&); \
    void operator=(const typename&) 

/**
template <typename T=double>
struct ValuedPoint {
    std::vector pt;
    T val;
    set_value(T v) {
	val = v;
    }
};
**/

template <typename T>
class ValuedGrid {
 public:
    ValuedGrid(const double max_x, const double min_x, const double max_y, const double min_y, const int n_xpts=10000, const int n_ypts=10000);
    ~ValuedGrid() {};
    std::vector<double> next_point();
    void set_value(const std::vector<int> coord, const T val);
 private:
    DISALLOW_COPY_AND_ASSIGN(ValuedGrid);
    static double current_x, current_y;
    const double x_inc, y_inc;
    const int n_xpts, n_ypts;
    std::vector< std::vector<T> > values;
};
    
#endif
