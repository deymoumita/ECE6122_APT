// Distributed two-dimensional Discrete FFT transform
// MOUMITA DEY
// ECE8893 Project 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>

#include "Complex.h"
#include "InputImage.h"

using namespace std;

// declaring functions
void Transform1D(Complex* h, int w, Complex* H);
void Transform1D_Inv(Complex* H, int w, Complex* h);


void Transform2D(const char* inputFN) 
{ 
  // Step 1 : Use InputImage object to read in the Tower.txt file and find the width/height of the input image
  InputImage image(inputFN);  // Create the helper object for reading the image
  int height = image.GetHeight();
  int width = image.GetWidth();

  // Step 2 : Use MPI to find how many CPUs in total, and which one this process is = nCPUs, myRank
  int myRank; // current CPU rank
  int nCPUs; // number of CPUs
  MPI_Comm_size(MPI_COMM_WORLD, &nCPUs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  int start_row[nCPUs]; // start row for each rank
  int num_completed_rows[nCPUs];  // number of computed rows of each rank
  int rc;   // return codes for MPI operations
  int num_rows_per_cpu = height / nCPUs;
  // initializing parameters
  for (int i = 0; i < nCPUs; i++)
  {
    num_completed_rows[i] = 0;
    start_row[i] = width / nCPUs * i;
  }
  int iter[nCPUs];

  // Step 3 : Allocate an array of Complex object of sufficient size to hold the 2d DFT results (size is width * height)
  Complex dfft1[height * width];  // stores 1dfft final results; with rank 0
  Complex dfft2[height * width];  // stores 2dfft final results; with rank 0
 
  // Step 4 : Obtain a pointer to the Complex 1d array of input data
  Complex *d = image.GetImageData();
  Complex d2give[width];  // part of matrix to be given to Transform1D()
  Complex H[width]; // output of Transform1D
  Complex Hrec[width * num_rows_per_cpu];  // received results by rank 0 of a particular rank
  Complex Hcompute[num_rows_per_cpu * width];   // 1D FFT results local to each rank
  Complex H2send[width * num_rows_per_cpu];   // Part of H to be sent to rank 0 from a particular rank
  Complex hin[width * num_rows_per_cpu];  // data received from rank 0 pertaining to that particular rank

  // Step 5 : Do the individual 1D transforms on the rows assigned to your CPU
  // Step 6 : Send the resultant transformed values to the appropriate other processors for the next phase.
  // Step 6a : To send and receive columns, you might need a separate Complex array of the correct size.
  // Step 7 : Receive messages from other processes to collect your columns

  // Each rank computes 1D fft of the set of rows pertaining to it and stores in its local Hcompute
  for(iter[myRank] = 0; iter[myRank] < num_rows_per_cpu; iter[myRank]++)
  {
      for(int i = 0; i < width; i++)
      {
        d2give[i] = d[(start_row[myRank] + num_completed_rows[myRank]) * width + i];
      }
      Transform1D(d2give, width, H);    
      for(int i = 0; i < width; i++)
      {
        Hcompute[num_completed_rows[myRank] * width + i] = H[i];
      }  
      num_completed_rows[myRank]++;
  }
  
  // Now all ranks will send its computed Hcompute to rank 0 and rank 0 will store in dfft1
  if (myRank == 0)
  {
    // store computed values of rank 0 directly to the matrix
    for (int i = 0; i < width * num_rows_per_cpu; i++)
    {
      dfft1[start_row[myRank] * width + i] = Hcompute[i];
    }
    for (int j = 1; j < nCPUs; j++)
    {
      MPI_Status status;
      rc = MPI_Recv(Hrec, sizeof(Hrec), MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);   
      if (rc != MPI_SUCCESS)
      {
        cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
        MPI_Finalize();
        //exit(1);
      }
      // now store the received results in the dfft1[]
      for (int i = 0; i < width * num_rows_per_cpu; i++)
      {
        dfft1[start_row[status.MPI_SOURCE] * width + i] = Hrec[i];
      }  
    }
  }
  else
  {
    rc = MPI_Send(Hcompute, sizeof(Hcompute), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    if (rc != MPI_SUCCESS)
    {
      cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
      MPI_Finalize();
      //exit(1);
    }      
  }
    
  // Save to file  
  if (myRank == 0)
  {
    //cout<<"Save to file"<<endl;
    string fn1("MyAfter1D.txt");
    image.SaveImageData(fn1.c_str(), dfft1, width, height);
  }
  //cout<<"1DFFT complete "<<myRank<<endl;

//------------------------------------------------------------------------------------------------------------

  // Step 8 : When all columns received, do the 1D transforms on the columns
  Complex dfft1_tr[width * height]; // transpose of dfft1
  Complex dfft2_tr[width * height]; // transpose of dfft2
  // initializing parameters
  for (int i; i < width * num_rows_per_cpu; i++)
  {
    H2send[i] = 0;
    hin[i] = 0;
    Hcompute[i] = 0;
  }

  // Create transpose of dfft1
  if (myRank == 0)
  {
    for(int i = 0; i < height; i++)
    {
      for(int j = 0; j < width; j++)
      {
        dfft1_tr[i * width + j] = dfft1[j * height + i];
      }
    }      
  }

  // Now send relevant rows to all CPUs from 0
  if (myRank == 0)
  {
    for (int k = 1; k < nCPUs; k++)
    {
      for (int j = 0; j < width * num_rows_per_cpu; j++)
      {
        H2send[j] = dfft1_tr[start_row[k] * width + j];  
      }
      rc = MPI_Send(H2send, sizeof(H2send), MPI_CHAR, k , 0, MPI_COMM_WORLD);
      if (rc != MPI_SUCCESS)
      {
        cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
        MPI_Finalize();
        //exit(1);
      } 
    }
    // storing only for rank 0
    for (int i = 0; i < width * num_rows_per_cpu; i++)
    {
      hin[i] = dfft1_tr[i];
    }
  }
  else
  {
    MPI_Status status;
    rc = MPI_Recv(hin, sizeof(hin), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status); 
    if (rc != MPI_SUCCESS)
    {
      cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
      MPI_Finalize();
      //exit(1);
    }
  }

  for (int i = 0; i < nCPUs; i++)
  {
    num_completed_rows[i] = 0;
  }
  // Each rank computes 1D fft of the set of rows pertaining to it and stores in its local Hcompute
  for(iter[myRank] = 0; iter[myRank] < num_rows_per_cpu; iter[myRank]++)
  {
      for(int i = 0; i < width; i++)
      {
        d2give[i] = hin[num_completed_rows[myRank] * width + i];
      }
      Transform1D(d2give, width, H);    
      for(int i = 0; i < width; i++)
      {
        Hcompute[num_completed_rows[myRank] * width + i] = H[i];
      }  
      num_completed_rows[myRank]++;
  }

  // Now all ranks will send their computed Hcompute to rank 0 and rank 0 will store in dfft2_tr
  if (myRank == 0)
  {
    // store computed values of rank 0 directly to the matrix
    for (int i = 0; i < width * num_rows_per_cpu; i++)
    {
      dfft2_tr[start_row[myRank] * width + i] = Hcompute[i];
    }
    for (int j = 1; j < nCPUs; j++)
    {
      MPI_Status status;
      rc = MPI_Recv(Hrec, sizeof(Hrec), MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);   
      if (rc != MPI_SUCCESS)
      {
        cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
        MPI_Finalize();
        //exit(1);
      }
      // now store the received results in the dfft2_tr[]
      for (int i = 0; i < width * num_rows_per_cpu; i++)
      {
        dfft2_tr[start_row[status.MPI_SOURCE] * width + i] = Hrec[i];
      }  
    }
  }
  else
  {
    rc = MPI_Send(Hcompute, sizeof(Hcompute), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    if (rc != MPI_SUCCESS)
    {
      cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
      MPI_Finalize();
      //exit(1);
    }      
  }

  // Create transpose of dfft2_tr
  if (myRank == 0)
  {
    for(int i = 0; i < height; i++)
    {
      for(int j = 0; j < width; j++)
      {
        dfft2[i * width + j] = dfft2_tr[j * height + i];
      }
    }      
  }
    
  // Save to file  
  if (myRank == 0)
  {
    //cout<<"Save to file"<<endl;
    string fn2("MyAfter2D.txt");
    image.SaveImageData(fn2.c_str(), dfft2, width, height);
  }
  //cout<<"2DFFT complete "<<myRank<<endl;
  
  // ----------------------------------------------------------------------------------------------------------- 
  // ----------------------------------------------------------------------------------------------------------- 
  // ----------------------------------------------------------------------------------------------------------- 
  // ----------------------------------------------------------------------------------------------------------- 
  // ----------------------------------------------------------------------------------------------------------- 

  // Computation of 2D FFT Inverse

  Complex idfft1[width * height];
  Complex idfft1_tr[width * height];
  Complex idfft2_tr[width * height];
  Complex idfft2[width * height];

  // Rank 0 has dfft2 array. It will first send out relevant rows to all other ranks
  if (myRank == 0)
  {
    for (int k = 1; k < nCPUs; k++)
    {
      for (int j = 0; j < width * num_rows_per_cpu; j++)
      {
        H2send[j] = dfft2[start_row[k] * width + j];  
      }
      rc = MPI_Send(H2send, sizeof(H2send), MPI_CHAR, k , 0, MPI_COMM_WORLD);
      if (rc != MPI_SUCCESS)
      {
        cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
        MPI_Finalize();
        //exit(1);
      } 
    }
    // storing only for rank 0
    for (int i = 0; i < width * num_rows_per_cpu; i++)
    {
      hin[i] = dfft2[i];
    }
  }
  else
  {
    MPI_Status status;
    rc = MPI_Recv(hin, sizeof(hin), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status); 
    if (rc != MPI_SUCCESS)
    {
      cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
      MPI_Finalize();
      //exit(1);
    }
  }

  // Now all ranks will calculate inverse 1D FFT on their specific set of rows
  for (int i = 0; i < nCPUs; i++)
  {
    num_completed_rows[i] = 0;
  }
  // Each rank computes 1D fft of the set of rows pertaining to it and stores in its local Hcompute
  for(iter[myRank] = 0; iter[myRank] < num_rows_per_cpu; iter[myRank]++)
  {
      for(int i = 0; i < width; i++)
      {
        d2give[i] = hin[num_completed_rows[myRank] * width + i];
      }
      Transform1D_Inv(d2give, width, H);    
      for(int i = 0; i < width; i++)
      {
        Hcompute[num_completed_rows[myRank] * width + i] = H[i];
      }  
      num_completed_rows[myRank]++;
  }

  // Now all ranks will send their computed Hcompute to rank 0 and rank 0 will store in idfft1
  if (myRank == 0)
  {
    // store computed values of rank 0 directly to the matrix
    for (int i = 0; i < width * num_rows_per_cpu; i++)
    {
      idfft1[start_row[myRank] * width + i] = Hcompute[i];
    }
    for (int j = 1; j < nCPUs; j++)
    {
      MPI_Status status;
      rc = MPI_Recv(Hrec, sizeof(Hrec), MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);   
      if (rc != MPI_SUCCESS)
      {
        cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
        MPI_Finalize();
        //exit(1);
      }
      // now store the received results in the dfft2_tr[]
      for (int i = 0; i < width * num_rows_per_cpu; i++)
      {
        idfft1[start_row[status.MPI_SOURCE] * width + i] = Hrec[i];
      }  
    }
  }
  else
  {
    rc = MPI_Send(Hcompute, sizeof(Hcompute), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    if (rc != MPI_SUCCESS)
    {
      cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
      MPI_Finalize();
      //exit(1);
    }      
  }

  // Create transpose of idfft1
  if (myRank == 0)
  {
    for(int i = 0; i < height; i++)
    {
      for(int j = 0; j < width; j++)
      {
        idfft1_tr[i * width + j] = idfft1[j * height + i];
      }
    }      
  }

  // Rank 0 has idfft1_tr array. It will first send out relevant rows to all other ranks for 2D FFT operation
  if (myRank == 0)
  {
    for (int k = 1; k < nCPUs; k++)
    {
      for (int j = 0; j < width * num_rows_per_cpu; j++)
      {
        H2send[j] = idfft1_tr[start_row[k] * width + j];  
      }
      rc = MPI_Send(H2send, sizeof(H2send), MPI_CHAR, k , 0, MPI_COMM_WORLD);
      if (rc != MPI_SUCCESS)
      {
        cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
        MPI_Finalize();
        //exit(1);
      } 
    }
    // storing only for rank 0
    for (int i = 0; i < width * num_rows_per_cpu; i++)
    {
      hin[i] = idfft1_tr[i];
    }
  }
  else
  {
    MPI_Status status;
    rc = MPI_Recv(hin, sizeof(hin), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status); 
    if (rc != MPI_SUCCESS)
    {
      cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
      MPI_Finalize();
      //exit(1);
    }
  }

  // All ranks will perform their operations of FFT
  // Now all ranks will calculate inverse 1D FFT on their specific set of rows
  for (int i = 0; i < nCPUs; i++)
  {
    num_completed_rows[i] = 0;
  }
  // Each rank computes 1D fft of the set of rows pertaining to it and stores in its local Hcompute
  for(iter[myRank] = 0; iter[myRank] < num_rows_per_cpu; iter[myRank]++)
  {
      for(int i = 0; i < width; i++)
      {
        d2give[i] = hin[num_completed_rows[myRank] * width + i];
      }
      Transform1D_Inv(d2give, width, H);    
      for(int i = 0; i < width; i++)
      {
        Hcompute[num_completed_rows[myRank] * width + i] = H[i];
      }  
      num_completed_rows[myRank]++;
  }

  // Now all ranks will send their computed Hcompute to rank 0 and rank 0 will store in idfft2_tr
  if (myRank == 0)
  {
    // store computed values of rank 0 directly to the matrix
    for (int i = 0; i < width * num_rows_per_cpu; i++)
    {
      idfft2_tr[start_row[myRank] * width + i] = Hcompute[i];
    }
    for (int j = 1; j < nCPUs; j++)
    {
      MPI_Status status;
      rc = MPI_Recv(Hrec, sizeof(Hrec), MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);   
      if (rc != MPI_SUCCESS)
      {
        cout<<"\nRank "<<myRank<<" recv failed, rc "<<rc<<endl;
        MPI_Finalize();
        //exit(1);
      }
      // now store the received results in the dfft2_tr[]
      for (int i = 0; i < width * num_rows_per_cpu; i++)
      {
        idfft2_tr[start_row[status.MPI_SOURCE] * width + i] = Hrec[i];
      }  
    }
  }
  else
  {
    rc = MPI_Send(Hcompute, sizeof(Hcompute), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    if (rc != MPI_SUCCESS)
    {
      cout<<"\nRank "<<myRank<<" send failed, rc "<<rc<<endl;
      MPI_Finalize();
      //exit(1);
    }      
  }

  // Create transpose of idfft2_tr
  if (myRank == 0)
  {
    for(int i = 0; i < height; i++)
    {
      for(int j = 0; j < width; j++)
      {
        idfft2[i * width + j] = idfft2_tr[j * height + i];
      }
    }      
  }

  // Save to file  
  if (myRank == 0)
  {
    //cout<<"Save to file"<<endl;
    string fn3("MyAfterInverse.txt");
    image.SaveImageData(fn3.c_str(), idfft2, width, height);
  }
  //cout<<"Inverse 2DFFT complete "<<myRank<<endl;







  //cout<<"Finalize "<<myRank <<endl;
  MPI_Finalize();
 
}

void Transform1D(Complex* h, int w, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
  int N = w;    // assigning this just for simplicity purpose
  Complex omega = 0;
  Complex temp1, temp2;

  for(int j = 0; j < N; j++)
  {
    temp1 = 0;
    temp2 = 0;
    for(int i = 0; i < N; i++)
    {
      omega = Complex(cos(2*M_PI*j*i/N), -sin(2*M_PI*j*i/N));
      temp2 = omega.operator*(h[i]);
      temp1 = temp1.operator+(temp2);
    }
    H[j] = temp1; 
  }
}

void Transform1D_Inv(Complex* H, int w, Complex* h)
{
  int N = w;    // assigning this just for simplicity purpose
  Complex omega;
  Complex temp1, temp2;

  for(int j = 0; j < N; j++)
  {
    temp1 = 0;
    temp2 = 0;
    for(int i = 0; i < N; i++)
    {
      omega = Complex(cos(2*M_PI*j*i/N), sin(2*M_PI*j*i/N));
      temp2 = omega.operator*(H[i]);
      temp1 = temp1.operator+(temp2);
    }
    h[j] = Complex(temp1.real/N, temp1.imag/N); 
  }
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  
  // MPI initialization here
  int rc;
  rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) 
  {
    cout<<"Error starting MPI Program. Terminating.\n";
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  // Perform the transform.
  Transform2D(fn.c_str()); 
  
}  
  

  
