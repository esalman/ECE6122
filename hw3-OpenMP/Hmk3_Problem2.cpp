/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  10/03/2019

Description:    This multithreaded program takes as a command line argument the 
                name of a data file. The data file contains a M x N grid of numbers. 
                The program reads in the data file and determines the largest product 
                of four adjacent numbers in the same direction (up, down, left, 
                right, or diagonally). Output is written to output2.txt.
*/

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <future>
#include <iostream>
#include <thread>
using namespace std;

// function for determining max product along columns
void productsAlongColumns( int **mat, int nrow, int ncol, promise<long long> && p )
{
    long long prod;
    long long maxProd = 0;

    for ( int j = 0; j < ncol; j++ ) {
        for ( int i = 0; i < nrow - 4 + 1; i++ ) {
            prod = mat[i][j] * mat[i+1][j];
            prod *= mat[i+2][j];
            prod *= mat[i+3][j];
            if ( prod > maxProd ) {
                maxProd = prod;
            }
        }
    }

    p.set_value( maxProd );
}
    
// function for determining max product along rows
void productsAlongRows( int **mat, int nrow, int ncol, promise<long long> && p )
{
    long long prod;
    long long maxProd = 0;

    for ( int i = 0; i < nrow; i++ ) {
        for ( int j = 0; j < ncol - 4 + 1; j++ ) {
            prod = mat[i][j] * mat[i][j+1];
            prod *= mat[i][j+2];
            prod *= mat[i][j+3];
            if ( prod > maxProd ) {
                maxProd = prod;
            }
        }
    }

    p.set_value( maxProd );
}

// function for determining max product along diagonal
void productsAlongDiagonal( int **mat, int nrow, int ncol, promise<long long> && p )
{
    long long prod;
    long long maxProd = 0;

    for ( int i = 0; i < nrow - 4 + 1; i++ ) {
        for ( int j = 0; j < ncol - 4 + 1; j++ ) {
            prod = mat[i][j] * mat[i+1][j+1];
            prod *= mat[i+2][j+2];
            prod *= mat[i+3][j+3];
            if ( prod > maxProd ) {
                maxProd = prod;
            }
        }
    }

    p.set_value( maxProd );
}

// function for determining max product along anti diagonal
void productsAlongAntiDiagonal( int **mat, int nrow, int ncol, promise<long long> && p )
{
    long long prod;
    long long maxProd = 0;

    for ( int i = 0; i < nrow - 4 + 1; i++ ) {
        for ( int j = 4-1; j < ncol; j++ ) {
            prod = mat[i][j] * mat[i+1][j-1] * mat[i+2][j-2] * mat[i+3][j-3];
            if ( prod > maxProd ) {
                maxProd = prod;
            }
        }
    }

    p.set_value( maxProd );
}

int main( int argc, char** argv )
{
    int nrow, ncol;
    long long prod, maxProd;
    long long maxProds[4];
    
    assert( argc >= 2 );    // for checking if the input data file name has been specified
    ifstream fid ( argv[1] );

    // read the matrix row, col number and values from file
    fid >> nrow;
    fid >> ncol;
    int **mat;
    mat = new int*[nrow];
    for ( int i = 0; i < nrow; i++ ) {
        mat[i] = new int[ncol];
        for ( int j = 0; j < ncol; j++ ) {
            fid >> mat[i][j];
        }
    }
    fid.close();

    // using promise<long long> to transmit result between threads
    promise<long long> p1;
    // using future to retrieve valid result
    future<long long> f1 = p1.get_future();
    // spawn a new thread which calls productsAlongColumns
    thread foo1 ( &productsAlongColumns, mat, nrow, ncol, move(p1) );
    
    promise<long long> p2;
    future<long long> f2 = p2.get_future();
    thread foo2 ( &productsAlongRows, mat, nrow, ncol, move(p2) );
    
    promise<long long> p3;
    future<long long> f3 = p3.get_future();
    thread foo3 ( &productsAlongDiagonal, mat, nrow, ncol, move(p3) );
    
    promise<long long> p4;
    future<long long> f4 = p4.get_future();
    thread foo4 ( &productsAlongAntiDiagonal, mat, nrow, ncol, move(p4) );

    // syncronize threads
    foo1.join();
    foo2.join();
    foo3.join();
    foo4.join();

    // retrieve results
    maxProds[0] = f1.get();
    maxProds[1] = f2.get();
    maxProds[2] = f3.get();
    maxProds[3] = f4.get();

    // find max between results from 4 threads
    maxProd = *max_element( maxProds, maxProds+4 );

    // destroy the matrix
    for ( int i = 0; i < nrow; i++ )
    {
        delete [] mat[i];
    }
    delete [] mat;

    // output to file
    ofstream fod;
    fod.open ("output2.txt");
    fod << maxProd << '\n';
    fod.close();

    return 0;
}
