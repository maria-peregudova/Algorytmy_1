#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;
using namespace std::chrono;

template <typename T>
bool compare(T a, T b, bool ascending){
    return ascending ? (a < b) : (a > b);
}

template <typename T>
void fix_heap(T array[], int start, int end, int i, bool ascending){
    int relative_i = i - start;
    
    int root = i;
    int left = start + (2 * relative_i + 1);
    int right = start + (2 * relative_i + 2);

    if(left <= end && compare(array[root], array[left], ascending)){
        root = left;
    }

    if(right <= end && compare(array[root], array[right], ascending)){
        root = right;
    }

    if(root != i){
        int temp = array[i];
        array[i] = array[root];
        array[root] = temp;

        fix_heap(array, start, end, root, ascending);
    }
}

template <typename T>
void heap_sort(T array[], int start, int end, bool ascending){
    int n = end - start + 1;
    if(n <= 1){
      return;
    }
    
    for(int i = start + (n / 2 - 1); i >= start; i--){
        fix_heap(array, start, end, i, ascending);
    }

    for(int i = end; i > start; i--){
        int temp = array[start];
        array[start] = array[i];
        array[i] = temp;

        fix_heap(array, start, i - 1, start, ascending);
    }
}

template <typename T>
void quick_sort(T array[], int begin, int end, bool ascending, int depthLimit = -1) {
    if(begin >= end){
        return;
    }
    if(depthLimit == 0){
        heap_sort(array, begin, end, ascending);
        return;
    }

    T pivot = array[begin + (end - begin) / 2];
    int l = begin;
    int r = end;

    while(l <= r){
        if(ascending){
            while(array[l] < pivot){
                l++;
            }
            while(array[r] > pivot){
                r--;
            }
        }
        else{
            while(array[l] > pivot){
                l++;
            }
            while(array[r] < pivot){
                r--;
            }
        }

        if(l <= r){
            T temp = array[l];
            array[l] = array[r];
            array[r] = temp;
            l++;
            r--;
        }
    }
    
    if(begin < r){
        quick_sort(array, begin, r, ascending, depthLimit > 0 ? depthLimit - 1 : depthLimit);
    }
    if(l < end){
        quick_sort(array, l, end, ascending, depthLimit > 0 ? depthLimit - 1 : depthLimit);
    }
}

template <typename T>
void intro_sort(T array[], int n, bool ascending) {
    if(n <= 1){
        return;
    }
    
    int depthLimit = 2 * std::log2(n);
    
    quick_sort(array, 0, n - 1, ascending, depthLimit);
}

template <typename T>
void merge(T left[], int left_size, T right[], int right_size, T array[], bool ascending) {
    int left_counter = 0;
    int right_counter = 0;
    int array_counter = 0;

    while(left_counter < left_size && right_counter < right_size) {
        bool condition;
        if(ascending){
            condition = (left[left_counter] <= right[right_counter]);
        }
        else{
            condition = (left[left_counter] >= right[right_counter]);
        }

        if(condition){
            array[array_counter] = left[left_counter];
            left_counter++;
        }
        else{
            array[array_counter] = right[right_counter];
            right_counter++;
        }
        array_counter++;
    }

    while(left_counter < left_size){
        array[array_counter++] = left[left_counter++];
    }
    while(right_counter < right_size){
        array[array_counter++] = right[right_counter++];
    }
}

template <typename T>
void merge_sort(T array[], int length, bool ascending) {
    if(length <= 1){
        return;
    }

    int left_size = (length / 2);
    int right_size = length - left_size;

    std::vector<T> left_vec(left_size);
    std::vector<T> right_vec(right_size);

    for(int i = 0; i < left_size; i++){
        left_vec[i] = array[i];
    }
    for(int i = 0; i < right_size; i++){
        right_vec[i] = array[i + left_size];
    }

    merge_sort(left_vec.data(), left_size, ascending);
    merge_sort(right_vec.data(), right_size, ascending);

    merge(left_vec.data(), left_size, right_vec.data(), right_size, array, ascending);
}

enum ArrayType { RANDOM, PARTIAL, REVERSE };

int** allocateTable(int rows, int cols){
    int** table = new int*[rows];
    for(int i = 0; i < rows; i++){
        table[i] = new int[cols];
    }
    return table;
}

void deallocateTable(int** table, int rows){
    for(int i = 0; i < rows; i++){
        delete[] table[i];
    }
    delete[] table;
}

void copyTable(int** source, int** dest, int rows, int cols){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; ++j){
            dest[i][j] = source[i][j];
        }
    }
}

void fillTables(int** table, int rows, int cols, ArrayType type, double sortedPercent = 0){
    static mt19937 gen(42);
    uniform_int_distribution<int> dis(1, 1000000);

    for(int i = 0; i < rows; i++){
        for (int j = 0; j < cols; ++j){
            table[i][j] = dis(gen);
        }
        if(type == REVERSE){
            sort(table[i], table[i] + cols, greater<int>());
        }
        else if(type == PARTIAL){
            int sortedCount = static_cast<int>(cols * sortedPercent);
            sort(table[i], table[i] + sortedCount);
        }
    }
}

void runExperiment() {
    const int sizes[] = {10000, 50000, 100000, 500000, 1000000};
    const double percentages[] = {0.25, 0.50, 0.75, 0.95, 0.99, 0.997};
    const int TRIALS = 100;

    std::cout << "Algorytm;Rozmiar;Typ_Tablicy;Sredni_Czas_ms" << endl;

    for(int size : sizes){
        
        struct Scenario{
            string name;
            ArrayType type;
            double p;
        };
        
        vector<Scenario> scenarios = { {"Losowa", RANDOM, 0} };
        
        for(double p : percentages){
            scenarios.push_back({to_string(p * 100) + "% posortowane", PARTIAL, p});
        }
        scenarios.push_back({"Odwrotna", REVERSE, 0});

        for(auto& sc : scenarios){
            int** baseData = allocateTable(TRIALS, size);
            int** workingData = allocateTable(TRIALS, size);

            fillTables(baseData, TRIALS, size, sc.type, sc.p);



            copyTable(baseData, workingData, TRIALS, size);
            auto start = high_resolution_clock::now();
            for(int i = 0; i < TRIALS; i++){
                merge_sort(workingData[i], size, true);
            }
            auto end = high_resolution_clock::now();
            std::cout << "MergeSort;" << size << ";" << sc.name << ";"<< duration<double, milli>(end - start).count() / TRIALS << endl;



            copyTable(baseData, workingData, TRIALS, size);
            start = high_resolution_clock::now();
            for(int i = 0; i < TRIALS; i++){
                quick_sort(workingData[i], 0, size - 1, true);
            }
            end = high_resolution_clock::now();
            std::cout << "QuickSort;" << size << ";" << sc.name << ";" << duration<double, milli>(end - start).count() / TRIALS << endl;

            

            copyTable(baseData, workingData, TRIALS, size);
            start = high_resolution_clock::now();
            for (int i = 0; i < TRIALS; i++){
                intro_sort(workingData[i], size, true);
            }
            end = high_resolution_clock::now();
            std::cout << "IntroSort;" << size << ";" << sc.name << ";" << duration<double, milli>(end - start).count() / TRIALS << endl;


            deallocateTable(baseData, TRIALS);
            deallocateTable(workingData, TRIALS);
        }
    }
}

int main() {
    runExperiment();
    return 0;
}
