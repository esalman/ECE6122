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
#include <list>
#include <math.h>
using namespace std;

void find_fibs_between( list<int> &vect, int x, int y )
{
    int num1, num2, root1, root2, i;
    for ( i = x; i <= y; i++ )
    {
        num1 = 5 * i * i + 4;
        root1 = round( sqrt( num1 ) );
        num2 = 5 * i * i - 4;
        root2 = round( sqrt( num2 ) );
        if ( num1 == ( root1 * root1 ) || num2 == ( root2 * root2 ) )
        {
            vect.push_back( i );
        }
    }
    for ( auto it = vect.begin(); it != vect.end(); it++ ) cout << " " << *it;
    cout << endl;
}

int main()
{
    clock_t begin = clock();
    int n, start, found, i, j, k;
    int debug = 1;
    bool endIteration;
    int temp1, previousNode;
    list<int> fibs;
                    
    // declare found sequences holder
    vector< vector<int> > sequences, tempSequences;
    
    if ( debug )
    {
        n = 4;
    }
    else
    {
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
    }
    
    // start from the max knight
    for ( start = n, found = 0; start > 0; start-- )
    {
        sequences.clear();
        sequences.push_back( {start} );
        cout << "iterations for " << start << endl;

        endIteration = false;
        while ( !endIteration )
        {
            // output current sequences
            if ( debug )
            {
                cout << "current sequences:" << endl;
                for ( i = 0; i < sequences.size(); i++ )
                {
                    for ( j = 0; j < sequences[i].size(); j++ )
                    {
                        cout << sequences[i][j] << " ";
                    }
                    cout << endl;
                }
            }

            for ( i = 0; i < sequences.size(); i++ ) 
            {
                cout << "handling sequence " << (i+1) << endl;

                temp1 = sequences[i].size() - 1;
                previousNode = sequences[i][ temp1 ];
                
                cout << "for x+" << previousNode << ", get fibonacci between " << previousNode + 1 << " and " << previousNode + n << ":";
                // fibs.clear();
                find_fibs_between( fibs, previousNode + 1, previousNode + n );
                
                cout << "next possible nodes: ";
                auto it = fibs.begin();
                for ( j = 0; it != fibs.end(); it++, j++ )
                {
                    temp1 = *it - previousNode;
                    cout << temp1 << " ";
                    tempSequences.push_back( sequences[i] );
                    if ( find(fibs.begin(), fibs.end(), temp1) == fibs.end() && find(tempSequences[j].begin(), tempSequences[j].end(), temp1) == tempSequences[j].end() )
                    {
                        tempSequences[j].push_back( temp1 );
                    }
                    else tempSequences.pop_back();
                }
                cout << endl;
                
                // output current temp sequences
                if ( debug )
                {
                    cout << "current temp sequences:" << endl;
                    for ( j = 0; j < tempSequences.size(); j++ )
                    {
                        for ( k = 0; k < tempSequences[i].size(); k++ )
                        {
                            cout << tempSequences[j][k] << " ";
                        }
                        cout << endl;
                    }
                }

                if ( tempSequences.size() == 0 ) 
                {
                    endIteration = true;
                    break;
                }

                sequences = tempSequences;
                tempSequences.clear();
            }
            
        }
        
        break;
    }

    
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Elapsed time: " << elapsed_secs << "s" << endl;
}

