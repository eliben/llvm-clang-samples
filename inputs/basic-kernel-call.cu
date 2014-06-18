__attribute__((global)) void kernel(int* in, int* out) {}


void host_func_foobar(int* inbuf, int* outbuf) {
  kernel<<<1, 32>>>(inbuf, outbuf);
}
