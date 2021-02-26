// RSA Assignment for ECE4122/6122 Fall 2015
// Moumita Dey

#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "RSA_Algorithm.h"

using namespace std;

// Implement the RSA_Algorithm methods here

// Constructor
RSA_Algorithm::RSA_Algorithm()
  : rng(gmp_randinit_default)
{
  // get a random seed for the random number generator
  int dr = open("/dev/random", O_RDONLY);
  if (dr < 0)
    {
      cout << "Can't open /dev/random, exiting" << endl;
      exit(0);
    }
  unsigned long drValue;
  read(dr, (char*)&drValue, sizeof(drValue));
  //cout << "drValue " << drValue << endl;
  rng.seed(drValue);
// No need to init n, d, or e.
}

// Fill in the remainder of the RSA_Algorithm methods

void RSA_Algorithm::GenerateRandomKeyPair(size_t sz)
{
  bool flag_p, flag_q, flag_set = false;
  mpz_class p, q;
  mpz_class phi;
  mpz_class rop;

  while(flag_set == false)
  {
    
       
    while(flag_p == 0)
    {
    	p = rng.get_z_bits(sz);
    	flag_p = mpz_probab_prime_p(p.get_mpz_t(), 100);
    }
    while(flag_q == 0)
    {
    	q = rng.get_z_bits(sz); 
    	flag_q = mpz_probab_prime_p(q.get_mpz_t(), 100);
    }
    
    if((flag_p != 0) && (flag_q != 0))
    {
      if(p != q)
      {
        flag_set = true;
      }
    }
  }

  n = p * q;

  phi = (p - 1) * (q - 1);
  flag_set = false;

  while(flag_set == false)
  {
    d = rng.get_z_bits(2 * sz);
    mpz_gcd(rop.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
    if((rop == 1) && (d < phi))
    {
      flag_set = true;
    }
  }

  int ret_val = 0; 

  //while (ret_val == 0)
  {
    ret_val = mpz_invert(e.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());    
  }
  //cout<<"p: "<<p<<endl<<"q: "<<q<<endl;
}

mpz_class RSA_Algorithm::Message(size_t sz)
{
  mpz_class M;
  bool flag_set = false;
  while(flag_set == false)
  {
    M = rng.get_z_bits(2 * sz);
    if(M < n)
    {
      flag_set = true;
    }
  }

  return M; 
}

mpz_class RSA_Algorithm::Encrypt(mpz_class M)
{
  mpz_class C;
  mpz_powm(C.get_mpz_t(), M.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());

  return C;
}

mpz_class RSA_Algorithm::Decrypt(mpz_class C)
{
  mpz_class D;
  mpz_powm(D.get_mpz_t(), C.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());

  return D;
}

void RSA_Algorithm::PrintND()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << endl;
}

void RSA_Algorithm::PrintNE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " e " << e << endl;
}

void RSA_Algorithm::PrintNDE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << " e " << e << endl;
}

void RSA_Algorithm::PrintM(mpz_class M)
{ // Do not change this, right format for the grading script
  cout << "M " << M << endl;
}

void RSA_Algorithm::PrintC(mpz_class C)
{ // Do not change this, right format for the grading script
  cout << "C " << C << endl;
}

void RSA_Algorithm::PrintD(mpz_class D)
{ // Do not change this, right format for the grading script
  cout << "D " << D << endl;
}




