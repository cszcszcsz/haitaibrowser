# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'openh264_common_includes': [
      'src/codec/api/svc',
      'src/codec/common/inc',
      'src/codec/common/src',
    ],
    'openh264_common_sources': [
      'src/codec/common/inc/WelsThreadLib.h',
      'src/codec/common/inc/copy_mb.h',
      'src/codec/common/inc/cpu.h',
      'src/codec/common/inc/cpu_core.h',
      'src/codec/common/inc/crt_util_safe_x.h',
      'src/codec/common/inc/deblocking_common.h',
      'src/codec/common/inc/expand_pic.h',
      'src/codec/common/inc/golomb_common.h',
      'src/codec/common/inc/intra_pred_common.h',
      'src/codec/common/inc/ls_defines.h',
      'src/codec/common/inc/macros.h',
      'src/codec/common/inc/mc.h',
      'src/codec/common/inc/measure_time.h',
      'src/codec/common/inc/memory_align.h',
      'src/codec/common/inc/sad_common.h',
      'src/codec/common/inc/typedefs.h',
      'src/codec/common/inc/utils.h',
      'src/codec/common/inc/version.h',
      'src/codec/common/inc/welsCodecTrace.h',
      'src/codec/common/inc/wels_common_defs.h',
      'src/codec/common/inc/wels_const_common.h',
      'src/codec/common/src/WelsThreadLib.cpp',
      'src/codec/common/src/common_tables.cpp',
      'src/codec/common/src/copy_mb.cpp',
      'src/codec/common/src/cpu.cpp',
      'src/codec/common/src/crt_util_safe_x.cpp',
      'src/codec/common/src/deblocking_common.cpp',
      'src/codec/common/src/expand_pic.cpp',
      'src/codec/common/src/intra_pred_common.cpp',
      'src/codec/common/src/mc.cpp',
      'src/codec/common/src/memory_align.cpp',
      'src/codec/common/src/sad_common.cpp',
      'src/codec/common/src/utils.cpp',
      'src/codec/common/src/welsCodecTrace.cpp',
    ],
    # TODO(hbos): compile with *_asm_* sources
    'openh264_common_sources_asm_x86': [
      'src/codec/common/src/x86/cpuid.asm',
      'src/codec/common/src/x86/deblock.asm',
      'src/codec/common/src/x86/expand_picture.asm',
      'src/codec/common/src/x86/intra_pred_com.asm',
      'src/codec/common/src/x86/mb_copy.asm',
      'src/codec/common/src/x86/mc_chroma.asm',
      'src/codec/common/src/x86/mc_luma.asm',
      'src/codec/common/src/x86/satd_sad.asm',
      'src/codec/common/src/x86/vaa.asm',
    ],
    'openh264_common_sources_asm_arm': [
      'src/codec/common/src/arm/copy_mb_neon.S',
      'src/codec/common/src/arm/deblocking_neon.S',
      'src/codec/common/src/arm/expand_picture_neon.S',
      'src/codec/common/src/arm/intra_pred_common_neon.S',
      'src/codec/common/src/arm/mc_neon.S',
    ],
    'openh264_common_sources_asm_arm64': [
      'src/codec/common/src/arm64/copy_mb_aarch64_neon.S',
      'src/codec/common/src/arm64/deblocking_aarch64_neon.S',
      'src/codec/common/src/arm64/expand_picture_aarch64_neon.S',
      'src/codec/common/src/arm64/intra_pred_common_aarch64_neon.S',
      'src/codec/common/src/arm64/mc_aarch64_neon.S',
    ],

    'openh264_processing_includes': [
      'src/codec/api/svc',
      'src/codec/common/inc',
      'src/codec/common/src',
      'src/codec/processing/interface',
      'src/codec/processing/interface/',
      'src/codec/processing/src/adaptivequantization',
      'src/codec/processing/src/backgrounddetection',
      'src/codec/processing/src/common',
      'src/codec/processing/src/complexityanalysis',
      'src/codec/processing/src/denoise',
      'src/codec/processing/src/downsample',
      'src/codec/processing/src/imagerotate',
      'src/codec/processing/src/scenechangedetection',
      'src/codec/processing/src/scrolldetection',
      'src/codec/processing/src/vaacalc',
    ],
    'openh264_processing_sources': [
      'src/codec/processing/interface/IWelsVP.h',
      'src/codec/processing/src/adaptivequantization/AdaptiveQuantization.cpp',
      'src/codec/processing/src/adaptivequantization/AdaptiveQuantization.h',
      'src/codec/processing/src/backgrounddetection/BackgroundDetection.cpp',
      'src/codec/processing/src/backgrounddetection/BackgroundDetection.h',
      'src/codec/processing/src/common/WelsFrameWork.cpp',
      'src/codec/processing/src/common/WelsFrameWork.h',
      'src/codec/processing/src/common/WelsFrameWorkEx.cpp',
      'src/codec/processing/src/common/common.h',
      'src/codec/processing/src/common/memory.cpp',
      'src/codec/processing/src/common/memory.h',
      'src/codec/processing/src/common/resource.h',
      'src/codec/processing/src/common/typedef.h',
      'src/codec/processing/src/common/util.h',
      'src/codec/processing/src/complexityanalysis/ComplexityAnalysis.cpp',
      'src/codec/processing/src/complexityanalysis/ComplexityAnalysis.h',
      'src/codec/processing/src/denoise/denoise.cpp',
      'src/codec/processing/src/denoise/denoise.h',
      'src/codec/processing/src/denoise/denoise_filter.cpp',
      'src/codec/processing/src/downsample/downsample.cpp',
      'src/codec/processing/src/downsample/downsample.h',
      'src/codec/processing/src/downsample/downsamplefuncs.cpp',
      'src/codec/processing/src/imagerotate/imagerotate.cpp',
      'src/codec/processing/src/imagerotate/imagerotate.h',
      'src/codec/processing/src/imagerotate/imagerotatefuncs.cpp',
      'src/codec/processing/src/scenechangedetection/SceneChangeDetection.cpp',
      'src/codec/processing/src/scenechangedetection/SceneChangeDetection.h',
      'src/codec/processing/src/scrolldetection/ScrollDetection.cpp',
      'src/codec/processing/src/scrolldetection/ScrollDetection.h',
      'src/codec/processing/src/scrolldetection/ScrollDetectionFuncs.cpp',
      'src/codec/processing/src/scrolldetection/ScrollDetectionFuncs.h',
      'src/codec/processing/src/vaacalc/vaacalcfuncs.cpp',
      'src/codec/processing/src/vaacalc/vaacalculation.cpp',
      'src/codec/processing/src/vaacalc/vaacalculation.h',
    ],
    # TODO(hbos): compile with *_asm_* sources
    'openh264_processing_sources_asm_x86': [
      'src/codec/processing/src/x86/denoisefilter.asm',
      'src/codec/processing/src/x86/downsample_bilinear.asm',
      'src/codec/processing/src/x86/vaa.asm',
    ],
    'openh264_processing_sources_asm_arm': [
      'src/codec/processing/src/arm/adaptive_quantization.S',
      'src/codec/processing/src/arm/down_sample_neon.S',
      'src/codec/processing/src/arm/pixel_sad_neon.S',
      'src/codec/processing/src/arm/vaa_calc_neon.S',
    ],
    'openh264_processing_sources_asm_arm64': [
      'src/codec/processing/src/arm64/adaptive_quantization_aarch64_neon.S',
      'src/codec/processing/src/arm64/down_sample_aarch64_neon.S',
      'src/codec/processing/src/arm64/pixel_sad_aarch64_neon.S',
      'src/codec/processing/src/arm64/vaa_calc_aarch64_neon.S',
    ],

    'openh264_encoder_includes': [
      'src/codec/api/svc',
      'src/codec/common/inc',
      'src/codec/common/src',
      'src/codec/encoder/core/inc',
      'src/codec/encoder/core/src',
      'src/codec/encoder/plus/inc',
      'src/codec/encoder/plus/src',
      'src/codec/processing/interface/',
    ],
    'openh264_encoder_sources': [
      'src/codec/encoder/core/inc/as264_common.h',
      'src/codec/encoder/core/inc/au_set.h',
      'src/codec/encoder/core/inc/deblocking.h',
      'src/codec/encoder/core/inc/decode_mb_aux.h',
      'src/codec/encoder/core/inc/dq_map.h',
      'src/codec/encoder/core/inc/encode_mb_aux.h',
      'src/codec/encoder/core/inc/encoder.h',
      'src/codec/encoder/core/inc/encoder_context.h',
      'src/codec/encoder/core/inc/extern.h',
      'src/codec/encoder/core/inc/get_intra_predictor.h',
      'src/codec/encoder/core/inc/mb_cache.h',
      'src/codec/encoder/core/inc/md.h',
      'src/codec/encoder/core/inc/mt_defs.h',
      'src/codec/encoder/core/inc/mv_pred.h',
      'src/codec/encoder/core/inc/nal_encap.h',
      'src/codec/encoder/core/inc/param_svc.h',
      'src/codec/encoder/core/inc/parameter_sets.h',
      'src/codec/encoder/core/inc/picture.h',
      'src/codec/encoder/core/inc/picture_handle.h',
      'src/codec/encoder/core/inc/property.h',
      'src/codec/encoder/core/inc/rc.h',
      'src/codec/encoder/core/inc/ref_list_mgr_svc.h',
      'src/codec/encoder/core/inc/sample.h',
      'src/codec/encoder/core/inc/set_mb_syn_cabac.h',
      'src/codec/encoder/core/inc/set_mb_syn_cavlc.h',
      'src/codec/encoder/core/inc/slice.h',
      'src/codec/encoder/core/inc/slice_multi_threading.h',
      'src/codec/encoder/core/inc/stat.h',
      'src/codec/encoder/core/inc/svc_base_layer_md.h',
      'src/codec/encoder/core/inc/svc_enc_frame.h',
      'src/codec/encoder/core/inc/svc_enc_golomb.h',
      'src/codec/encoder/core/inc/svc_enc_macroblock.h',
      'src/codec/encoder/core/inc/svc_enc_slice_segment.h',
      'src/codec/encoder/core/inc/svc_encode_mb.h',
      'src/codec/encoder/core/inc/svc_encode_slice.h',
      'src/codec/encoder/core/inc/svc_mode_decision.h',
      'src/codec/encoder/core/inc/svc_motion_estimate.h',
      'src/codec/encoder/core/inc/svc_set_mb_syn.h',
      'src/codec/encoder/core/inc/svc_set_mb_syn_cavlc.h',
      'src/codec/encoder/core/inc/vlc_encoder.h',
      'src/codec/encoder/core/inc/wels_common_basis.h',
      'src/codec/encoder/core/inc/wels_const.h',
      'src/codec/encoder/core/inc/wels_func_ptr_def.h',
      'src/codec/encoder/core/inc/wels_preprocess.h',
      'src/codec/encoder/core/inc/wels_transpose_matrix.h',
      'src/codec/encoder/core/src/au_set.cpp',
      'src/codec/encoder/core/src/deblocking.cpp',
      'src/codec/encoder/core/src/decode_mb_aux.cpp',
      'src/codec/encoder/core/src/encode_mb_aux.cpp',
      'src/codec/encoder/core/src/encoder.cpp',
      'src/codec/encoder/core/src/encoder_data_tables.cpp',
      'src/codec/encoder/core/src/encoder_ext.cpp',
      'src/codec/encoder/core/src/get_intra_predictor.cpp',
      'src/codec/encoder/core/src/md.cpp',
      'src/codec/encoder/core/src/mv_pred.cpp',
      'src/codec/encoder/core/src/nal_encap.cpp',
      'src/codec/encoder/core/src/picture_handle.cpp',
      'src/codec/encoder/core/src/property.cpp',
      'src/codec/encoder/core/src/ratectl.cpp',
      'src/codec/encoder/core/src/ref_list_mgr_svc.cpp',
      'src/codec/encoder/core/src/sample.cpp',
      'src/codec/encoder/core/src/set_mb_syn_cabac.cpp',
      'src/codec/encoder/core/src/set_mb_syn_cavlc.cpp',
      'src/codec/encoder/core/src/slice_multi_threading.cpp',
      'src/codec/encoder/core/src/svc_base_layer_md.cpp',
      'src/codec/encoder/core/src/svc_enc_slice_segment.cpp',
      'src/codec/encoder/core/src/svc_encode_mb.cpp',
      'src/codec/encoder/core/src/svc_encode_slice.cpp',
      'src/codec/encoder/core/src/svc_mode_decision.cpp',
      'src/codec/encoder/core/src/svc_motion_estimate.cpp',
      'src/codec/encoder/core/src/svc_set_mb_syn_cabac.cpp',
      'src/codec/encoder/core/src/svc_set_mb_syn_cavlc.cpp',
      'src/codec/encoder/core/src/wels_preprocess.cpp',
      'src/codec/encoder/plus/inc/welsEncoderExt.h',
      'src/codec/encoder/plus/src/welsEncoderExt.cpp',
      # Note: Purposefully excluded: 'src/codec/encoder/plus/src/DllEntry.cpp',
      # This file is not built by the OpenH264 original build files.
    ],
    # TODO(hbos): compile with *_asm_* sources
    'openh264_encoder_sources_asm_x86': [
      'src/codec/encoder/core/x86/coeff.asm',
      'src/codec/encoder/core/x86/dct.asm',
      'src/codec/encoder/core/x86/intra_pred.asm',
      'src/codec/encoder/core/x86/matrix_transpose.asm',
      'src/codec/encoder/core/x86/memzero.asm',
      'src/codec/encoder/core/x86/quant.asm',
      'src/codec/encoder/core/x86/sample_sc.asm',
      'src/codec/encoder/core/x86/score.asm',
    ],
    'openh264_encoder_sources_asm_arm': [
      'src/codec/encoder/core/arm/intra_pred_neon.S',
      'src/codec/encoder/core/arm/intra_pred_sad_3_opt_neon.S',
      'src/codec/encoder/core/arm/memory_neon.S',
      'src/codec/encoder/core/arm/pixel_neon.S',
      'src/codec/encoder/core/arm/reconstruct_neon.S',
      'src/codec/encoder/core/arm/svc_motion_estimation.S',
    ],
    'openh264_encoder_sources_asm_arm64': [
      'src/codec/encoder/core/arm64/intra_pred_aarch64_neon.S',
      'src/codec/encoder/core/arm64/intra_pred_sad_3_opt_aarch64_neon.S',
      'src/codec/encoder/core/arm64/memory_aarch64_neon.S',
      'src/codec/encoder/core/arm64/pixel_aarch64_neon.S',
      'src/codec/encoder/core/arm64/reconstruct_aarch64_neon.S',
      'src/codec/encoder/core/arm64/svc_motion_estimation_aarch64_neon.S',
    ],
  },
}