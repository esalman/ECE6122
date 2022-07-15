/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  08/24/2019

Description:    This program takes a number *n* from the console and outputs to 
                the console *the number of 1's that must be added* to the 
                positive integer *n* before the process ends. The process refers
                to:
                - if n=1, do nothing and the process stops,
                - if n is divisible by 7, divide it by 7,
                - otherwise add 1
*/

#include <iostream>
#include <limits>
using namespace std;

int main()
{
    int n, count, debug;
    // debug = 1 will print all the steps
    debug = 0;  
    
    // loop until the input is a valid integer
    cout << "Please enter the starting number n: ";
    while ( ! ( cin >> n ) || n <= 0 )
    {
        // handle non-integer inputs
        cout << "Please enter a valid positive integer: ";
        cin.clear();
        // ignore any input up to new line character
        cin.ignore( numeric_limits<streamsize>::max(), '\n' );
    }

    // the input is a valid integer; start the process
    count = 0;
    // if n=1, do nothing and the process stops
    while ( n != 1 )
    {
        // print all the steps for debugging
        if ( debug ) cout << n << endl;
        
        // if n is divisible by 7, divide it by 7
        if ( n % 7 == 0 )
        {
            n /= 7;
            continue;
        }
        count += 1;
        // otherwise add 1
        n += 1;
    }

    // output
    cout << "The sequence had "<< count <<" instances of the number 1 being added."<< endl;
    
}