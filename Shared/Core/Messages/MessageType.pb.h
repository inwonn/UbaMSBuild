// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: MessageType.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_MessageType_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_MessageType_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_MessageType_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_MessageType_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_MessageType_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace ubavs {

enum MessageType : int {
  Build = 0,
  WriteLog = 1,
  MessageType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  MessageType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool MessageType_IsValid(int value);
constexpr MessageType MessageType_MIN = Build;
constexpr MessageType MessageType_MAX = WriteLog;
constexpr int MessageType_ARRAYSIZE = MessageType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* MessageType_descriptor();
template<typename T>
inline const std::string& MessageType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, MessageType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function MessageType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    MessageType_descriptor(), enum_t_value);
}
inline bool MessageType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, MessageType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<MessageType>(
    MessageType_descriptor(), name, value);
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

template <> struct is_proto_enum< ::ubavs::MessageType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ubavs::MessageType>() {
  return ::ubavs::MessageType_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_MessageType_2eproto
