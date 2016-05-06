#include <iostream>
#include <stdint.h>

#include "codecfactory.h"
#include "intersection.h"
#include "variablebyte.h"
#include "util.h"

using namespace SIMDCompressionLib;


// sorted
static shared_ptr<IntegerCODEC> codec_sorted =  CODECFactory::getFromName("s4-bp128-dm");

static CompositeCodec<SIMDBinaryPacking<SIMDBlockPacker<NoDelta, false>>, VariableByte<false>> composite_codec_unsorted = CompositeCodec<SIMDBinaryPacking<SIMDBlockPacker<NoDelta, false>>, VariableByte<false>>();

// variable byte
static VariableByte<false> codec_unsorted = VariableByte<false>();

static SIMDBinaryPacking<SIMDIntegratedBlockPacker<Max4DeltaSIMD, true>> simd_pack_sorted = SIMDBinaryPacking<SIMDIntegratedBlockPacker<Max4DeltaSIMD, true>>();

static SIMDBinaryPacking<SIMDBlockPacker<NoDelta, false>> simd_pack = SIMDBinaryPacking<SIMDBlockPacker<NoDelta, false>>();


static VariableByte<true> vint_codec = VariableByte<true>();
// SIMDBinaryPacking<SIMDBlockPacker<NoDelta, true>

extern "C" {


  // encode 128 u32 at a time.
  size_t encode_block128_native(
      uint32_t* begin,
      uint8_t* output,
      const size_t output_capacity) {
        size_t output_length = output_capacity / 4;
        simd_pack.encodeArray(begin,
                        128,
                        reinterpret_cast<uint32_t*>(output),
                        output_length);
        return output_length * 4;
  }

  // returns the number of byte that have been read.
  size_t decode_block128_native(
    const uint8_t* compressed_data,
    const size_t compressed_size,
    uint32_t* uncompressed) {
      size_t output_capacity = 128;
      const uint32_t* pointer_end = simd_pack.decodeArray(reinterpret_cast<const uint32_t*>(compressed_data), compressed_size / 4, uncompressed, output_capacity);
      const uint8_t* pointer_end_u8 = reinterpret_cast<const uint8_t*>(pointer_end);
      return static_cast<size_t>(pointer_end_u8 - compressed_data);

  }

  // encode 128 u32 at a time.
  size_t encode_sorted_block128_native(
      uint32_t* begin,
      uint8_t* output,
      const size_t output_capacity) {
        size_t output_length = output_capacity;
        simd_pack_sorted.encodeArray(begin,
                        128,
                        reinterpret_cast<uint32_t*>(output),
                        output_length);
        return output_length * 4;
  }

  // returns the number of byte that have been read.
  size_t decode_sorted_block128_native(
    const uint8_t* compressed_data,
    const size_t compressed_size,
    uint32_t* uncompressed) {
      size_t output_capacity = 128;
      const uint32_t* pointer_end = simd_pack_sorted.decodeArray(reinterpret_cast<const uint32_t*>(compressed_data), compressed_size / 4, uncompressed, output_capacity);
      const uint8_t* pointer_end_u8 = reinterpret_cast<const uint8_t*>(pointer_end);
      return static_cast<size_t>(pointer_end_u8 - compressed_data);

  }

  size_t encode_sorted_vint_native(
      uint32_t* begin,
      const size_t num_els,
      uint8_t* output,
      const size_t output_capacity) {
        size_t output_length = output_capacity / 4;
        vint_codec.encodeArray(begin,
                        num_els,
                        reinterpret_cast<uint32_t*>(output),
                        output_length);
        return output_length * 4;
  }

  size_t decode_sorted_vint_native(
    const uint8_t* compressed_data,
    const size_t compressed_size,
    uint32_t* uncompressed,
    const size_t uncompressed_capacity) {
      size_t num_ints = uncompressed_capacity;
      vint_codec.decodeArray(reinterpret_cast<const uint32_t*>(compressed_data), compressed_size / 4, uncompressed, num_ints);
      return num_ints;
  }


  size_t encode_s4_bp128_dm_native(
       uint32_t* begin,
       const size_t num_els,
       uint8_t* output,
       const size_t output_capacity) {
        size_t output_length = output_capacity / 4;
        codec_sorted -> encodeArray(begin,
                          num_els,
                          reinterpret_cast<uint32_t*>(output),
                          output_length);
        return output_length * 4;
  }

  size_t decode_s4_bp128_dm_native(
      const uint8_t* compressed_data,
      const size_t compressed_size,
      uint32_t* uncompressed,
      const size_t uncompressed_capacity) {
        size_t num_ints = uncompressed_capacity;
        codec_sorted -> decodeArray(reinterpret_cast<const uint32_t*>(compressed_data), compressed_size / 4, uncompressed, num_ints);
        return num_ints;
  }
  
    size_t encode_composite_native(
       uint32_t* uncompressed,
       const size_t num_els,
       uint8_t* output,
       const size_t output_capacity) {
        size_t output_length = output_capacity / 4;
        composite_codec_unsorted.encodeArray(uncompressed,
                          num_els,
                          reinterpret_cast<uint32_t*>(output),
                          output_length);
        return output_length * 4;
  }

  size_t decode_composite_native(
      const uint8_t* compressed_data,
      const size_t compressed_size,
      uint32_t* uncompressed,
      const size_t uncompressed_capacity) {
        size_t num_ints = uncompressed_capacity;
        composite_codec_unsorted.decodeArray(reinterpret_cast<const uint32_t*>(compressed_data), compressed_size/4, uncompressed, num_ints);
        return num_ints;
  }


  size_t encode_unsorted_native(
       uint32_t* begin,
       const size_t num_els,
       uint8_t* output,
       const size_t output_capacity) {
         size_t output_length = output_capacity / 4;
         codec_unsorted.encodeArray(begin,
                           num_els,
                           reinterpret_cast<uint32_t*>(output),
                           output_length);
        return output_length * 4;
  }



  size_t decode_unsorted_native(
      const uint8_t* compressed_data,
      const size_t compressed_size,
      uint32_t* uncompressed,
      const size_t uncompressed_capacity) {
         size_t num_ints = uncompressed_capacity;
         codec_unsorted.decodeArray(reinterpret_cast<const uint32_t*>(compressed_data), compressed_size / 4, uncompressed, num_ints);
         return num_ints;
  }

  size_t intersection_native(
      const uint32_t* left,
      const size_t left_size,
      const uint32_t* right,
      const size_t right_size,
      uint32_t* output) {
        return IntersectionFactory::getFromName("simd")(left, left_size, right, right_size, output);
  }
}
