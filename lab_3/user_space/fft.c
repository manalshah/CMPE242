/*******************************************************************************
 * Program is for CMPE 245 class use, see Dr. Harry Li's lecture notes for details
 * Reference: Digital Signal Processing, by A.V. Oppenhaim;
 * fft.c for calculting 4 points input, but you can easily expand this to 2^x inputs;
 * Version: x0.1; Date: Sept. 2009;
 *
 * Modified by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 ******************************************************************************/
 
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "fft.h"


void FFT(Complex * X, uint16_t N ) {
	Complex U, W, T, Tmp;
	uint16_t M = (uint16_t)log2(N) ;
	uint16_t i = 1, j = 1 ;
	int k = 1; int LE = 0, LE1 = 0; int IP = 0;

	for (k = 1; k <= M; k++) {
		LE = pow(2, M + 1 - k);
		LE1 = LE / 2;
		U.r = 1.0;
		U.i = 0.0;
		W.r = cos(M_PI / (double)LE1);
		W.i = -sin(M_PI/ (double)LE1);

		for (j = 1; j <= LE1; j++) {
			for (i = j; i <= N; i = i + LE) {
				IP = i + LE1;
				T.r = X[i].r + X[IP].r;
				T.i = X[i].i + X[IP].i;
				Tmp.r = X[i].r - X[IP].r;
				Tmp.i = X[i].i - X[IP].i;
				X[IP].r = (Tmp.r * U.r) - (Tmp.i * U.i);
				X[IP].i = (Tmp.r * U.i) + (Tmp.i * U.r);
				X[i].r = T.r;
				X[i].i = T.i;
			}
		}
	}

	Tmp.r = (U.r * W.r) - (U.i * W.i);
	Tmp.i = (U.r * W.i) + (U.i * W.r);
	U.r = Tmp.r;
	U.i = Tmp.i;

	int NV2 = N / 2;
	int NM1 = N - 1;
	int K = 0;
	j = 1;

	for (i = 1; i <= NM1; i++) {
		if (i >= j) goto TAG25;
		T.r = X[j].r;
		T.i = X[j].i;
		X[j].r = X[i].r; X[j].i = X[i].i; X[i].r = T.r; X[i].i = T.i;
		TAG25: K=NV2;
		TAG26: if (K >= j) goto TAG30;
		j = j - K;
		K = K / 2; goto TAG26;
		TAG30: j=j+K;
	}
}
