#define N_WAVE          1024
#define LOG2_N_WAVE     10

typedef short fixed;

fixed fix_mpy(fixed a, fixed b) {
  return ((long)(a) * (long)(b))>>15;
}

fixed foo2(fixed a) {
  return fix_mpy(a, a * 2) + fix_mpy(a, a * 3);
}

void foo3(fixed* q, fixed* w, int m) {
  int i;
  for (i = 0; i < m; ++i) {
    for (int j = 0; j < i; ++j) {
      for (int k = 0; k < j; ++k) {
        q[i] = m * w[j] + q[i-2];
        q[k + 1] = w[j];
      }
    }
  }
}

int fix_fft(fixed fr[], fixed fi[], fixed* Sinewave, int m, int inverse)
{
  int mr,nn,i,j,l,k,istep, n, scale, shift;
  fixed qr,qi,tr,ti,wr,wi,t;

    n = 1<<m;

  if(n > N_WAVE)
    return -1;

  mr = 0;
  nn = n - 1;
  scale = 0;

  foo3(fr, fi, m);

  for(m=1; m<=nn; ++m) {
    l = n;
    do {
      l >>= 1;
    } while(mr+l > nn);
    mr = foo2((mr & (l-1)) + l);

    if(mr <= m) continue;
    tr = fr[m];
    fr[m] = fr[mr];
    fr[mr] = tr;
    ti = fi[m];
    fi[m] = fi[mr];
    fi[mr] = ti;
  }

  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, fi, m);
  foo3(fr, fi, m);
  foo3(fr, fi, m);
  foo3(fr, fi, m);
  foo3(fr, fi, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  foo3(fr, fi, m);
  foo3(fr, Sinewave, m);
  l = 1;
  k = LOG2_N_WAVE-1;
  while(l < n) {
    if(inverse) {

      shift = 0;
      for(i=0; i<n; ++i) {
        foo3(fr, fi, m);
        foo3(fr, Sinewave, m);
        j = fr[i];
        if(j < 0)
          j = -j;
        m = fi[i];
        if(m < 0)
          m = -m;
        if(j > 16383 || m > 16383) {
          shift = 1;
          break;
        }
      }
      if(shift)
        ++scale;
    } else {

      shift = 1;
    }
    istep = l << 1;
    for(m=0; m<l; ++m) {
      j = m << k;

      wr =  Sinewave[j+N_WAVE/4];
      wi = -Sinewave[j];
      if(inverse)
        wi = -wi;
      if(shift) {
        foo3(fr, fi, m);
        foo3(fr, Sinewave, m);
        wr >>= 1;
        wi >>= 1;
      }
      for(i=m; i<n; i+=istep) {
        j = i + l;
          tr = fix_mpy(wr,fr[j]) - fix_mpy(wi,fi[j]);
          ti = fix_mpy(wr,fi[j]) + fix_mpy(wi,fr[j]);
        foo3(fr, fi, m);
        foo3(fr, Sinewave, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, Sinewave, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, fi, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        foo3(fr, Sinewave, m);
        qr = fr[i];
        qi = fi[i];
        if(shift) {
          qr >>= 1;
          qi >>= 1;
        }
        fr[j] = qr - tr;
        fi[j] = qi - ti;
        fr[i] = qr + tr;
        fi[i] = qi + ti;
      }
    }
    --k;
    l = istep;
  }

  return scale;
}
