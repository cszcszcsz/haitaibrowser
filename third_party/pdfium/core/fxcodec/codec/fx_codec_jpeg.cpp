// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <setjmp.h>

#include "core/fxcodec/codec/codec_int.h"
#include "core/fxcodec/include/fx_codec.h"
#include "core/fxcrt/include/fx_safe_types.h"
#include "core/fxge/include/fx_dib.h"

extern "C" {
#undef FAR
#if defined(USE_SYSTEM_LIBJPEG)
#include <jpeglib.h>
#elif defined(USE_LIBJPEG_TURBO)
#include "third_party/libjpeg_turbo/jpeglib.h"
#else
#include "third_party/libjpeg/jpeglib.h"
#endif
}

extern "C" {
static void _JpegScanSOI(const uint8_t*& src_buf, uint32_t& src_size) {
  if (src_size == 0) {
    return;
  }
  uint32_t offset = 0;
  while (offset < src_size - 1) {
    if (src_buf[offset] == 0xff && src_buf[offset + 1] == 0xd8) {
      src_buf += offset;
      src_size -= offset;
      return;
    }
    offset++;
  }
}
};
extern "C" {
static void _src_do_nothing(struct jpeg_decompress_struct* cinfo) {}
};
extern "C" {
static void _error_fatal(j_common_ptr cinfo) {
  longjmp(*(jmp_buf*)cinfo->client_data, -1);
}
};
extern "C" {
static void _src_skip_data(struct jpeg_decompress_struct* cinfo, long num) {
  if (num > (long)cinfo->src->bytes_in_buffer) {
    _error_fatal((j_common_ptr)cinfo);
  }
  cinfo->src->next_input_byte += num;
  cinfo->src->bytes_in_buffer -= num;
}
};
extern "C" {
static boolean _src_fill_buffer(j_decompress_ptr cinfo) {
  return 0;
}
};
extern "C" {
static boolean _src_resync(j_decompress_ptr cinfo, int desired) {
  return 0;
}
};
extern "C" {
static void _error_do_nothing(j_common_ptr cinfo) {}
};
extern "C" {
static void _error_do_nothing1(j_common_ptr cinfo, int) {}
};
extern "C" {
static void _error_do_nothing2(j_common_ptr cinfo, char*) {}
};
#define JPEG_MARKER_EXIF (JPEG_APP0 + 1)
#define JPEG_MARKER_ICC (JPEG_APP0 + 2)
#define JPEG_MARKER_AUTHORTIME (JPEG_APP0 + 3)
#define JPEG_MARKER_MAXSIZE 0xFFFF
#define JPEG_OVERHEAD_LEN 14
static FX_BOOL _JpegEmbedIccProfile(j_compress_ptr cinfo,
                                    const uint8_t* icc_buf_ptr,
                                    uint32_t icc_length) {
  if (!icc_buf_ptr || icc_length == 0) {
    return FALSE;
  }
  uint32_t icc_segment_size = (JPEG_MARKER_MAXSIZE - 2 - JPEG_OVERHEAD_LEN);
  uint32_t icc_segment_num = (icc_length / icc_segment_size) + 1;
  if (icc_segment_num > 255) {
    return FALSE;
  }
  uint32_t icc_data_length =
      JPEG_OVERHEAD_LEN + (icc_segment_num > 1 ? icc_segment_size : icc_length);
  uint8_t* icc_data = FX_Alloc(uint8_t, icc_data_length);
  FXSYS_memcpy(icc_data, "\x49\x43\x43\x5f\x50\x52\x4f\x46\x49\x4c\x45\x00",
               12);
  icc_data[13] = (uint8_t)icc_segment_num;
  for (uint8_t i = 0; i < (icc_segment_num - 1); i++) {
    icc_data[12] = i + 1;
    FXSYS_memcpy(icc_data + JPEG_OVERHEAD_LEN,
                 icc_buf_ptr + i * icc_segment_size, icc_segment_size);
    jpeg_write_marker(cinfo, JPEG_MARKER_ICC, icc_data, icc_data_length);
  }
  icc_data[12] = (uint8_t)icc_segment_num;
  uint32_t icc_size = (icc_segment_num - 1) * icc_segment_size;
  FXSYS_memcpy(icc_data + JPEG_OVERHEAD_LEN, icc_buf_ptr + icc_size,
               icc_length - icc_size);
  jpeg_write_marker(cinfo, JPEG_MARKER_ICC, icc_data,
                    JPEG_OVERHEAD_LEN + icc_length - icc_size);
  FX_Free(icc_data);
  return TRUE;
}
extern "C" {
static void _dest_do_nothing(j_compress_ptr cinfo) {}
};
extern "C" {
static boolean _dest_empty(j_compress_ptr cinfo) {
  return FALSE;
}
};
#define JPEG_BLOCK_SIZE 1048576
static void _JpegEncode(const CFX_DIBSource* pSource,
                        uint8_t*& dest_buf,
                        FX_STRSIZE& dest_size,
                        int quality,
                        const uint8_t* icc_buf,
                        uint32_t icc_length) {
  struct jpeg_error_mgr jerr;
  jerr.error_exit = _error_do_nothing;
  jerr.emit_message = _error_do_nothing1;
  jerr.output_message = _error_do_nothing;
  jerr.format_message = _error_do_nothing2;
  jerr.reset_error_mgr = _error_do_nothing;

  struct jpeg_compress_struct cinfo;
  memset(&cinfo, 0, sizeof(cinfo));
  cinfo.err = &jerr;
  jpeg_create_compress(&cinfo);
  int Bpp = pSource->GetBPP() / 8;
  uint32_t nComponents = Bpp >= 3 ? (pSource->IsCmykImage() ? 4 : 3) : 1;
  uint32_t pitch = pSource->GetPitch();
  uint32_t width = pdfium::base::checked_cast<uint32_t>(pSource->GetWidth());
  uint32_t height = pdfium::base::checked_cast<uint32_t>(pSource->GetHeight());
  FX_SAFE_UINT32 safe_buf_len = width;
  safe_buf_len *= height;
  safe_buf_len *= nComponents;
  safe_buf_len += 1024;
  if (icc_length) {
    safe_buf_len += 255 * 18;
    safe_buf_len += icc_length;
  }
  uint32_t dest_buf_length = 0;
  if (!safe_buf_len.IsValid()) {
    dest_buf = nullptr;
  } else {
    dest_buf_length = safe_buf_len.ValueOrDie();
    dest_buf = FX_TryAlloc(uint8_t, dest_buf_length);
    const int MIN_TRY_BUF_LEN = 1024;
    while (!dest_buf && dest_buf_length > MIN_TRY_BUF_LEN) {
      dest_buf_length >>= 1;
      dest_buf = FX_TryAlloc(uint8_t, dest_buf_length);
    }
  }
  if (!dest_buf) {
    FX_OutOfMemoryTerminate();
  }
  struct jpeg_destination_mgr dest;
  dest.init_destination = _dest_do_nothing;
  dest.term_destination = _dest_do_nothing;
  dest.empty_output_buffer = _dest_empty;
  dest.next_output_byte = dest_buf;
  dest.free_in_buffer = dest_buf_length;
  cinfo.dest = &dest;
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = nComponents;
  if (nComponents == 1) {
    cinfo.in_color_space = JCS_GRAYSCALE;
  } else if (nComponents == 3) {
    cinfo.in_color_space = JCS_RGB;
  } else {
    cinfo.in_color_space = JCS_CMYK;
  }
  uint8_t* line_buf = NULL;
  if (nComponents > 1) {
    line_buf = FX_Alloc2D(uint8_t, width, nComponents);
  }
  jpeg_set_defaults(&cinfo);
  if (quality != 75) {
    jpeg_set_quality(&cinfo, quality, TRUE);
  }
  jpeg_start_compress(&cinfo, TRUE);
  _JpegEmbedIccProfile(&cinfo, icc_buf, icc_length);
  JSAMPROW row_pointer[1];
  JDIMENSION row;
  while (cinfo.next_scanline < cinfo.image_height) {
    const uint8_t* src_scan = pSource->GetScanline(cinfo.next_scanline);
    if (nComponents > 1) {
      uint8_t* dest_scan = line_buf;
      if (nComponents == 3) {
        for (uint32_t i = 0; i < width; i++) {
          dest_scan[0] = src_scan[2];
          dest_scan[1] = src_scan[1];
          dest_scan[2] = src_scan[0];
          dest_scan += 3;
          src_scan += Bpp;
        }
      } else {
        for (uint32_t i = 0; i < pitch; i++) {
          *dest_scan++ = ~*src_scan++;
        }
      }
      row_pointer[0] = line_buf;
    } else {
      row_pointer[0] = (uint8_t*)src_scan;
    }
    row = cinfo.next_scanline;
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
    if (cinfo.next_scanline == row) {
      dest_buf =
          FX_Realloc(uint8_t, dest_buf, dest_buf_length + JPEG_BLOCK_SIZE);
      dest.next_output_byte = dest_buf + dest_buf_length - dest.free_in_buffer;
      dest_buf_length += JPEG_BLOCK_SIZE;
      dest.free_in_buffer += JPEG_BLOCK_SIZE;
    }
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  FX_Free(line_buf);
  dest_size = dest_buf_length - (FX_STRSIZE)dest.free_in_buffer;
}

#ifdef PDF_ENABLE_XFA
static void _JpegLoadAttribute(struct jpeg_decompress_struct* pInfo,
                               CFX_DIBAttribute* pAttribute) {
  if (pInfo == NULL || pAttribute == NULL) {
    return;
  }
  if (pAttribute) {
    pAttribute->m_nXDPI = pInfo->X_density;
    pAttribute->m_nYDPI = pInfo->Y_density;
    pAttribute->m_wDPIUnit = pInfo->density_unit;
  }
}
#endif  // PDF_ENABLE_XFA

static FX_BOOL _JpegLoadInfo(const uint8_t* src_buf,
                             uint32_t src_size,
                             int& width,
                             int& height,
                             int& num_components,
                             int& bits_per_components,
                             FX_BOOL& color_transform,
                             uint8_t** icc_buf_ptr,
                             uint32_t* icc_length) {
  _JpegScanSOI(src_buf, src_size);
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  jerr.error_exit = _error_fatal;
  jerr.emit_message = _error_do_nothing1;
  jerr.output_message = _error_do_nothing;
  jerr.format_message = _error_do_nothing2;
  jerr.reset_error_mgr = _error_do_nothing;
  jerr.trace_level = 0;
  cinfo.err = &jerr;
  jmp_buf mark;
  cinfo.client_data = &mark;
  if (setjmp(mark) == -1) {
    return FALSE;
  }
  jpeg_create_decompress(&cinfo);
  struct jpeg_source_mgr src;
  src.init_source = _src_do_nothing;
  src.term_source = _src_do_nothing;
  src.skip_input_data = _src_skip_data;
  src.fill_input_buffer = _src_fill_buffer;
  src.resync_to_restart = _src_resync;
  src.bytes_in_buffer = src_size;
  src.next_input_byte = src_buf;
  cinfo.src = &src;
  if (setjmp(mark) == -1) {
    jpeg_destroy_decompress(&cinfo);
    return FALSE;
  }
  if (icc_buf_ptr && icc_length) {
    jpeg_save_markers(&cinfo, JPEG_MARKER_ICC, JPEG_MARKER_MAXSIZE);
  }
  int ret = jpeg_read_header(&cinfo, TRUE);
  if (ret != JPEG_HEADER_OK) {
    jpeg_destroy_decompress(&cinfo);
    return FALSE;
  }
  width = cinfo.image_width;
  height = cinfo.image_height;
  num_components = cinfo.num_components;
  color_transform =
      cinfo.jpeg_color_space == JCS_YCbCr || cinfo.jpeg_color_space == JCS_YCCK;
  bits_per_components = cinfo.data_precision;
  if (icc_buf_ptr) {
    *icc_buf_ptr = NULL;
  }
  if (icc_length) {
    *icc_length = 0;
  }
  jpeg_destroy_decompress(&cinfo);
  return TRUE;
}

class CCodec_JpegDecoder : public CCodec_ScanlineDecoder {
 public:
  CCodec_JpegDecoder();
  ~CCodec_JpegDecoder() override;

  FX_BOOL Create(const uint8_t* src_buf,
                 uint32_t src_size,
                 int width,
                 int height,
                 int nComps,
                 FX_BOOL ColorTransform);

  // CCodec_ScanlineDecoder
  FX_BOOL v_Rewind() override;
  uint8_t* v_GetNextLine() override;
  uint32_t GetSrcOffset() override;

  FX_BOOL InitDecode();

  jmp_buf m_JmpBuf;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  struct jpeg_source_mgr src;
  const uint8_t* m_SrcBuf;
  uint32_t m_SrcSize;
  uint8_t* m_pScanlineBuf;

  FX_BOOL m_bInited;
  FX_BOOL m_bStarted;
  FX_BOOL m_bJpegTransform;

 protected:
  uint32_t m_nDefaultScaleDenom;
};

CCodec_JpegDecoder::CCodec_JpegDecoder() {
  m_pScanlineBuf = NULL;
  m_bStarted = FALSE;
  m_bInited = FALSE;
  FXSYS_memset(&cinfo, 0, sizeof(cinfo));
  FXSYS_memset(&jerr, 0, sizeof(jerr));
  FXSYS_memset(&src, 0, sizeof(src));
  m_nDefaultScaleDenom = 1;
}
CCodec_JpegDecoder::~CCodec_JpegDecoder() {
  FX_Free(m_pScanlineBuf);
  if (m_bInited) {
    jpeg_destroy_decompress(&cinfo);
  }
}
FX_BOOL CCodec_JpegDecoder::InitDecode() {
  cinfo.err = &jerr;
  cinfo.client_data = &m_JmpBuf;
  if (setjmp(m_JmpBuf) == -1) {
    return FALSE;
  }
  jpeg_create_decompress(&cinfo);
  m_bInited = TRUE;
  cinfo.src = &src;
  src.bytes_in_buffer = m_SrcSize;
  src.next_input_byte = m_SrcBuf;
  if (setjmp(m_JmpBuf) == -1) {
    jpeg_destroy_decompress(&cinfo);
    m_bInited = FALSE;
    return FALSE;
  }
  cinfo.image_width = m_OrigWidth;
  cinfo.image_height = m_OrigHeight;
  int ret = jpeg_read_header(&cinfo, TRUE);
  if (ret != JPEG_HEADER_OK) {
    return FALSE;
  }
  if (cinfo.saw_Adobe_marker) {
    m_bJpegTransform = TRUE;
  }
  if (cinfo.num_components == 3 && !m_bJpegTransform) {
    cinfo.out_color_space = cinfo.jpeg_color_space;
  }
  m_OrigWidth = cinfo.image_width;
  m_OrigHeight = cinfo.image_height;
  m_OutputWidth = m_OrigWidth;
  m_OutputHeight = m_OrigHeight;
  m_nDefaultScaleDenom = cinfo.scale_denom;
  return TRUE;
}
FX_BOOL CCodec_JpegDecoder::Create(const uint8_t* src_buf,
                                   uint32_t src_size,
                                   int width,
                                   int height,
                                   int nComps,
                                   FX_BOOL ColorTransform) {
  _JpegScanSOI(src_buf, src_size);
  m_SrcBuf = src_buf;
  m_SrcSize = src_size;
  jerr.error_exit = _error_fatal;
  jerr.emit_message = _error_do_nothing1;
  jerr.output_message = _error_do_nothing;
  jerr.format_message = _error_do_nothing2;
  jerr.reset_error_mgr = _error_do_nothing;
  src.init_source = _src_do_nothing;
  src.term_source = _src_do_nothing;
  src.skip_input_data = _src_skip_data;
  src.fill_input_buffer = _src_fill_buffer;
  src.resync_to_restart = _src_resync;
  m_bJpegTransform = ColorTransform;
  if (src_size > 1 &&
      FXSYS_memcmp(src_buf + src_size - 2, "\xFF\xD9", 2) != 0) {
    ((uint8_t*)src_buf)[src_size - 2] = 0xFF;
    ((uint8_t*)src_buf)[src_size - 1] = 0xD9;
  }
  m_OutputWidth = m_OrigWidth = width;
  m_OutputHeight = m_OrigHeight = height;
  if (!InitDecode()) {
    return FALSE;
  }
  if (cinfo.num_components < nComps) {
    return FALSE;
  }
  if ((int)cinfo.image_width < width) {
    return FALSE;
  }
  m_Pitch =
      (static_cast<uint32_t>(cinfo.image_width) * cinfo.num_components + 3) /
      4 * 4;
  m_pScanlineBuf = FX_Alloc(uint8_t, m_Pitch);
  m_nComps = cinfo.num_components;
  m_bpc = 8;
  m_bStarted = FALSE;
  return TRUE;
}

FX_BOOL CCodec_JpegDecoder::v_Rewind() {
  if (m_bStarted) {
    jpeg_destroy_decompress(&cinfo);
    if (!InitDecode()) {
      return FALSE;
    }
  }
  if (setjmp(m_JmpBuf) == -1) {
    return FALSE;
  }
  cinfo.scale_denom = m_nDefaultScaleDenom;
  m_OutputWidth = m_OrigWidth;
  m_OutputHeight = m_OrigHeight;
  if (!jpeg_start_decompress(&cinfo)) {
    jpeg_destroy_decompress(&cinfo);
    return FALSE;
  }
  if ((int)cinfo.output_width > m_OrigWidth) {
    ASSERT(FALSE);
    return FALSE;
  }
  m_bStarted = TRUE;
  return TRUE;
}
uint8_t* CCodec_JpegDecoder::v_GetNextLine() {
  if (setjmp(m_JmpBuf) == -1)
    return nullptr;

  int nlines = jpeg_read_scanlines(&cinfo, &m_pScanlineBuf, 1);
  if (nlines < 1) {
    return nullptr;
  }
  return m_pScanlineBuf;
}
uint32_t CCodec_JpegDecoder::GetSrcOffset() {
  return (uint32_t)(m_SrcSize - src.bytes_in_buffer);
}
CCodec_ScanlineDecoder* CCodec_JpegModule::CreateDecoder(
    const uint8_t* src_buf,
    uint32_t src_size,
    int width,
    int height,
    int nComps,
    FX_BOOL ColorTransform) {
  if (!src_buf || src_size == 0) {
    return NULL;
  }
  CCodec_JpegDecoder* pDecoder = new CCodec_JpegDecoder;
  if (!pDecoder->Create(src_buf, src_size, width, height, nComps,
                        ColorTransform)) {
    delete pDecoder;
    return NULL;
  }
  return pDecoder;
}
FX_BOOL CCodec_JpegModule::LoadInfo(const uint8_t* src_buf,
                                    uint32_t src_size,
                                    int& width,
                                    int& height,
                                    int& num_components,
                                    int& bits_per_components,
                                    FX_BOOL& color_transform,
                                    uint8_t** icc_buf_ptr,
                                    uint32_t* icc_length) {
  return _JpegLoadInfo(src_buf, src_size, width, height, num_components,
                       bits_per_components, color_transform, icc_buf_ptr,
                       icc_length);
}
FX_BOOL CCodec_JpegModule::Encode(const CFX_DIBSource* pSource,
                                  uint8_t*& dest_buf,
                                  FX_STRSIZE& dest_size,
                                  int quality,
                                  const uint8_t* icc_buf,
                                  uint32_t icc_length) {
  if (pSource->GetBPP() < 8 || pSource->GetPalette())
    return FALSE;

  _JpegEncode(pSource, dest_buf, dest_size, quality, icc_buf, icc_length);
  return TRUE;
}
struct FXJPEG_Context {
  jmp_buf m_JumpMark;
  jpeg_decompress_struct m_Info;
  jpeg_error_mgr m_ErrMgr;
  jpeg_source_mgr m_SrcMgr;
  unsigned int m_SkipSize;
  void* (*m_AllocFunc)(unsigned int);
  void (*m_FreeFunc)(void*);
};
extern "C" {
static void _error_fatal1(j_common_ptr cinfo) {
  longjmp(((FXJPEG_Context*)cinfo->client_data)->m_JumpMark, -1);
}
};
extern "C" {
static void _src_skip_data1(struct jpeg_decompress_struct* cinfo, long num) {
  if (cinfo->src->bytes_in_buffer < (size_t)num) {
    ((FXJPEG_Context*)cinfo->client_data)->m_SkipSize =
        (unsigned int)(num - cinfo->src->bytes_in_buffer);
    cinfo->src->bytes_in_buffer = 0;
  } else {
    cinfo->src->next_input_byte += num;
    cinfo->src->bytes_in_buffer -= num;
  }
}
};
static void* jpeg_alloc_func(unsigned int size) {
  return FX_Alloc(char, size);
}
static void jpeg_free_func(void* p) {
  FX_Free(p);
}
FXJPEG_Context* CCodec_JpegModule::Start() {
  FXJPEG_Context* p = FX_Alloc(FXJPEG_Context, 1);
  p->m_AllocFunc = jpeg_alloc_func;
  p->m_FreeFunc = jpeg_free_func;
  p->m_ErrMgr.error_exit = _error_fatal1;
  p->m_ErrMgr.emit_message = _error_do_nothing1;
  p->m_ErrMgr.output_message = _error_do_nothing;
  p->m_ErrMgr.format_message = _error_do_nothing2;
  p->m_ErrMgr.reset_error_mgr = _error_do_nothing;
  p->m_SrcMgr.init_source = _src_do_nothing;
  p->m_SrcMgr.term_source = _src_do_nothing;
  p->m_SrcMgr.skip_input_data = _src_skip_data1;
  p->m_SrcMgr.fill_input_buffer = _src_fill_buffer;
  p->m_SrcMgr.resync_to_restart = _src_resync;
  p->m_Info.client_data = p;
  p->m_Info.err = &p->m_ErrMgr;
  if (setjmp(p->m_JumpMark) == -1) {
    return 0;
  }
  jpeg_create_decompress(&p->m_Info);
  p->m_Info.src = &p->m_SrcMgr;
  p->m_SkipSize = 0;
  return p;
}

void CCodec_JpegModule::Finish(FXJPEG_Context* ctx) {
  jpeg_destroy_decompress(&ctx->m_Info);
  ctx->m_FreeFunc(ctx);
}

void CCodec_JpegModule::Input(FXJPEG_Context* ctx,
                              const unsigned char* src_buf,
                              uint32_t src_size) {
  if (ctx->m_SkipSize) {
    if (ctx->m_SkipSize > src_size) {
      ctx->m_SrcMgr.bytes_in_buffer = 0;
      ctx->m_SkipSize -= src_size;
      return;
    }
    src_size -= ctx->m_SkipSize;
    src_buf += ctx->m_SkipSize;
    ctx->m_SkipSize = 0;
  }
  ctx->m_SrcMgr.next_input_byte = src_buf;
  ctx->m_SrcMgr.bytes_in_buffer = src_size;
}

#ifdef PDF_ENABLE_XFA
int CCodec_JpegModule::ReadHeader(FXJPEG_Context* ctx,
                                  int* width,
                                  int* height,
                                  int* nComps,
                                  CFX_DIBAttribute* pAttribute) {
#else   // PDF_ENABLE_XFA
int CCodec_JpegModule::ReadHeader(FXJPEG_Context* ctx,
                                  int* width,
                                  int* height,
                                  int* nComps) {
#endif  // PDF_ENABLE_XFA
  if (setjmp(ctx->m_JumpMark) == -1)
    return 1;

  int ret = jpeg_read_header(&ctx->m_Info, true);
  if (ret == JPEG_SUSPENDED)
    return 2;
  if (ret != JPEG_HEADER_OK)
    return 1;

  *width = ctx->m_Info.image_width;
  *height = ctx->m_Info.image_height;
  *nComps = ctx->m_Info.num_components;
#ifdef PDF_ENABLE_XFA
  _JpegLoadAttribute(&ctx->m_Info, pAttribute);
#endif
  return 0;
}

int CCodec_JpegModule::StartScanline(FXJPEG_Context* ctx, int down_scale) {
  if (setjmp(ctx->m_JumpMark) == -1)
    return 0;

  ctx->m_Info.scale_denom = down_scale;
  return jpeg_start_decompress(&ctx->m_Info);
}

FX_BOOL CCodec_JpegModule::ReadScanline(FXJPEG_Context* ctx,
                                        unsigned char* dest_buf) {
  if (setjmp(ctx->m_JumpMark) == -1)
    return FALSE;

  int nlines = jpeg_read_scanlines(&ctx->m_Info, &dest_buf, 1);
  return nlines == 1;
}

uint32_t CCodec_JpegModule::GetAvailInput(FXJPEG_Context* ctx,
                                          uint8_t** avail_buf_ptr) {
  if (avail_buf_ptr) {
    *avail_buf_ptr = NULL;
    if (ctx->m_SrcMgr.bytes_in_buffer > 0) {
      *avail_buf_ptr = (uint8_t*)ctx->m_SrcMgr.next_input_byte;
    }
  }
  return (uint32_t)ctx->m_SrcMgr.bytes_in_buffer;
}