// Grad student portion of the RSA assignment
// Fall 2015 ECE6122
// Moumita Dey

#include <iostream>
#include <math.h>
#include <string>

#include "RSA_Algorithm.h"

using namespace std;

int main(int argc, char** argv)
{ // Arguments are as follows:
  //argv[1] = n;
  //argv[2] = d;  // n and e are the public key
  //argv[3] = first 6 characters of encrypted message
  //argv[4] = next 6 characters .. up to argv[12] are the lsat 6 characters
  // The number of arguments will always be exacty 12, and each argument past the
  // public key contain 6 ascii characters of the encrypted message.
  // Each of the 32bit values in the argv[] array are right justified in the
  // low order 48 bits of each unsigned long.  The upper 16 bits are always
  // zero, which insures the each unsigned long is < n (64 bits) and therefore
  // the RSA encryption will work.

  // Below is an example of the BreakRSA and command line arguments:

/*
 ./BreakRSA  2966772883822367927 2642027824495698257  817537070500556663 1328829247235192134 1451942276855579785 2150743175814047358 72488230455769594 1989174916172335943 962538406513796755 1069665942590443121 72678741742252898 1379869649761557209
*/
//   The corect output from the above is:
//   HelloTest  riley CekwkABRIZFlqmWTanyXLogFgBUENvzwHpEHRCZIKRZ
//
//   The broken (computed) private key for the above is 4105243553



  // Our one and only RSA_Algorithm object
  RSA_Algorithm rsa;
  
  // First "break" the keys by factoring n and computing d
  // Set the keys in the rsa object afer calculating d
  rsa.n = mpz_class(argv[1]);
  rsa.d = mpz_class(argv[2]);

  /* Pollard rho algorithm for factorization:
  g(x) = (x^2 + 1)mod n
    x ← 2; y ← 2; d ← 1
    while d = 1:
        x ← g(x)
        y ← g(g(y))
        d ← gcd(|x - y|, n)
    if d = n: 
        return failure
    else:
        return d
  */

  mpz_class x, y, d, z;
  mpz_class p, q, phi;
  x = 2;
  y = 2;
  d = 1;
  long unsigned int pow = 2;
  int ret_val = 0; 

  while(d == 1)
  {
    mpz_pow_ui(x.get_mpz_t(), x.get_mpz_t(), pow);
    x++;
    mpz_mod (x.get_mpz_t(), x.get_mpz_t(), (rsa.n).get_mpz_t());
    mpz_pow_ui(y.get_mpz_t(), y.get_mpz_t(), pow);
    y++;
    mpz_mod (y.get_mpz_t(), y.get_mpz_t(), (rsa.n).get_mpz_t());
    mpz_pow_ui(y.get_mpz_t(), y.get_mpz_t(), pow);
    y++;
    mpz_mod (y.get_mpz_t(), y.get_mpz_t(), (rsa.n).get_mpz_t());
    z = x - y;
    mpz_abs(z.get_mpz_t(), z.get_mpz_t());
    mpz_gcd(d.get_mpz_t(), z.get_mpz_t(), (rsa.n).get_mpz_t());    
  }
  //cout<<"x = "<<x<<"\ty = "<<y<<"\tz = "<<z<<"\td = "<<d<<endl;
  p = d;
  q = rsa.n / p;
  phi = (p - 1) * (q - 1);

  // Set rsa.d to the calculated private key d
  while (ret_val == 0)
  {
    ret_val = mpz_invert((rsa.e).get_mpz_t(), (rsa.d).get_mpz_t(), phi.get_mpz_t());    
  }  
  //rsa.PrintNDE();
  // rsa.d = mpz_class(// broken d value here) 
  for (int i = 3; i < 13; ++i)
  { 
    // Decrypt each set of 6 characters
    mpz_class c(argv[i]);
    char ch[6];
    mpz_class m = rsa.Decrypt(c);
    //  use the get_ui() method in mpz_class to get the lower 48 bits of the m
    unsigned long ul = m.get_ui();
    // Now print the 6 ascii values in variable ul.  As stated above the 6 characters
    // are in the low order 48 bits of ui.
    //cout<<m<<"("<<sizeof(m)<<")\t"<<ul<<"("<<sizeof(ul)<<")"<<endl;
    for(int i = 0; i < 8; i++)
    {
      int x = (ul >> (8*i)) & 0xff;
      ch[5-i] = (char)x;
    }
    for(int i = 0; i < 6; i++)
    {
      cout<<ch[i];
    }
  }
  //cout << endl << endl;
  //cout<<"The broken (computed) private key for the above is "<<rsa.d<<endl;
}

