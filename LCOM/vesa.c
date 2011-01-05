#include <stdio.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/farptr.h>
#include <pc.h>
#include <stdlib.h>

typedef struct {
  unsigned char  VESASignature[4]     __attribute__ ((packed));
  unsigned short VESAVersion          __attribute__ ((packed));
  unsigned long  OEMStringPtr         __attribute__ ((packed));
  unsigned long  Capabilities         __attribute__ ((packed));
  unsigned long  VideoModePtr         __attribute__ ((packed));
  unsigned short TotalMemory          __attribute__ ((packed));
  unsigned short OemSoftwareRev       __attribute__ ((packed));
  unsigned long  OemVendorNamePtr     __attribute__ ((packed));
  unsigned long  OemProductNamePtr    __attribute__ ((packed));
  unsigned long  OemProductRevPtr     __attribute__ ((packed));
  unsigned char  Reserved[222]        __attribute__ ((packed));
  unsigned char  OemData[256]         __attribute__ ((packed));
} VESA_INFO;

typedef struct {
  unsigned short ModeAttributes        __attribute__ ((packed));
  unsigned char  WinAAttributes        __attribute__ ((packed));
  unsigned char  WinBAttributes        __attribute__ ((packed));
  unsigned short WinGranularity        __attribute__ ((packed));
  unsigned short WinSize               __attribute__ ((packed));
  unsigned short WinASegment           __attribute__ ((packed));
  unsigned short WinBSegment           __attribute__ ((packed));
  unsigned long  WinFuncPtr            __attribute__ ((packed));
  unsigned short BytesPerScanLine      __attribute__ ((packed));
  unsigned short XResolution           __attribute__ ((packed));
  unsigned short YResolution           __attribute__ ((packed));
  unsigned char  XCharSize             __attribute__ ((packed));
  unsigned char  YCharSize             __attribute__ ((packed));
  unsigned char  NumberOfPlanes        __attribute__ ((packed));
  unsigned char  BitsPerPixel          __attribute__ ((packed));
  unsigned char  NumberOfBanks         __attribute__ ((packed));
  unsigned char  MemoryModel           __attribute__ ((packed));
  unsigned char  BankSize              __attribute__ ((packed));
  unsigned char  NumberOfImagePages    __attribute__ ((packed));
  unsigned char  Reserved_page         __attribute__ ((packed));
  unsigned char  RedMaskSize           __attribute__ ((packed));
  unsigned char  RedMaskPos            __attribute__ ((packed));
  unsigned char  GreenMaskSize         __attribute__ ((packed));
  unsigned char  GreenMaskPos          __attribute__ ((packed));
  unsigned char  BlueMaskSize          __attribute__ ((packed));
  unsigned char  BlueMaskPos           __attribute__ ((packed));
  unsigned char  RsvdMaskSize          __attribute__ ((packed));
  unsigned char  RsvdMaskPos           __attribute__ ((packed));
  unsigned char  DirectColorModeInfo   __attribute__ ((packed));
  unsigned long  PhysBasePtr           __attribute__ ((packed));
  unsigned long  OffScreenMemOffset    __attribute__ ((packed));
  unsigned short OffScreenMemSize      __attribute__ ((packed));
  unsigned short LinBytesPerScanLine   __attribute__ ((packed));
  unsigned char  BnkNumberOfImagePages __attribute__ ((packed));
  unsigned char  LinNumberOfImagePages __attribute__ ((packed));
  unsigned char  LinRedMaskSize        __attribute__ ((packed));
  unsigned char  LinRedFieldPosition   __attribute__ ((packed));
  unsigned char  LinGreenMaskSize      __attribute__ ((packed));
  unsigned char  LinGreenFieldPosition __attribute__ ((packed));
  unsigned char  LinBlueMaskSize       __attribute__ ((packed));
  unsigned char  LinBlueFieldPosition  __attribute__ ((packed));
  unsigned char  LinRsvdMaskSize       __attribute__ ((packed));
  unsigned char  LinRsvdFieldPosition  __attribute__ ((packed));
  unsigned long  MaxPixelClock         __attribute__ ((packed));
  unsigned char  Reserved[189]         __attribute__ ((packed));
} MODE_INFO;


int get_vesa_info(VESA_INFO *);
void print_vesa(VESA_INFO *, int *);
void print_modes(int *);

int main(void)
{
  VESA_INFO info;
  int modes[100];

  if (get_vesa_info(&info)==0) {
    printf("VESA supported.\n");
    print_vesa(&info, modes);
    print_modes(modes);
  }
  else
    printf("VESA not supported.\n");
  return 0;
}

int get_vesa_info(VESA_INFO *vesa_buffer)
{
  __dpmi_regs r;
  long dosbuf;
  int c;

  dosbuf = __tb & 0xFFFFF;
  for (c=0; c<sizeof(VESA_INFO); c++)
    _farpokeb(_dos_ds, dosbuf+c, 0);
  dosmemput("VBE2", 4, dosbuf);

  r.x.ax = 0x4F00;
  r.x.di = dosbuf & 0xF;
  r.x.es = (dosbuf>>4) & 0xFFFF;
  __dpmi_int(0x10, &r);
  if (r.h.ah)
    return -1;
  dosmemget(dosbuf, sizeof(VESA_INFO), vesa_buffer);
  if (strncmp(vesa_buffer->VESASignature, "VESA", 4) != 0)
    return -1;
  return 0;
}

void print_vesa(VESA_INFO *info, int *modes)
{
  char msg[256], c;
  long mem_ptr;
  int k;

  printf("  Signature: '%4.4s'\n", &(info->VESASignature));
  printf("  Version: %d.%d\n", (int)(info->VESAVersion>>8), (int)(info->VESAVersion&0x00FF));
  k = 0;
  mem_ptr = ((info->OEMStringPtr&0xFFFF0000)>>12) + (info->OEMStringPtr&0xFFFF);
  while (k<255 && (c=_farpeekb(_dos_ds, mem_ptr))!=0) {
    msg[k++] = c;
    mem_ptr++;
  }
  msg[k] = 0;
  printf("  Vendor: %s\n",  msg);
  printf("  Modes supported:");
  mem_ptr = ((info->VideoModePtr & 0xFFFF0000) >> 12) + (info->VideoModePtr & 0xFFFF);
  k = 0;
  while ((modes[k] = _farpeekw(_dos_ds, mem_ptr)) != 0xFFFF) {
    printf(" %3X", modes[k++]);
    mem_ptr += 2;
    if (k % 10 == 0)
      printf("\n                  ");
  }
  if (k % 10 != 0)
    printf("\n");
  printf("  Capabilities (%X): ", info->Capabilities);
  if (info->Capabilities & 0x1)
    printf("DAC 8bit");
  else
    printf("DAC 6bit");
  if (info->Capabilities & 0x2)
    printf(", not VGA");
  else
    printf(", VGA");
  if (info->Capabilities & 0x4)
    printf(", RAMDAC must use blank bit");
  else
    printf(", normal RAMDAC");
  printf("\n  Memory: %d KB\n", info->TotalMemory*64);
  printf("  Vendor software revision: %d.%d\n", (int)(info->OemSoftwareRev>>8), (int)(info->OemSoftwareRev&0x00FF));
  k = 0;
  mem_ptr = ((info->OemVendorNamePtr&0xFFFF0000)>>12) + (info->OemVendorNamePtr&0xFFFF);
  while (k<255 && (c=_farpeekb(_dos_ds, mem_ptr))!=0) {
    msg[k++] = c;
    mem_ptr++;
  }
  msg[k] = 0;
  printf("  Vendor name: %s\n", msg);
  k = 0;
  mem_ptr = ((info->OemProductNamePtr&0xFFFF0000)>>12) + (info->OemProductNamePtr&0xFFFF);
  while (k<255 && (c=_farpeekb(_dos_ds, mem_ptr))!=0) {
    msg[k++] = c;
    mem_ptr++;
  }
  msg[k] = 0;
  printf("  Product name: %s\n", msg);
  k = 0;
  mem_ptr = ((info->OemProductRevPtr&0xFFFF0000)>>12) + (info->OemProductRevPtr&0xFFFF);
  while (k<255 && (c=_farpeekb(_dos_ds, mem_ptr))!=0) {
    msg[k++] = c;
    mem_ptr++;
  }
  msg[k] = 0;
  printf("  Product revision: %s\n", msg);
  getkey();
}

void print_modes(int *modes)
{
  int k;
  short m;
  MODE_INFO mi;
  __dpmi_regs r;
  long dosbuf;

  while (1) {
    system("cls");
    printf("Modes:");
    for (k=0; modes[k]!=0xFFFF; k++) {
      printf(" %X", modes[k]);
      if ((k+1)%15 == 0)
        printf("\n      ");
    }
    if (k%15)
      printf("\n");
    printf("\nMode (0-stops): ");
    scanf("%X", &m);
    printf("Chosen mode: %X\n\n", m);
    if (m==0)
      return;

    dosbuf = __tb & 0xFFFFF;
    for (k=0; k<sizeof(MODE_INFO); k++)
      _farpokeb(_dos_ds, dosbuf+k, 0);
    r.x.ax = 0x4F01;
    r.x.di = dosbuf & 0xF;
    r.x.es = (dosbuf>>4) & 0xFFFF;
    r.x.cx = m;
    __dpmi_int(0x10, &r);
    if (r.h.ah) {
      printf("Error in call\n");
      getkey();
      continue;
    }
    dosmemget(dosbuf, sizeof(MODE_INFO), &mi);
    printf("Mode flags: ");
    k = mi.ModeAttributes;
    if (k & 1) {
      printf("mode supported");
      if (k & 4) printf(", BIOS support");
      if (k & 8)
        printf(", color");
      else
        printf(", mono");
      if (k & 16)
        printf(", graphics");
      else
        printf(", text");
      if (!(k&64) && !(k&128)) printf(", windowed only");
      if (!(k&64) && (k&128)) printf(", windowed and linear");
      if ((k&64) && (k&128)) printf(", linear only");
    }
    else
      printf("mode not supported");
    printf("\n");
    k = mi.WinAAttributes;
    printf("Window A: ");
    if (k & 1) {
      printf("existent");
      if (k & 2) printf(" readable");
      if (k & 4) printf(" writeable");
    }
    else
      printf("nonexistent");
    printf("\n");
    k = mi.WinBAttributes;
    printf("Window B: ");
    if (k & 1) {
      printf("existent");
      if (k & 2) printf(" readable");
      if (k & 4) printf(" writeable");
    }
    else
      printf("nonexistent");
    printf("\n");
    printf("Granularity: %d KB  Window Size: %d KB\n", mi.WinGranularity, mi.WinSize);
    printf("Window A seg: %04Xh  Window B seg: %04Xh\n", mi.WinASegment, mi.WinBSegment);
    k = mi.WinFuncPtr;
    printf("Address of window function call: %04X:%04X\n", (k>>16)&0xFFFF, k&0xFFFF);
    printf("Banked bytes per scan line: %d\n", mi.BytesPerScanLine);
    printf("Resolution: %d X %d\n", mi.XResolution, mi.YResolution);
    printf("Char cell: %d X %d\n", mi.XCharSize, mi.YCharSize);
    printf("Bit planes: %d\n", mi.NumberOfPlanes);
    printf("Bits per pixel: %d\n", mi.BitsPerPixel);
    printf("Memory banks: %d  Size: %d KB\n", mi.NumberOfBanks, mi.BankSize);
    printf("Max. display page: %d\n", mi.NumberOfImagePages);
    printf("Memory model: ");
    switch (mi.MemoryModel) {
      case 0: printf("Text\n"); break;
      case 1: printf("CGA\n"); break;
      case 2: printf("Hercules\n"); break;
      case 3: printf("Planar\n"); break;
      case 4: printf("Packed pixel\n"); break;
      case 5: printf("Non-chain-4, 256 colours\n"); break;
      case 6:
        printf("Direct Colour:  ");
        printf("Banked: %d:%d:%d:%d   Linear: %d:%d:%d:%d\n", mi.RedMaskSize, mi.GreenMaskSize,
               mi.BlueMaskSize, mi.RsvdMaskSize, mi.LinRedMaskSize, mi.LinGreenMaskSize,
               mi.LinBlueMaskSize, mi.LinRsvdMaskSize);
        printf("Attributes: ");
        if (mi.DirectColorModeInfo & 1)
          printf("Colour ramp programmable");
        else
          printf("Colour ramp fixed");
        if (mi.DirectColorModeInfo & 2)
          printf(", Reserved bits usable");
        printf("\n");
        break;
      case 7:
        printf("YUV:  ");
        printf("Banked: %d:%d:%d:%d   Linear: %d:%d:%d:%d\n", mi.GreenMaskSize, mi.BlueMaskSize,
               mi.RedMaskSize, mi.RsvdMaskSize, mi.LinGreenMaskSize, mi.LinBlueMaskSize,
               mi.LinRedMaskSize, mi.LinRsvdMaskSize);
        break;
      default: printf("Other\n");
    }
    printf("Linear buffer address: %08X\n", mi.PhysBasePtr);
    printf("Linear bytes per scan line: %d\n", mi.LinBytesPerScanLine);
    printf("Max. pixel clock: %d Hz\n", mi.MaxPixelClock);
    getkey();
  }
}
