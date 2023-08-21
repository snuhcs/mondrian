///////////////////////////////////////////////////////////////////////////////
// Hungarian.h: Header file for Class HungarianAlgorithm.
// 
// This is a C++ wrapper with slight modification of a hungarian algorithm implementation by Markus Buehren.
// The original implementation is a few mex-functions for use in MATLAB, found here:
// http://www.mathworks.com/matlabcentral/fileexchange/6543-functions-for-the-rectangular-assignment-problem
// 
// Both this code and the original code are published under the BSD license.
// by Cong Ma, 2016
// 

#ifndef HUNGARIAN_HPP_
#define HUNGARIAN_HPP_

#include <iostream>
#include <vector>

namespace md {

class HungarianAlgorithm {
 public:
  static float Solve(std::vector<std::vector<float>>& DistMatrix, std::vector<int>& Assignment);

 private:
  static void assignmentoptimal(int* assignment,
                                float* cost,
                                float* distMatrix,
                                int nOfRows,
                                int nOfColumns);
  static void buildassignmentvector(int* assignment, bool* starMatrix, int nOfRows, int nOfColumns);
  static void computeassignmentcost(int* assignment, float* cost, float* distMatrix, int nOfRows);
  static void step2a(int* assignment,
                     float* distMatrix,
                     bool* starMatrix,
                     bool* newStarMatrix,
                     bool* primeMatrix,
                     bool* coveredColumns,
                     bool* coveredRows,
                     int nOfRows,
                     int nOfColumns,
                     int minDim);
  static void step2b(int* assignment,
                     float* distMatrix,
                     bool* starMatrix,
                     bool* newStarMatrix,
                     bool* primeMatrix,
                     bool* coveredColumns,
                     bool* coveredRows,
                     int nOfRows,
                     int nOfColumns,
                     int minDim);
  static void step3(int* assignment,
                    float* distMatrix,
                    bool* starMatrix,
                    bool* newStarMatrix,
                    bool* primeMatrix,
                    bool* coveredColumns,
                    bool* coveredRows,
                    int nOfRows,
                    int nOfColumns,
                    int minDim);
  static void step4(int* assignment,
                    float* distMatrix,
                    bool* starMatrix,
                    bool* newStarMatrix,
                    bool* primeMatrix,
                    bool* coveredColumns,
                    bool* coveredRows,
                    int nOfRows,
                    int nOfColumns,
                    int minDim,
                    int row,
                    int col);
  static void step5(int* assignment,
                    float* distMatrix,
                    bool* starMatrix,
                    bool* newStarMatrix,
                    bool* primeMatrix,
                    bool* coveredColumns,
                    bool* coveredRows,
                    int nOfRows,
                    int nOfColumns,
                    int minDim);
};

} // namespace md

#endif // HUNGARIAN_HPP_
