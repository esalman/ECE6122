/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  09/22/2019

Description:    This file implements the members of the ECE_Matrix class.
*/

#include <fstream>
#include <iomanip>
#include <iostream>
#include "ECE_Matrix.h"
using namespace std;

// Constructors
// Default constructor
// Create a 0*0 matrix
ECE_Matrix::ECE_Matrix()
{
    rows = 0;
    cols = 0;

    matrix = new double*[rows];
    for ( int i = 0; i < rows; i++ ) 
    {
        matrix[i] = new double[cols];
    }
}

// Construct an x*x array, set all elements to y
ECE_Matrix::ECE_Matrix( const int &x, const double &v ) 
{
    rows = x;
    cols = x;

    matrix = new double*[rows];
    for ( int i = 0; i < rows; i++ ) 
    {
        matrix[i] = new double[cols];
        for ( int j = 0; j < cols; j++ ) 
        {
            matrix[i][j] = v;
        }
    }
}

// Construct an x*y array, setting all element to v
ECE_Matrix::ECE_Matrix( const int &x, const int &y, const double &v ) 
{
    rows = x;
    cols = y;

    matrix = new double*[rows];
    for ( int i = 0; i < rows; i++ ) 
    {
        matrix[i] = new double[cols];
        for ( int j = 0; j < cols; j++ ) 
        {
            matrix[i][j] = v;
        }
    }
}

// Construct a matrix from file
ECE_Matrix::ECE_Matrix( const string &fname ) 
{
    ifstream fid ( fname );    
    fid >> rows;
    fid >> cols;
    matrix = new double*[rows];
    for ( int i = 0; i < rows; i++ ) 
    {
        matrix[i] = new double[cols];
        for ( int j = 0; j < cols; j++ ) 
        {
            fid >> matrix[i][j];
        }
    }
    fid.close();
}

// destructor
ECE_Matrix::~ECE_Matrix() 
{
    for ( int i = 0; i < rows; i++ ) 
    {
        delete [] matrix[i];
    }
    delete [] matrix;
}

// Overloading member functions
// Scalar addition
ECE_Matrix ECE_Matrix::operator+( const double &num ) const 
{
    ECE_Matrix ret( rows, cols, 0.0 );
    for ( int i = 0; i < rows; i++ ) 
    {
        for ( int j = 0; j < cols; j++ ) 
        {
            ret.matrix[i][j] = matrix[i][j] + num;
        }
    }
    
    return ret;
}

// Matrix addition
ECE_Matrix ECE_Matrix::operator+( const ECE_Matrix &mat2 ) const 
{
    int t1, t2;
    int retRow = max( rows, mat2.rows );
    int retCol = max( cols, mat2.cols );
    ECE_Matrix ret( retRow, retCol, 0.0 );
    for ( int i = 0; i < retRow; i++ ) 
    {
        for ( int j = 0; j < retCol; j++ ) 
        {
            t1 = 0;
            if ( i < rows && j < cols )
                t1 = matrix[i][j];
            t2 = 0;
            if ( i < mat2.rows && j < mat2.cols )
                t2 = mat2.matrix[i][j];
            ret.matrix[i][j] = t1 + t2;
        }
    }

    return ret;
}

// Matrix += addition
ECE_Matrix& ECE_Matrix::operator+=( const ECE_Matrix &mat2 ) 
{
    // assign a matrix with zero values to *this
    *this = ECE_Matrix( mat2.rows, mat2.cols, 0.0 );
    for ( int i = 0; i < rows; i++ ) 
    {
        for ( int j = 0; j < cols; j++ ) 
        {
            matrix[i][j] += mat2.matrix[i][j];
        }
    }
}

// Scalar subtraction
ECE_Matrix ECE_Matrix::operator-( const double &num ) const 
{
    ECE_Matrix ret( rows, cols, 0.0 );
    for ( int i = 0; i < rows; i++ ) 
    {
        for ( int j = 0; j < cols; j++ ) 
        {
            ret.matrix[i][j] = matrix[i][j] - num;
        }
    }
    return ret;
}

// Matrix subtraction
ECE_Matrix ECE_Matrix::operator-( const ECE_Matrix &mat2 ) const 
{
    int t1, t2;
    int retRow = max( rows, mat2.rows );
    int retCol = max( cols, mat2.cols );
    ECE_Matrix ret( retRow, retCol, 0.0 );
    for ( int i = 0; i < rows; i++ ) {
        for ( int j = 0; j < cols; j++ ) {
            t1 = 0;
            if ( i < rows && j < cols )
                t1 = matrix[i][j];
            t2 = 0;
            if ( i < mat2.rows && j < mat2.cols )
                t2 = mat2.matrix[i][j];
            ret.matrix[i][j] = t1 - t2;
        }
    }
    return ret;
}

// Matrix -= subtraction
ECE_Matrix& ECE_Matrix::operator-=( const ECE_Matrix &mat2 ) 
{
    *this = ECE_Matrix( mat2.rows, mat2.cols, 0.0 );
    for ( int i = 0; i < rows; i++ ) 
    {
        for ( int j = 0; j < cols; j++ ) 
        {
            matrix[i][j] -= mat2.matrix[i][j];
        }
    }
}

// Scalar multiplication
ECE_Matrix ECE_Matrix::operator*( const double &num ) const 
{
    ECE_Matrix ret( rows, cols, 0.0 );
    for ( int i = 0; i < rows; i++ ) 
    {
        for ( int j = 0; j < cols; j++ ) 
        {
            ret.matrix[i][j] = matrix[i][j] * num;
        }
    }
    return ret;
}

// Scalar division
ECE_Matrix ECE_Matrix::operator/( const double &num ) const 
{
    ECE_Matrix ret( rows, cols, 0.0 );
    if ( num == 0 )
        return ret;

    for ( int i = 0; i < rows; i++ ) 
    {
        for ( int j = 0; j < cols; j++ ) 
        {
            ret.matrix[i][j] = matrix[i][j] / num;
        }
    }
    return ret;
}

// Copy assignment
// Modifies *this according to the size of mat2
// Note, keeps the old values from *this. For the purpose of the assignemnt,
// mat2 is assumed to be zero.
ECE_Matrix& ECE_Matrix::operator=( const ECE_Matrix&  mat2 )
{
    // save old row/col
    int oldRows = rows;
    int oldCols = cols;
    double **oldMatrix = matrix;
    
    // determine new row/col
    rows = max( rows, mat2.rows );
    cols = max( cols, mat2.cols );

    // delete old values
    for ( int i = 0; i < oldRows; i++ ) 
    {
        delete [] matrix[i];
    }
    delete [] matrix;

    // assign empty values
    matrix = new double*[rows];
    for ( int i = 0; i < rows; i++ ) 
    {
        matrix[i] = new double[cols];
    }

    // assign new values
    for ( int i = 0; i < oldRows; i++ ) 
    {
        for ( int j = 0; j < oldCols; j++ ) 
        {
            matrix[i][j] = oldMatrix[i][j];
        }
    }

    return *this;
}

// transpose
ECE_Matrix ECE_Matrix::transpose( ) const 
{
    ECE_Matrix ret( cols, rows, 0.0 );
    for ( int i = 0; i < cols; i++ ) 
    {
        for ( int j = 0; j < rows; j++ ) 
        {
            ret.matrix[i][j] = matrix[j][i];
        }
    }
    return ret;
}

// stream insertion operation
ostream& operator<<(ostream& os, const ECE_Matrix& foo)
{
    if ( foo.rows * foo.cols == 0 )
    {
        os << "[0x0]";
    }
    else 
    {
        for ( int i = 0; i < foo.rows; i++ ) 
        {
            for ( int j = 0; j < foo.cols; j++ ) 
            {
                os << scientific << setw(12) << setprecision(3) << foo.matrix[i][j] << " ";
            }
            os << '\n';
        }
    }
    return os;
}

