#include <fstream>
#include <iostream>
#include <NTL/matrix.h>
#include <NTL/mat_ZZ.h>
#include <NTL/vector.h>
#include <NTL/ZZ.h>


using namespace std;


NTL_CLIENT;


void set_params(
        int argc, char* argv[],
        string& encrypted_path,
        string& decrypted_path,
        string& priv_key_path);
void read_priv_key(ifstream& priv_key,
        int& N,
        Mat<ZZ>& private_B,
        Mat<ZZ>& private_U);
string decrypt(Vec<ZZ> encrypted,
        const int& N,
        Mat<ZZ> private_B,
        Mat<ZZ> private_U);
int main(int argc, char* argv[]);


void set_params(
        int argc, char* argv[],
        string& encrypted_path,
        string& decrypted_path,
        string& priv_key_path)
{
    encrypted_path = "./encrypted-output.txt";
    decrypted_path = "./decrypted-output.txt";
    priv_key_path = "./private.key";

    if(argc >= 2)
    {
        encrypted_path = argv[1];
    }
    if(argc >= 3)
    {
        decrypted_path = argv[2];
    }
    if(argc >= 4)
    {
        priv_key_path = argv[3];
    }
}


void read_priv_key(ifstream& priv_key,
        int& N,
        Mat<ZZ>& private_B,
        Mat<ZZ>& private_U)
{
    priv_key >> N;

    private_B.SetDims(N, N);
    private_U.SetDims(N, N);

    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            priv_key >> private_B[i][j];
        }
    }

    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            priv_key >> private_U[i][j];
        }
    }
}


string decrypt(Vec<ZZ> encrypted_vect,
        const int& N,
        Mat<ZZ> private_B,
        Mat<ZZ> private_U)
{
    string decrypted_string;
    Vec<ZZ> decrypted_vect;

    decrypted_vect.SetLength(N);

    decrypted_vect = encrypted_vect * inv(private_B) * inv(private_U);

    for(int i = 0; i < N; ++i)
    {
        decrypted_string += char(conv<int>(decrypted_vect[i]) % 256);
    }

    return decrypted_string;
}


int main(int argc, char* argv[])
{
    Mat<ZZ> private_B;
    Mat<ZZ> private_U;
    Vec<ZZ> encrypted_vect;
    int N;
    string decrypted_str;

    string encrypted_path;
    string decrypted_path;
    string priv_key_path;

    set_params(argc, argv,
            encrypted_path,
            decrypted_path,
            priv_key_path);

    ifstream encrypted(encrypted_path);
    ofstream decrypted(decrypted_path);
    ifstream priv_key(priv_key_path);

    read_priv_key(priv_key, N, private_B, private_U);

    priv_key.close();

    encrypted_vect.SetLength(N);

    for(int i = 0; i < N; ++i)
    {
        encrypted >> encrypted_vect[i];
    }

    encrypted.close();

    decrypted_str = decrypt(encrypted_vect, N, private_B, private_U);

    decrypted << decrypted_str;
    cout << decrypted_str << endl;

    decrypted.close();

    return 0;
}
