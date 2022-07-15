/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  09/7/2019

Description:    This program takes the number of knights, n (unsigned long),
                then determines if the rules (two knights can only sit next to 
                each other if their respective numbers sum to a Fibonacci 
                number) can be satisfied. If a solution is found, then it shows
                the solution in a clockwise fashion starting with the king. If 
                no solution is possible then output “No solution found!” is 
                displayed.
*/

#include <algorithm>
#include <ctime>
#include <iostream>
#include <list>
#include <math.h>
#include <vector>
using namespace std;

/*
findFibonacciBetween: find Fibonacci numbers between two input integers, using 
    the rule that if x is a Fibonacci number, then either of 5x^2\pm4 
    must be a perfect square.

    Arguments:  
    int x, y:           the two intergers 
    Output:
    vector<int> vect:   passed by reference, contains the Fibonacci numbers 
                        between x and y
*/
void findFibonacciBetween( vector<int> &vect, int x, int y ) {
    int num1, num2, root1, root2;
    for ( int i = x; i <= y; i++ ) {
        num1 = 5 * i * i + 4;
        num2 = 5 * i * i - 4;
        root1 = round( sqrt( num1 ) );
        root2 = round( sqrt( num2 ) );
        if ( num1 == (root1*root1) || num2 == (root2*root2) ) {
            vect.push_back( i );
        }
    }
}

int main() {
    // for measureing performance
    clock_t begin = clock();

    bool cond, debug, found;
    int n, temp1, temp2;
    vector<int> fibs;
    vector< vector<int> > seqs, tempSeqs;

    // setting debug=true will output a lot of comments and hence take time
    debug = false;
    
    // input handling block
    if ( !debug )
    {
        cout << "Please enter the number of knights: ";
        // loop until the input is a valid integer
        while ( ! ( cin >> n ) || n < 1 )
        {
            // handle non-integer inputs
            cout << "Please enter a valid positive integer > 1: ";
            cin.clear();
            // ignore any input up to new line character
            cin.ignore( numeric_limits<streamsize>::max(), '\n' );
        }
    }
    else
    {
        n = 13;
    }
        
    found = false;
    // find valid sequences which begins with current_n, starting with max n. 
    // decrement current_n if not found
    for ( int current_n = n; current_n > 1; current_n-- )
    {
        if ( debug ) cout << "iterations for sequences starting with " << current_n << endl;
        // store valid sequences starting with current_n in a dynamic vector of 
        // integer vectors
        seqs.clear();
        seqs.push_back( {current_n} );
        
        // this loop will run as long as there's a chance a valid sequence 
        // starting with current_n exists
        cond = false;
        while ( !cond ) 
        {
            // use another temprary vector to store new sequences
            tempSeqs.clear();
            // loop through valid sequences found in previous step
            for ( int i = 0; i < seqs.size(); i++ ) 
            {
                if ( debug ) 
                {
                    cout << "current sequence: ";
                    for ( auto it = seqs[i].begin(); it != seqs[i].end(); it++ ) cout << *it << " ";
                    cout << endl;
                }
                
                // get last element of the sequence
                temp1 = seqs[i].size() - 1;
                temp2 = seqs[i][ temp1 ];
                
                // find all Fibonacci numbers between the last element and n
                fibs.clear();
                findFibonacciBetween( fibs, temp2+1, temp2+n );
                if ( debug ) 
                {
                    cout << "for x+" << temp2 << ", the fibonacci numbers between " << temp2+1 << " and " << temp2+n << " are: " << endl;
                    for ( auto it = fibs.begin(); it != fibs.end(); it++ ) cout << *it << " ";
                    cout << endl;
                }                

                // the adjacent number must be the one of the Fibonacci numbers 
                // less the last element. check all possible numbers for adjacency
                for ( int j = 0; j < fibs.size(); j++ ) 
                {
                    temp1 = fibs[j] - temp2;
                    tempSeqs.push_back( seqs[i] );
                    if ( find( seqs[i].begin(), seqs[i].end(), temp1 ) == seqs[i].end() ) 
                    {
                        tempSeqs[ tempSeqs.size() - 1 ].push_back( temp1 );
                    }
                    else 
                    {
                        tempSeqs.pop_back();
                    }
                }
            }
            
            // update the sequences
            seqs = tempSeqs;
            if ( debug ) {
                cout << "updated sequences: " << endl;
                for ( int i = 0; i < seqs.size(); i++ ) {
                    for ( int j = 0; j < seqs[i].size(); j++ )
                        cout << seqs[i][j] << " ";
                    cout << endl;
                }
            }            

            // stopping conditions
            // if there' no new sequence, move on
            if ( seqs.size() == 0 ) cond = true;
            //  if the sequence length is equal to n, stop
            if ( seqs[0].size() == n ) found = true;
            if ( cond || found ) break;
        }
        if ( found == true ) break;
    }

    // output
    if ( found == true )
    {
        cout << "The knights should sit clockwise as follows: ";
        for ( auto it = seqs[0].begin(); it != seqs[0].end(); it++ ) cout << *it << " ";
        cout << endl;
    }
    else
    {
        cout << "No solution found!" << endl;
    }

    // output the time elapsed
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << "Elapsed time: " << elapsed_secs << "s" << endl;
    
}
