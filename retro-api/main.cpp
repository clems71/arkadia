#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>

#include "core.h"

using namespace v8;
using namespace node;

void nodeCoreInit(const FunctionCallbackInfo<Value> & args)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  v8::String::Utf8Value corePath(args[0]->ToString());
  coreInit(*corePath);
}

void nodeCoreLoadGame(const FunctionCallbackInfo<Value> & args)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  v8::String::Utf8Value romPath(args[0]->ToString());
  coreLoadGame(*romPath);
}

void nodeCoreUpdate(const FunctionCallbackInfo<Value> & args)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() != 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  coreUpdate();
}

void nodeCoreVideoData(const FunctionCallbackInfo<Value> & args)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  size_t width, height;
  const auto & videoBuf = coreVideoData(width, height);

  Local<Object> obj = Object::New(isolate);
  obj->Set(String::NewFromUtf8(isolate, "width"), Number::New(isolate, width));
  obj->Set(String::NewFromUtf8(isolate, "height"), Number::New(isolate, height));

  // Buffer access
  Local<Object> array = args[0]->ToObject();
  uint8_t * ptr = static_cast<uint8_t *>(array->GetIndexedPropertiesExternalArrayData());

  memcpy(ptr, &videoBuf[0], videoBuf.size() * 4);

  args.GetReturnValue().Set(obj);
}

void nodeCoreVideoSize(const FunctionCallbackInfo<Value> & args)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() != 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  size_t width, height;
  coreVideoData(width, height);

  Local<Object> obj = Object::New(isolate);
  obj->Set(String::NewFromUtf8(isolate, "width"), Number::New(isolate, width));
  obj->Set(String::NewFromUtf8(isolate, "height"), Number::New(isolate, height));

  args.GetReturnValue().Set(obj);
}

void nodeCoreAudioData(const FunctionCallbackInfo<Value> & args)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() != 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  const auto audioBuf = coreAudioData();

  auto slowBuffer = Buffer::New(audioBuf.size() * 2); // uint16_t hence the factor 2
  memcpy(Buffer::Data(slowBuffer), &audioBuf[0], audioBuf.size() * 2);
  args.GetReturnValue().Set(slowBuffer);
}

void nodeCoreTimings(const FunctionCallbackInfo<Value> & args)
{
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.Length() != 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  double fps, audioSampleRate;
  coreTimings(fps, audioSampleRate);

  Local<Object> obj = Object::New(isolate);
  obj->Set(String::NewFromUtf8(isolate, "fps"), Number::New(isolate, fps));
  obj->Set(String::NewFromUtf8(isolate, "audio_sample_rate"), Number::New(isolate, audioSampleRate));

  args.GetReturnValue().Set(obj);
}

void init(Handle<Object> exports)
{
  NODE_SET_METHOD(exports, "coreInit", nodeCoreInit);
  NODE_SET_METHOD(exports, "coreLoadGame", nodeCoreLoadGame);
  NODE_SET_METHOD(exports, "coreUpdate", nodeCoreUpdate);
  NODE_SET_METHOD(exports, "coreVideoData", nodeCoreVideoData);
  NODE_SET_METHOD(exports, "coreVideoSize", nodeCoreVideoSize);
  NODE_SET_METHOD(exports, "coreAudioData", nodeCoreAudioData);
  NODE_SET_METHOD(exports, "coreTimings", nodeCoreTimings);
}

NODE_MODULE(retro_api, init)
