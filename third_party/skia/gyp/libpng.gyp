# Copyright 2014 Google Inc.
#
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'skia_warnings_as_errors': 0,
  },
  'targets': [
    {
      'target_name': 'libpng',
      'type': 'none',
      'conditions': [
        [ 'skia_android_framework', {
            'dependencies':              [ 'android_deps.gyp:png' ],
            'export_dependent_settings': [ 'android_deps.gyp:png' ],
        },{
            'dependencies':              [ 'libpng.gyp:libpng_static' ],
            'export_dependent_settings': [ 'libpng.gyp:libpng_static' ],
        }]
      ]
    },
    {
      'target_name': 'libpng_static',
      'type': 'static_library',
      'standalone_static_library': 1,
      'include_dirs': [
        # Needed for generated pnglibconf.h and pngprefix.h
        '../third_party/libpng',
        '../third_party/externals/libpng',
      ],
      'dependencies': [
        'zlib.gyp:zlib',
      ],
      'export_dependent_settings': [
        'zlib.gyp:zlib',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '../third_party/externals/libpng',
          # Needed for generated pnglibconf.h and pngprefix.h
          '../third_party/libpng',
        ],
      },
      'cflags': [
        '-w',
        '-fvisibility=hidden',
      ],
      'sources': [
        '../third_party/externals/libpng/png.c',
        '../third_party/externals/libpng/pngerror.c',
        '../third_party/externals/libpng/pngget.c',
        '../third_party/externals/libpng/pngmem.c',
        '../third_party/externals/libpng/pngpread.c',
        '../third_party/externals/libpng/pngread.c',
        '../third_party/externals/libpng/pngrio.c',
        '../third_party/externals/libpng/pngrtran.c',
        '../third_party/externals/libpng/pngrutil.c',
        '../third_party/externals/libpng/pngset.c',
        '../third_party/externals/libpng/pngtrans.c',
        '../third_party/externals/libpng/pngwio.c',
        '../third_party/externals/libpng/pngwrite.c',
        '../third_party/externals/libpng/pngwtran.c',
        '../third_party/externals/libpng/pngwutil.c',
      ],
      'conditions': [
        [ '"x86" in skia_arch_type', {
          'defines': [
            'PNG_INTEL_SSE_OPT=1',
          ],
          'sources': [
            '../third_party/externals/libpng/contrib/intel/intel_init.c',
            '../third_party/externals/libpng/contrib/intel/filter_sse2_intrinsics.c',
          ],
        }],
        [ '(("arm64" == skia_arch_type) or                   \
            ("arm" == skia_arch_type and arm_neon == 1)) and \
           ("ios" != skia_os)', {
          'defines': [
            'PNG_ARM_NEON_OPT=2',
            'PNG_ARM_NEON_IMPLEMENTATION=1',
          ],
          'sources': [
            '../third_party/externals/libpng/arm/arm_init.c',
            '../third_party/externals/libpng/arm/filter_neon_intrinsics.c',
          ],
        }],
        [ '"ios" == skia_os', {
          'defines': [
            'PNG_ARM_NEON_OPT=0',
          ],
        }],
      ],
    }
  ]
}
