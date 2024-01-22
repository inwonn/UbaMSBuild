// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ToolTaskStatus.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_ToolTaskStatus_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_ToolTaskStatus_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_ToolTaskStatus_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_ToolTaskStatus_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_ToolTaskStatus_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace ubavs {

enum ToolTaskStatus : int {
  Created = 0,
  Cancelled = 1,
  Completed = 2,
  Faulted = 3,
  ProviderRunning = 4,
  ProviderRanToCompletion = 5,
  ProviderFaulted = 6,
  HostRunning = 7,
  HostRanToCompletion = 8,
  HostFaulted = 9,
  ToolTaskStatus_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ToolTaskStatus_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ToolTaskStatus_IsValid(int value);
constexpr ToolTaskStatus ToolTaskStatus_MIN = Created;
constexpr ToolTaskStatus ToolTaskStatus_MAX = HostFaulted;
constexpr int ToolTaskStatus_ARRAYSIZE = ToolTaskStatus_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ToolTaskStatus_descriptor();
template<typename T>
inline const std::string& ToolTaskStatus_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ToolTaskStatus>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ToolTaskStatus_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ToolTaskStatus_descriptor(), enum_t_value);
}
inline bool ToolTaskStatus_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ToolTaskStatus* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ToolTaskStatus>(
    ToolTaskStatus_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace ubavs

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::ubavs::ToolTaskStatus> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ubavs::ToolTaskStatus>() {
  return ::ubavs::ToolTaskStatus_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_ToolTaskStatus_2eproto
