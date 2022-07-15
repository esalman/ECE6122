#include <algorithm>
#include <ctime>
#include <iostream>
#include <list>
#include <math.h>
#include <vector>
using namespace std;

void find_fibonacci_between( vector<int> &vect, int x, int y ) {
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
    clock_t begin = clock();
    bool cond;
    int found, n, temp1, temp2;
    int debug = 0;
    vector<int> fibs;
    vector< vector<int> > seqs, tempSeqs;
    
    if ( debug )
    {
        n = 7;
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

    found = 0;
    for ( int current_n = n; current_n > 0; current_n-- ) {
        cout << "iterations for sequences starting with " << current_n << endl;
        seqs.clear();
        seqs.push_back( {current_n} );
        
        cond = false;
        while ( !cond ) {
            tempSeqs.clear();
            for ( int i = 0; i < seqs.size(); i++ ) {
                // cout << "current sequence: ";
                // for ( auto it = seqs[i].begin(); it != seqs[i].end(); it++ ) cout << *it << " ";
                // cout << endl;

                temp1 = seqs[i].size() - 1;
                temp2 = seqs[i][ temp1 ];
                
                fibs.clear();
                find_fibonacci_between( fibs, temp2+1, temp2+n );
                // cout << "for x+" << temp2 << ", the fibonacci numbers between " << temp2+1 << " and " << temp2+n << " are: " << endl;
                for ( int j = 0; j < fibs.size(); j++ ) {
                    // cout << fibs[j] << " " << endl;

                    temp1 = fibs[j] - temp2;
                    tempSeqs.push_back( seqs[i] );
                    if ( find( seqs[i].begin(), seqs[i].end(), temp1 ) == seqs[i].end() ) {
                        tempSeqs[ tempSeqs.size() - 1 ].push_back( temp1 );
                    }
                    else {
                        tempSeqs.pop_back();
                    }
                }
                // cout << endl;
            }
            
            seqs = tempSeqs;
            // cout << "updated sequences: " << endl;
            // for ( int i = 0; i < seqs.size(); i++ ) {
            //     for ( int j = 0; j < seqs[i].size(); j++ )
            //         cout << seqs[i][j] << " ";
            //     cout << endl;
            // }

            if ( seqs.size() == 0 ) {
                cond = true;
            }

            if ( seqs[0].size() == n ) {
                found = 1;
            }

            if ( cond || found ) break;
            
        }
        if ( found == 1 ) break;
        // cout << endl;
    }

    if ( found == 1 )
    {
        cout << "The knights should sit clockwise as follows: ";
        for ( auto it = seqs[0].begin(); it != seqs[0].end(); it++ ) cout << *it << " ";
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
