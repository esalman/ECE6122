/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  08/27/2019

Description:    This program takes a \beta (double) and the number of 
                subdivisions, N (unsigned long) and numerically calculate the 
                value of the following integral using the midpoint rule:
                \int_{0}^{\beta} \frac{4}{ (1+x^2) } dx
*/

#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
using namespace std;

int main()
{
    double beta, x, dx, fx, sum;
    unsigned long N;
    int precision = 16;
    
    // loop until the input (beta) is a valid double
    cout << "Please enter a value for the upper limit (beta): ";
    while ( ! ( cin >> beta ) )
    {
        // handle non-integer inputs
        cout << "Please enter a valid double: ";
        cin.clear();
        // ignore any input up to new line character
        cin.ignore( numeric_limits<streamsize>::max(), '\n' );
    }
    // loop until the input (N) is a valid  integer
    cout << "Please enter the number of subdivisions to use (N): ";
    while ( ! ( cin >> N ) || N < 1 )
    {
        // handle non-integer inputs
        cout << "Please enter a valid positive integer: ";
        cin.clear();
        // ignore any input up to new line character
        cin.ignore( numeric_limits<streamsize>::max(), '\n' );
    }
    
    // handle when the input beta is zero
    if ( beta == 0 ) 
    {
        beta = 1e-9;
    }

    // implemtation: approximation of midpoint rule
    // \sum_{1}^{N} F(x_i) \times \delta x
    dx = beta / N;
    sum = 0;
    for ( x = dx/2; x <= beta; x += dx )
    {
        fx = 4 / ( 1 + pow(x, 2) );
        sum += fx * dx;
    }
    
    // set cout precision
    std::cout << std::fixed << std::showpoint;
    std::cout << std::setprecision( precision );
    
    // output
    cout << "The integer evaluates to: " << sum << endl;

}