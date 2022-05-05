/*
 *
 * This code is based on the code found here:
 *     https://github.com/EdwardMork/GGH-CryptoSystem/blob/master/ggh.py
 *
 * Written by Matthew Sprague, May 5th 2022
 *
 * */


#include <fstream>
#include <iostream>
#include <sstream>
#include <NTL/matrix.h>
#include <NTL/mat_ZZ.h>
#include <NTL/vector.h>
#include <NTL/RR.h>
#include <NTL/ZZ.h>


using namespace std;


NTL_CLIENT;



void set_params(
        int argc, char* argv[],
        string& plaintext_path,
        string& encrypted_path,
        string& priv_key_path,
        string& pub_key_path);
void keygen(Mat<ZZ>& private_B, Mat<ZZ>& private_U, Mat<ZZ>& public_B, const int & N);
RR hadamard_ratio(Mat<ZZ> basis, const int & N);
Mat<ZZ> rand_unimod_mat_N(const int & N);
Vec<ZZ> encrypt_string(string m, Mat<ZZ> public_B);
int main(int argc, char* argv[]);


void set_params(
        int argc, char* argv[],
        string& plaintext_path,
        string& encrypted_path,
        string& priv_key_path,
        string& pub_key_path)
{
    plaintext_path = "./input-file.txt";
    encrypted_path = "./encrypted-output.txt";
    priv_key_path = "./private.key";
    pub_key_path = "./public.key";

    if(argc >= 2)
    {
        plaintext_path = argv[1];
    }
    if(argc >= 3)
    {
        encrypted_path = argv[2];
    }
    if(argc >= 4)
    {
        priv_key_path = argv[3];
    }
    if(argc >= 5)
    {
        priv_key_path = argv[4];
    }
}


// Generate keys for GGH encryption
void keygen(Mat<ZZ>& private_B, Mat<ZZ>& private_U, Mat<ZZ>& public_B, const int & N)
{
    RR h_ratio = RR(1);

    private_B.SetDims(N, N);
    private_U.SetDims(N, N);
    public_B.SetDims(N, N);

    // Generate a private basis
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            if(i == j)
            {
                private_B[i][j] = 1; 
            }
            else
            {
                private_B[i][j] = 0;
            }
        }
    }

    // Generate a public basis
    while(h_ratio > 0.1)
    {
        Mat<ZZ> uni = rand_unimod_mat_N(N);
        Mat<ZZ> candidate;

        candidate.SetDims(N, N);

        candidate = private_B * uni;

        h_ratio = hadamard_ratio(candidate, N);
        private_U = uni;
        public_B = candidate;
    }
}


// Find the hadamard ratio of a matrix
// Close to 1: orthagonal
// Close to 0: parallel
RR hadamard_ratio(Mat<ZZ> basis, const int & N)
{
    ZZ det;
    RR mult = RR(1);
    RR ratio = RR(0);
    RR exponent = RR(1);

    determinant(det, basis);

    exponent /= N;

    // for each row in the matrix
    for(int i = 0; i < N; ++i)
    {
        RR norm = RR(0);
        for(int j = 0; j < N; ++j)
        {
            norm += conv<RR>(basis[i][j] ^ 2);
        }
        norm = SqrRoot(norm);
        mult *= norm;
    }

    // cannot call pow on a negative 1st argument
    if(conv<RR>(det) / mult < 0)
        return RR(0);

    ratio = pow((conv<RR>(det) / mult), exponent);

    return ratio;
}



// Generate a random unimodular matrix of N x N dimensions
Mat<ZZ> rand_unimod_mat_N(const int & N)
{
    Mat<ZZ> random_mat;
    Mat<ZZ> upper_triangle;
    Mat<ZZ> lower_triangle;
    Mat<ZZ> unimod;

    random_mat.SetDims(N, N);
    upper_triangle.SetDims(N, N);
    lower_triangle.SetDims(N, N);
    unimod.SetDims(N, N);

    // populate our matricies
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            // populate the random matrix
            random_mat[i][j] = RandomBnd(21) - 10;

            // populate the upper & lower trianglular matricies
            if(j > i)
            {
                upper_triangle[i][j] = random_mat[i][j];
                lower_triangle[i][j] = 0;
            }
            // we want -1 or +1 in the diagonal
            else if(j == i)
            {
                ZZ rand = ZZ(RandomBnd(1));
                if(rand == 0)
                    rand = -1;

                upper_triangle[i][j] = rand;
                lower_triangle[i][j] = rand;
            }
            else
            {
                upper_triangle[i][j] = 0;
                lower_triangle[i][j] = RandomBnd(21) - 10;
            }
        }
    }

    unimod = upper_triangle * lower_triangle;
    
    /*
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            cout << unimod[i][j] << " ";
            if(unimod[i][j] >= 0)
                cout << " ";
            if(unimod[i][j] != -10 && unimod[i][j] != 10)
                cout << " ";
        }
        cout << endl;
    }
    */

    return unimod;
}


Vec<ZZ> encrypt_string(string m, Mat<ZZ> public_B)
{
    Vec<ZZ> unencrypted;
    Vec<ZZ> encrypted;
    unencrypted.SetLength(m.length());
    encrypted.SetLength(m.length());

    for(int i = 0; i < m.length(); ++i)
    {
        unencrypted[i] = ZZ(int(m[i]));
    }

    encrypted = unencrypted * public_B;

    return encrypted;
}


int main(int argc, char* argv[])
{
    Mat<ZZ> public_B;
    Mat<ZZ> private_B;
    Mat<ZZ> private_U;
    int N;
    string plaintext_str;
    Vec<ZZ> encrypted_vect;

    string plaintext_path;
    string encrypted_path;
    string pub_key_path;
    string priv_key_path;

    set_params(argc, argv,
            plaintext_path,
            encrypted_path,
            priv_key_path,
            pub_key_path);

    ifstream plaintext(plaintext_path);
    ofstream encrypted(encrypted_path);
    ofstream priv_key(priv_key_path);
    ofstream pub_key(pub_key_path);

    stringstream plaintext_buffer;
    plaintext_buffer << plaintext.rdbuf();

    plaintext_str = plaintext_buffer.str();
    N = plaintext_str.length();
    cout << plaintext_str;

    plaintext.close();

    keygen(private_B, private_U, public_B, N);

    // write public, private keys
    priv_key << N << endl;
    pub_key << N << endl;

    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            priv_key << private_B[i][j] << " ";
            pub_key << public_B[i][j] << " ";
        }
        priv_key << endl;
        pub_key << endl;
    }
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            priv_key << private_U[i][j] << " ";
        }
        priv_key << endl;
    }


    priv_key.close();
    pub_key.close();

    encrypted_vect = encrypt_string(plaintext_str, public_B);

    for(int i = 0; i < encrypted_vect.length(); ++i)
    {
        encrypted << encrypted_vect[i] << endl;
        cout << encrypted_vect[i] << " ";
    }
    cout << endl;

    encrypted.close();
    
    return 0;
}
