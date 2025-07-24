/************************************************************************
	fft.c

    FFT Audio Analysis
    Copyright (C) 2011 Simon Inns

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Email: simon.inns@gmail.com

************************************************************************/

#ifndef FFT_C
#define FFT_C

#include <htc.h> // Microchip C Compiler (MCC) için gerekli ba?l?k dosyas? olabilir, projenizin kurulumuna göre farkl?l?k gösterebilir.
#include "fft.h"


// fix_fft.c - Fixed-point in-place Fast Fourier Transform

// All data are fixed-point short integers, in which -32768
// to +32768 represent -1.0 to +1.0 respectively. Integer
// arithmetic is used for speed, instead of the more natural
// floating-point.
//
// For the forward FFT (time -> freq), fixed scaling is
// performed to prevent arithmetic overflow, and to map a 0dB
// sine/cosine wave (i.e. amplitude = 32767) to two -6dB freq
// coefficients.
//
// Written by:  Tom Roberts  11/8/89
// Made portable:  Malcolm Slaney 12/15/94 malcolm@interval.com
// Enhanced:  Dimitrios P. Bouras  14 Jun 2006 dbouras@ieee.org
// Ported to PIC18F:  Simon Inns 20110104

/*
  fix_fft() - perform forward fast Fourier transform.
  fr[n],fi[n] are real and imaginary arrays, both INPUT AND
  RESULT (in-place FFT), with 0 <= n < 2**m
*/
void fix_fft(int16_t fr[], int16_t fi[], int16_t m)
{
	int32_t mr = 0, nn, i, j, l, k, istep, n, shift;
	int16_t qr, qi, tr, ti, wr, wi;

	n = 1 << m; // FFT boyutu: 2^m
	nn = n - 1;

	/* max FFT size = N_WAVE */
	//if (n > N_WAVE) return -1; // Bu kontrol aktif de?il ama N_WAVE'den büyük FFT yap?lmamal?.

	/* decimation in time - re-order data */
	// Bit-tersine çevirme (bit-reversal) veya kelebek yap?s?na uygun veri yeniden düzenlemesi.
	// Bu ad?m, FFT algoritmas?n?n temelidir ve hesaplama verimlili?i için gereklidir.
	for (m=1; m<=nn; ++m)
	{
		l = n;
		do
		{
			l >>= 1;
		} while (mr+l > nn);
		
		mr = (mr & (l-1)) + l;
		if (mr <= m) continue;
		
		// Gerçek ve sanal k?s?mlar? yer de?i?tirme
		tr = fr[m];
		fr[m] = fr[mr];
		fr[mr] = tr;
		ti = fi[m];
		fi[m] = fi[mr];
		fi[mr] = ti;
	}

	l = 1;  
	k = LOG2_N_WAVE-1; // Döner faktör (twiddle factor) indeksleme için kullan?l?r.
	
	while (l < n) // FFT'nin log2(n) a?amas?
	{
		/*
		  fixed scaling, for proper normalization --
		  there will be log2(n) passes, so this results
		  in an overall factor of 1/n, distributed to
		  maximize arithmetic accuracy.

		  It may not be obvious, but the shift will be
		  performed on each data point exactly once,
		  during this pass.
		*/
		
		// De?i?kenler
		int32_t c;
		int16_t b;
		
		istep = l << 1; // Bir sonraki a?amadaki kelebek ad?m? (butterfly step)
		for (m=0; m<l; ++m)
		{
			j = m << k; // Döner faktör Sinewave tablosundan almak için indeks
			/* 0 <= j < N_WAVE/2 */
			// Döner faktörler: Cos(theta) ve -Sin(theta)
			wr =  Sinewave[j+N_WAVE/4]; // Cosine de?eri
			wi = -Sinewave[j];          // Negative Sine de?eri

			wr >>= 1; // Ölçeklendirme
			wi >>= 1; // Ölçeklendirme
			
			for (i=m; i<n; i+=istep) // Kelebek hesaplamalar?n? yap
			{
				j = i + l; // ?kinci kelebek giri? indeksi
				
				// Here I unrolled the multiplications to prevent overhead
				// for procedural calls (we don't need to be clever about 
				// the actual multiplications since the pic has an onboard
				// 8x8 multiplier in the ALU):
				
				// tr = (wr * fr[j]) - (wi * fi[j]); // Karma??k say? çarp?m?n?n gerçek k?sm?
				c = ((int32_t)wr * (int32_t)fr[j]);
				c = c >> 14; // Sabit nokta çarp?m? sonucu ölçeklendirme (Q15 * Q15 = Q30, Q15 için 15 sa?a kayd?rma, burada bir fazladan kayd?rma var)
				b = c & 0x01; // Yuvarlama için en dü?ük bit kontrolü
				tr = (c >> 1) + b; // Toplamda 15 sa?a kayd?rma: c>>14 ve c>>1
				
				c = ((int32_t)wi * (int32_t)fi[j]);
				c = c >> 14;
				b = c & 0x01;
				tr = tr - ((c >> 1) + b);
				
				// ti = (wr * fi[j]) + (wi * fr[j]); // Karma??k say? çarp?m?n?n sanal k?sm?
				c = ((int32_t)wr * (int32_t)fi[j]);
				c = c >> 14;
				b = c & 0x01;
				ti = (c >> 1) + b;
				
				c = ((int32_t)wi * (int32_t)fr[j]);
				c = c >> 14;
				b = c & 0x01;
				ti = ti + ((c >> 1) + b);
				
				// Kelebek hesaplamas?: (A + B*W) ve (A - B*W)
				qr = fr[i];
				qi = fi[i];
				qr >>= 1; // Normalizasyon için 2'ye bölme
				qi >>= 1; // Normalizasyon için 2'ye bölme

				fr[j] = qr - tr; // (A - B*W) gerçek k?s?m
				fi[j] = qi - ti; // (A - B*W) sanal k?s?m
				fr[i] = qr + tr; // (A + B*W) gerçek k?s?m
				fi[i] = qi + ti; // (A + B*W) sanal k?s?m
			}
		}
		
		--k;
		l = istep; // Bir sonraki a?amaya geç
	}
}


#endif