#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <complex>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include "cellinfo.h"
#include "common.h"
#include "mointegrals.h"
#include "Eigen/Dense"
#include <time.h>
using namespace std;
using namespace Eigen;

extern "C"
{
    void clear_fortran_file_( char* filename );
    void append_moint_to_file_( char* filename, double* dval, int* p,
       int* q, int* r, int* s );
}

static size_t get_ijkl( int i, int j, int k, int l ){
    size_t pq, rs, index;
    pq = ( i > j ) ? (i*(i+1))/2+j : (j*(j+1))/2+i;
    rs = ( k > l ) ? (k*(k+1))/2+l : (l*(l+1))/2+k;
    index = ( pq > rs ) ? (pq*(pq+1))/2+rs : (rs*(rs+1))/2+pq; 
    return index;
}

void moIntegralFactory::write_gamma_moint_to_fcidump_coulomb( 
    const char* ofilename,
    int tol
){
    int ij_index, kl_index, mo_index;
    double val;
    FILE* oFile;
    printf( "WRITING TO FCIDUMP FILE.... \n" );
    oFile = fopen( ofilename, "w" );

    bool create_new_kl;
    bool create_first_cont,
         create_second_cont,
         create_third_cont,
         create_fourth_cont;
    double dtol = pow( 10., 1. * tol );
    //
    //
    // printing header
    //
    //
    int orbsym = 1;
    int ms2 = 0;
    int isym = 1;
    fprintf( oFile, " &FCI NORB=%3d,NELEC=%3d,MS2=%2d,\n", nmo_scell, nmo_scell, ms2 );
    fprintf( oFile, "  ORBSYM=" );
    for( int i = 0; i < nmo_scell; ++i ) 
      fprintf( oFile, "%1d,", orbsym );
    fprintf( oFile, "\n" );
    fprintf( oFile, "  ISYM=%1d \n", isym );
    fprintf( oFile, " &END\n" );
    
    for( int k = 0; k < nmo_scell; ++k ){
    create_first_cont = true;
    printf( "Performing gamma transform ; Completed (%3d /%3d ) ... \n", (k+1), nmo_scell ); cout << flush;
    for( int l = 0; l <= k; ++l ){
      create_second_cont = true;
      kl_index = ( k * ( k + 1 ) ) / 2 + l;
      create_new_kl = true;
      for( int i = 0; i < nmo_scell; ++i ){
      create_third_cont = true;
      for( int j = 0; j <= i; ++j ){
        create_fourth_cont = true;
        ij_index = ( i * ( i + 1 ) ) / 2 + j;
        if( ij_index >= kl_index ){
          mo_index = ( ij_index * ( ij_index + 1 ) ) / 2 + kl_index;
          val = quarter_gamma_transform_coulomb( k, l, i, j, create_first_cont, create_second_cont, create_third_cont, create_fourth_cont );
          if( fabs( val ) > dtol )
            fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, (i+1), (j+1), (k+1), (l+1) );

          create_first_cont  = false;
          create_second_cont = false;
          create_third_cont  = false;
          create_fourth_cont = false;
        }
      }
      }
    }
    }
    for( int i = 0; i < nmo_scell; ++i ){
      val = evals.irrep( 0 )( i, 0 );
      fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, (i+1), (i+1), 0, 0 );
    }
    val = 0.0;
    fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, 0, 0, 0, 0 );
    fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, 0, 0, 0, 0 );
    fclose( oFile );
}


void moIntegralFactory::write_gamma_moint_to_fcidump( 
    const char* ofilename,
    int tol
){
    int ij_index, kl_index, mo_index;
    double val;
    FILE* oFile;
    printf( "WRITING TO FCIDUMP FILE.... \n" );
    oFile = fopen( ofilename, "w" );

    bool create_new_kl;
    bool create_first_cont,
         create_second_cont,
         create_third_cont,
         create_fourth_cont;
    double dtol = pow( 10., 1. * tol );
    //
    //
    // printing header
    //
    //
    int orbsym = 1;
    int ms2 = 0;
    int isym = 1;
    fprintf( oFile, " &FCI NORB=%3d,NELEC=%3d,MS2=%2d,\n", nmo_scell, nmo_scell, ms2 );
    fprintf( oFile, "  ORBSYM=" );
    for( int i = 0; i < nmo_scell; ++i ) 
      fprintf( oFile, "%1d,", orbsym );
    fprintf( oFile, "\n" );
    fprintf( oFile, "  ISYM=%1d \n", isym );
    fprintf( oFile, " &END\n" );
    
    for( int k = 0; k < nmo_scell; ++k ){
    create_first_cont = true;
    printf( "Performing gamma transform ; Completed (%3d /%3d ) ... \n", (k+1), nmo_scell ); cout << flush;
    for( int l = 0; l <= k; ++l ){
      create_second_cont = true;
      kl_index = ( k * ( k + 1 ) ) / 2 + l;
      create_new_kl = true;
      for( int i = 0; i < nmo_scell; ++i ){
      create_third_cont = true;
      for( int j = 0; j <= i; ++j ){
        create_fourth_cont = true;
        ij_index = ( i * ( i + 1 ) ) / 2 + j;
        if( ij_index >= kl_index ){
          mo_index = ( ij_index * ( ij_index + 1 ) ) / 2 + kl_index;
          val = quarter_gamma_transform( k, l, i, j, create_first_cont, create_second_cont, create_third_cont, create_fourth_cont );
          if( fabs( val ) > dtol )
            fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, (i+1), (j+1), (k+1), (l+1) );

          create_first_cont  = false;
          create_second_cont = false;
          create_third_cont  = false;
          create_fourth_cont = false;
        }
      }
      }
    }
    }
    for( int i = 0; i < nmo_scell; ++i ){
      val = evals.irrep( 0 )( i, 0 );
      fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, (i+1), (i+1), 0, 0 );
    }
    val = 0.0;
    fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, 0, 0, 0, 0 );
    fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, 0, 0, 0, 0 );
    fclose( oFile );
}


void moIntegralFactory::write_gamma_moint_to_fcidump_new( 
    const char* ofilename,
    int tol
){
    int ij_index, kl_index, mo_index;
    double val;
    FILE* oFile;
    printf( "WRITING TO FCIDUMP FILE.... \n" );
    oFile = fopen( ofilename, "w" );

    bool create_new_kl;
    bool create_first_cont,
         create_second_cont,
         create_third_cont,
         create_fourth_cont;
    double dtol = pow( 10., 1. * tol );
    //
    //
    // printing header
    //
    //
    int orbsym = 1;
    int ms2 = 0;
    int isym = 1;
    fprintf( oFile, " &FCI NORB=%3d,NELEC=%3d,MS2=%2d,\n", nmo_scell, nmo_scell, ms2 );
    fprintf( oFile, "  ORBSYM=" );
    for( int i = 0; i < nmo_scell; ++i ) 
      fprintf( oFile, "%1d,", orbsym );
    fprintf( oFile, "\n" );
    fprintf( oFile, "  ISYM=%1d \n", isym );
    fprintf( oFile, " &END\n" );
    
    for( int k = 0; k < nmo_scell; ++k ){
    create_first_cont = true;
    printf( "Performing gamma transform ; Completed (%3d /%3d ) ... \n", (k+1), nmo_scell ); cout << flush;
    for( int l = 0; l <= k; ++l ){
      create_second_cont = true;
      kl_index = ( k * ( k + 1 ) ) / 2 + l;
      create_new_kl = true;
      for( int i = 0; i < nmo_scell; ++i ){
      create_third_cont = true;
      for( int j = 0; j <= i; ++j ){
        create_fourth_cont = true;
        ij_index = ( i * ( i + 1 ) ) / 2 + j;
        if( ij_index >= kl_index ){
          mo_index = ( ij_index * ( ij_index + 1 ) ) / 2 + kl_index;
          val = quarter_gamma_transform( k, l, i, j, create_first_cont, create_second_cont, 
                                                     create_third_cont, create_fourth_cont );
          if( k == l || i == j ){
            val = make_2eint_pqrs( coulomb_matr, evecsXd.irrep( 0 ), i, j, k, l );
          }else{
          }
          if( fabs( val ) > dtol )
            fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, (i+1), (j+1), (k+1), (l+1) );

          create_first_cont  = false;
          create_second_cont = false;
          create_third_cont  = false;
          create_fourth_cont = false;
        }
      }
      }
    }
    }
    for( int i = 0; i < nmo_scell; ++i ){
      val = evals.irrep( 0 )( i, 0 );
      //if( i < (int)nmo_scell/2 ){
      //  val -= make_2eint_pqrs( kernel_matr, evecsXd.irrep( 0 ), i, i, i, i );
      //  val += make_2eint_pqrs( coulomb_matr, evecsXd.irrep( 0 ), i, i, i, i );
      //}
      fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, (i+1), (i+1), 0, 0 );
    }
    val = 0.0;
    fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, 0, 0, 0, 0 );
    fprintf( oFile, "%24.16e %3d %3d %3d %3d \n", val, 0, 0, 0, 0 );
    fclose( oFile );
}


void moIntegralFactory::write_gamma_moint_to_fcidump_binary( 
    const char* ofilename,
    int tol
){
    int ij_index, kl_index, mo_index;
    char fileholder[20];
    strcpy( fileholder, ofilename );
    double val;
    printf( "WRITING TO BINARY FCIDUMP FILE.... '%s' \n", ofilename );
    printf( "CLEARING FILE... \n" );
    clear_fortran_file_( fileholder );
    printf( "APPENDING VALUES... \n" );
    bool create_new_kl;
    bool create_first_cont,
         create_second_cont,
         create_third_cont,
         create_fourth_cont;
    double dtol = pow( 10., 1. * tol );
    //
    //
    // IGNORING HEADER
    //
    //
    int orbsym = 1;
    int ms2 = 0;
    int isym = 1;
    //fprintf( oFile, " &FCI NORB=%3d,NELEC=%3d,MS2=%2d,\n", nmo_scell, nmo_scell, ms2 );
    //fprintf( oFile, "  ORBSYM=" );
    //for( int i = 0; i < nmo_scell; ++i ) 
    //  fprintf( oFile, "%1d,", orbsym );
    //fprintf( oFile, "\n" );
    //fprintf( oFile, "  ISYM=%1d \n", isym );
    //fprintf( oFile, " &END\n" );
    
    for( int k = 0; k < nmo_scell; ++k ){
    create_first_cont = true;
    printf( "Performing gamma transform ; Completed (%3d /%3d ) ... \n", (k+1), nmo_scell ); cout << flush;
    for( int l = 0; l <= k; ++l ){
      create_second_cont = true;
      kl_index = ( k * ( k + 1 ) ) / 2 + l;
      create_new_kl = true;
      for( int i = 0; i < nmo_scell; ++i ){
      create_third_cont = true;
      for( int j = 0; j <= i; ++j ){
        create_fourth_cont = true;
        ij_index = ( i * ( i + 1 ) ) / 2 + j;
        if( ij_index >= kl_index ){
          mo_index = ( ij_index * ( ij_index + 1 ) ) / 2 + kl_index;
          val = quarter_gamma_transform( k, l, i, j, create_first_cont, create_second_cont, create_third_cont, create_fourth_cont );
          if( fabs( val ) > dtol ){
            i++; j++; k++; l++;
            append_moint_to_file_( fileholder, &val, &i, &j, &k, &l );
            i--; j--; k--; l--;
          }
          create_first_cont  = false;
          create_second_cont = false;
          create_third_cont  = false;
          create_fourth_cont = false;
        }
      }
      }
    }
    }
    int zero = 0;
    for( int i = 0; i < nmo_scell; ++i ){
      val = evals.irrep( 0 )( i, 0 );
      i++; 
      append_moint_to_file_( fileholder, &val, &i, &i, &zero, &zero );
      i--;
    }
    val = 0.0;
    append_moint_to_file_( fileholder, &val, &zero, &zero, &zero, &zero );
    append_moint_to_file_( fileholder, &val, &zero, &zero, &zero, &zero );
}








void moIntegralFactory::write_gamma_mointb_ind_p( 
    std::string ofilename,
    int tol
){
    int ij_index, kl_index, mo_index;
    double val;
    FILE* oFile;
    printf( "WRITING MOINTEGRAL BINARY FILE.... \n" );

    bool create_new_kl;
    bool create_first_cont,
         create_second_cont,
         create_third_cont,
         create_fourth_cont;
    double dtol = pow( 10., 1. * tol );
    for( int i = 0; i < nmo_scell; ++i ){
      std::string filename;
      std::stringstream ss;
      ss << ofilename << i;
      ss >> filename;
      oFile = fopen( filename.c_str(), "wb" );
      create_first_cont = true;
      printf( "WRITING MOINTEGRAL BINARY FILE '%s'.... ( %4d / %4d ) \n", filename.c_str(), i, nmo_scell ); cout << flush;
      for( int j = 0; j <= i; ++j ){
        create_second_cont = true;
        ij_index = ( i * ( i + 1 ) ) / 2 + j;
        create_new_kl = true;
        for( int k = 0; k < nmo_scell; ++k ){
        create_third_cont = true;
        for( int l = 0; l <= k; ++l ){
          create_fourth_cont = true;
          kl_index = ( k * ( k + 1 ) ) / 2 + l;
//          if( ij_index >= kl_index ){
          if( i >= k ){
            mo_index = ( ij_index * ( ij_index + 1 ) ) / 2 + kl_index;
            val = quarter_gamma_transform( i, j, k, l, create_first_cont, create_second_cont, create_third_cont, create_fourth_cont );
            if( fabs( val ) > dtol ){
              fwrite( &i, sizeof( int ), 1, oFile );
              fwrite( &j, sizeof( int ), 1, oFile );
              fwrite( &k, sizeof( int ), 1, oFile );
              fwrite( &l, sizeof( int ), 1, oFile );
              fwrite( &val, sizeof( double ), 1, oFile );
            }
     
            create_first_cont  = false;
            create_second_cont = false;
            create_third_cont  = false;
            create_fourth_cont = false;
          }
        }
        }
      }
      fclose( oFile );
    }
}


void moIntegralFactory::vec_gamma_mointb_ind_p( 
    const int& p1,
    const int& p2,
    std::vector< double >& dble_arr,
    std::vector< size_t >& indx_arr,
    size_t& size_arr,
    int tol
){
    int ij_index, kl_index, mo_index;
    double val;

    bool create_new_kl;
    bool create_first_cont,
         create_second_cont,
         create_third_cont,
         create_fourth_cont;
    double dtol = pow( 10., 1. * tol );
    int counter = 0;
    size_t dble_size = dble_arr.size();

    for( int i = p1; i < p2; ++i ){
      create_first_cont = true;
      for( int j = 0; j <= i; ++j ){
        create_second_cont = true;
        ij_index = ( i * ( i + 1 ) ) / 2 + j;
        create_new_kl = true;
        for( int k = 0; k < nmo_scell; ++k ){
        create_third_cont = true;
        for( int l = 0; l <= k; ++l ){
          create_fourth_cont = true;
          kl_index = ( k * ( k + 1 ) ) / 2 + l;
          if( i >= k ){
            mo_index = ( ij_index * ( ij_index + 1 ) ) / 2 + kl_index;
            val = quarter_gamma_transform( i, j, k, l, create_first_cont, create_second_cont, create_third_cont, create_fourth_cont );
            if( fabs( val ) > dtol ){
              //fwrite( &i, sizeof( int ), 1, oFile );
              //fwrite( &j, sizeof( int ), 1, oFile );
              //fwrite( &k, sizeof( int ), 1, oFile );
              //fwrite( &l, sizeof( int ), 1, oFile );
              //fwrite( &val, sizeof( double ), 1, oFile );
              if( counter < dble_size ){
                indx_arr[ counter ] = get_ijkl( i, j, k, l ); 
                dble_arr[ counter ] = val;
              }else{
                indx_arr.push_back( get_ijkl( i, j, k, l ) ); 
                dble_arr.push_back( val );
                dble_size++;
              }
              counter++;
            }
     
            create_first_cont  = false;
            create_second_cont = false;
            create_third_cont  = false;
            create_fourth_cont = false;
          }
        }
        }
      }
    }
    size_arr = counter;
}
