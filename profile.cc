#include "profile.h"
#include "profile_node.h"

namespace nodex {
  using v8::CpuProfile;
  using v8::CpuProfileNode;
  using v8::Exception;
  using v8::FunctionCallbackInfo;
  using v8::FunctionTemplate;
  using v8::Handle;
  using v8::HandleScope;
  using v8::Integer;
  using v8::Isolate;
  using v8::Local;
  using v8::Object;
  using v8::ObjectTemplate;
  using v8::Persistent;
  using v8::PropertyCallbackInfo;
  using v8::String;
  using v8::ThrowException;
  using v8::Undefined;
  using v8::Value;

  Persistent<ObjectTemplate> Profile::profile_template_;

  void Profile::Initialize (Isolate* isolate) {
    HandleScope scope(isolate);
    Local<ObjectTemplate> o = ObjectTemplate::New();
    o->SetInternalFieldCount(1);
    o->SetAccessor(String::New("title"), Profile::GetTitle);
    o->SetAccessor(String::New("uid"), Profile::GetUid);
    o->SetAccessor(String::New("topRoot"), Profile::GetTopRoot);
    o->SetAccessor(String::New("samplesCount"), Profile::GetSamplesCount);
    o->Set(String::New("delete"), FunctionTemplate::New(Profile::Delete));
    profile_template_.Reset(isolate, o);
  }

  void Profile::GetUid (Local<String> property, const PropertyCallbackInfo<Value>& info) {
    HandleScope scope(info.GetIsolate());
    Local<Object> self = info.Holder();
    void* ptr = self->GetAlignedPointerFromInternalField(0);
    uint32_t uid = static_cast<CpuProfile*>(ptr)->GetUid();
    info.GetReturnValue().Set(Integer::NewFromUnsigned(uid));
  }


  void Profile::GetTitle (Local<String> property, const PropertyCallbackInfo<Value>& info) {
    HandleScope scope(info.GetIsolate());
    Local<Object> self = info.Holder();
    void* ptr = self->GetAlignedPointerFromInternalField(0);
    Handle<String> title = static_cast<CpuProfile*>(ptr)->GetTitle();
    info.GetReturnValue().Set(title);
  }

  void Profile::GetTopRoot (Local<String> property, const PropertyCallbackInfo<Value>& info) {
    Isolate* isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Object> self = info.Holder();
    void* ptr = self->GetAlignedPointerFromInternalField(0);
    const CpuProfileNode* node = static_cast<CpuProfile*>(ptr)->GetTopDownRoot();
    info.GetReturnValue().Set(ProfileNode::New(isolate, node));
  }

  void Profile::GetSamplesCount (Local<String> property, const PropertyCallbackInfo<Value>& info) {
    HandleScope scope(info.GetIsolate());
    Local<Object> self = info.Holder();
    void* ptr = self->GetAlignedPointerFromInternalField(0);
    int scount = static_cast<CpuProfile*>(ptr)->GetSamplesCount();
    info.GetReturnValue().Set(Integer::New(scount));
  }


  void Profile::Delete (const FunctionCallbackInfo<Value>& args) {
    HandleScope scope(args.GetIsolate());
    Handle<Object> self = args.This();
    void* ptr = self->GetAlignedPointerFromInternalField(0);
    static_cast<CpuProfile*>(ptr)->Delete();
    args.GetReturnValue().SetUndefined();
  }

  Handle<Value> Profile::New (Isolate* isolate, const CpuProfile* profile) {
    if (profile_template_.IsEmpty()) {
      Profile::Initialize(isolate);
    }
    
    if ( ! profile) {
      return Undefined();
    } else {
      Local<ObjectTemplate> tpl = Local<ObjectTemplate>::New(isolate, profile_template_);
      Local<Object> obj = tpl->NewInstance();
      obj->SetAlignedPointerInInternalField(0, const_cast<CpuProfile*>(profile));
      return obj;
    }
  }
}
