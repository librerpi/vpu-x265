#include <stdint.h>
#include <string.h>

#define X265_DEPTH 8

void partialButterfly32_half(uint32_t add, uint32_t shift);

void dct32(const int16_t* src, int16_t* dst, intptr_t srcStride) {
  const int shift_1st = 4 + X265_DEPTH - 8;
  const int shift_2nd = 11;
  uint16_t block[32][32];
  for (int i = 0; i < 4; i++) {
    memcpy(&block[i * 4], &src[i * srcStride], 4 * sizeof(int16_t));
  }
  __asm__ volatile ("v16ld HY(0++,0), (%0+=%1) REP16": : "r"(block), "r"(2*16):"memory");
  partialButterfly32_half(1 << (shift_1st-1), shift_1st);
  __asm__ volatile ("v32add HY(0++,0), HY(32++,0), 0 REP16");
  partialButterfly32_half(1 << (shift_2nd-1), shift_2nd);
  __asm__ volatile ("v16st HY(0++,0), (%0+=%1) REP16": : "r"(dst), "r"(2*16):"memory");

  __asm__ volatile ("v16ld HY(0++,0), (%0+=%1) REP16": : "r"(block + (2*32*16)), "r"(2*16):"memory");
  partialButterfly32_half(1 << (shift_1st-1), shift_1st);
  __asm__ volatile ("v32add HY(0++,0), HY(32++,0), 0 REP16");
  partialButterfly32_half(1 << (shift_2nd-1), shift_2nd);
  __asm__ volatile ("v16st HY(0++,0), (%0+=%1) REP16": : "r"(dst + (2*32*16)), "r"(2*16):"memory");
}
