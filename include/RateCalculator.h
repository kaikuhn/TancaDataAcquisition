#pragma once

#include <vector>
#include <algorithm>
#include <limits>

class RateCalculator {
    public:
        // add element to vector
        void addElement(double timeStamp) {
            timeContainer.push_back(timeStamp);
        }

        // calculate average and clear vector
        double calcRate() {
            // rate
            double rate = std::numeric_limits<double>::quiet_NaN();
            
            // calculate the rate
            double size = timeContainer.size();
            if (size > 10) {
                auto minEl = *std::min_element(timeContainer.begin(), timeContainer.end());
                auto maxEl = *std::max_element(timeContainer.begin(), timeContainer.end());
                rate = size/((maxEl-minEl)/std::pow(10,9));

                // clear vector
                timeContainer.clear();
            }

            return rate;
        }
    
    private:
        // vector, that containes the last event time stamps
        std::vector<double> timeContainer;

};