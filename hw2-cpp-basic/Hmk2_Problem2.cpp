/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  09/22/2019

Description:    This program takes as a command line argument the name of a data file.
                The data file contains a M x N grid of numbers. The program reads
                in the data file and determines the largest product of four adjacent 
                numbers in the same direction (up, down, left, right, or diagonally). 
                Output is written to output2.txt.
*/

#include <iostream>
#include <fstream>
#include <assert.h> // for checking if the input data file name has been specified
using namespace std;

int main( int argc, char** argv )
{
    int nrow, ncol, prod;
    int maxProd = 0;

    assert( argc >= 2 );    // for checking if the input data file name has been specified
    ifstream fid ( argv[1] );
    
    // read the matrix row, col number and values from file
    fid >> nrow;
    fid >> ncol;
    int mat[nrow][ncol];
    for ( int i = 0; i < nrow; i++ ) {
        for ( int j = 0; j < ncol; j++ ) {
            fid >> mat[i][j];
        }
    }
    fid.close();

    // products along columns
    for ( int j = 0; j < ncol; j++ ) {
        for ( int i = 0; i < nrow - 4 + 1; i++ ) {
            prod = mat[i][j] * mat[i+1][j] * mat[i+2][j] * mat[i+3][j];
            if ( prod > maxProd ) {
                maxProd = prod;
            }
        }
    }

    // products along rows
    for ( int i = 0; i < nrow; i++ ) {
        for ( int j = 0; j < ncol - 4 + 1; j++ ) {
            prod = mat[i][j] * mat[i][j+1] * mat[i][j+2] * mat[i][j+3];
            if ( prod > maxProd ) {
                maxProd = prod;
            }
        }
    }

    // products along diagonal
    for ( int i = 0; i < nrow - 4 + 1; i++ ) {
        for ( int j = 0; j < ncol - 4 + 1; j++ ) {
            prod = mat[i][j] * mat[i+1][j+1] * mat[i+2][j+2] * mat[i+3][j+3];
            if ( prod > maxProd ) {
                maxProd = prod;
            }
        }
    }

    // products along anti diagonal
    for ( int i = 0; i < nrow - 4 + 1; i++ ) {
        for ( int j = 4-1; j < ncol; j++ ) {
            prod = mat[i][j] * mat[i+1][j-1] * mat[i+2][j-2] * mat[i+3][j-3];
            if ( prod > maxProd ) {
                maxProd = prod;
            }
        }
    }

    // output to file
    ofstream fod;
    fod.open ("output2.txt");
    fod << maxProd;
    fod.close();

    return 0;
}
