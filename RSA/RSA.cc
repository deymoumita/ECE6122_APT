// ECE4122/6122 RSA Encryption/Decryption assignment
// Fall Semester 2015
// Moumita Dey

#include <iostream>
#include "RSA_Algorithm.h"

using namespace std;

int main()
{
  // Instantiate the one and only RSA_Algorithm object
  RSA_Algorithm RSA;
  mpz_class M, C, D;
  
  // Loop from sz = 32 to 1024 inclusive
  // for each size choose 10 different key pairs
  // For each key pair choose 100 differnt plaintext 
  // messages making sure it is smaller than n.
  // If not smaller then n then choose another
  // For each message encrypt it using the public key (n,e).
  // After encryption, decrypt the ciphertext using the private
  // key (n,d) and verify it matches the original message.

  // your code here

  size_t sz = 32;
  int c_fail = 0;
  int c_success = 0;
  for(size_t sz = 32; sz <= 1024; sz *= 2)
  { 
    //cout<<"Size of keys = "<<sz<<endl<<endl;
    for(int i = 0; i < 100; i++)
    {
      //cout<<"Key pairs "<<i+1<<endl<<endl;
      RSA.GenerateRandomKeyPair(sz);
      RSA.PrintNDE();
      for(int j = 0; j < 100; j++)
      {
        //cout<<"Message "<<j+1<<endl<<endl;
        M = RSA.Message(sz);
        RSA.PrintM(M);
        C = RSA.Encrypt(M);
        RSA.PrintC(C);
        D = RSA.Decrypt(C);
        //RSA.PrintD(D); 
        if (M == D)
        {
          c_success++;
          //cout<<"Decrypted message matches original message!!"<<endl<<endl;
        }
        else
        {
          c_fail++;
          //cout<<"Decrypted message doesn't match original message :("<<endl<<endl;
        }       
      }
      //cout<<"---------------------------------"<<endl<<endl;    
    }
    //cout<<"-----------------------------------------------------------------"<<endl<<endl;
  }
  //cout<<endl;
  //cout<<"Success: "<<c_success<<endl;
  //cout<<"Failure: "<<c_fail<<endl;
  //cout<<"-------------------------------------------------------------------------------------------------------------"<<endl<<endl;
}
  
