/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  08/23/2019

Description:    This program takes the number of knights, n (type unsigned long)
                . Then it determines if the rules (two knights can only sit next 
                to each other if their respective numbers sum to a Fibonacci 
                number) can be satisfied. If a solution is found, then output to
                the console the solution in a clockwise fashion starting with 
                the king. If no solution is possible then output “No solution 
                found!” is displayed.
                
                Notes: elapsed time on a single thread
                for n=10 is < 1s, 
                for n=11 is ~ 6s, 
                for n=12 is ~ 30s.
*/

#include <algorithm>
#include <ctime>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>
using namespace std;

int main()
{
    clock_t begin = clock();
    int n, i, s, found;
    
    cout << "Please enter the number of knights: ";
    // loop until the input is a valid integer
    while ( ! ( cin >> n ) || n < 0 )
    {
        // handle non-integer inputs
        cout << "Please enter a valid positive integer: ";
        cin.clear();
        // ignore any input up to new line character
        cin.ignore( numeric_limits<streamsize>::max(), '\n' );
    }
    
    // generate default sequence of knights 
    vector<int> seq(n);
    iota(seq.begin(), seq.end(), 1);
    
    // generate fibonacci series up to 2n
    vector<int> fibs = {1};
    do
    {
        s = fibs.back() + *(fibs.end() - 2);
        fibs.push_back( s );
    } while ( fibs.back() + *(fibs.end() - 2) < 2*n );

    // check rule
    do
    {
        found = 1;
        // loop through current permutation of sequence
        for ( auto i = ++seq.begin(); i != seq.end(); i++ )
        {
            s = *i + *(i-1);
            if ( std::find( fibs.begin(), fibs.end(), s ) == fibs.end() )
            {
                found = 0;
                break;
            }
        }
        if ( found == 1 ) break;
        // get the next permutation using algorithm library
    } while ( next_permutation( seq.begin(), seq.end() ) );

    if ( found == 1 )
    {
        cout << "The knights should sit clockwise as follows: ";
        for (int& i:seq) std::cout << i << ' ';
        cout << endl;
    }
    else
    {
        cout << "No solution found!" << endl;
    }
    
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Elapsed time: " << elapsed_secs << "s" << endl;
}

