#include <cassert>
#include <stdio.h>

#define INITGUID
#include <Unknwn.h>


// {63302041-1D44-43BA-872D-7B7AF364E23E}
DEFINE_GUID(IID_MyInterface, 
0x63302041, 0x1d44, 0x43ba, 0x87, 0x2d, 0x7b, 0x7a, 0xf3, 0x64, 0xe2, 0x3e);

struct MyInterface : IUnknown {
  virtual HRESULT __stdcall MyMethod(BSTR input, BSTR * output) = 0;
};

// COM:
//  COM was developed to allow different software to built of components built in
//  in different programming languages. Big ones at the time, VB, C++, Delphi (Pascal)
//  No common way on how to call a function, no common way to handle errors,
//  no common way to query what interfaces are supported, no common way to handle
//  parameter life-time, no common way to handle object life-times
//  
//  A calling convention : Called pascal calling convention aka stdcall
//  A vtable
//  Not allowed to throw exceptions! Exceptions are not portable
//  In parameters are allocated and deallocated by caller
//  Out parameters are allocated by callee and deallocated by caller
//  In/Out is allocated by caller and can be deallocated and allocated by callee, deallocated by caller
//  Call COM objects must implement IUnknown which allows you to query
//    for more interfaces and handle life times (reference counting)

struct MyComObject : MyInterface {
  MyComObject() : _ref_count(0) {
  }

  virtual ~MyComObject() {
  }

  virtual HRESULT __stdcall QueryInterface( 
        const IID & riid
      , void        **ppvObject
      ) {
    if (riid == IID_MyInterface) {
      AddRef();
      *ppvObject = static_cast<MyInterface*>(this);
      return S_OK;
    } else if (riid == IID_IUnknown) {
      AddRef();
      *ppvObject = static_cast<IUnknown*>(this);
      return S_OK;
    } else {
      return E_NOINTERFACE;
    }
  }

  virtual ULONG __stdcall AddRef() {
    return ++_ref_count;
  }

  virtual ULONG __stdcall Release() {
    auto rc = --_ref_count;
    if (rc == 0) {
      delete this;
    }
    return rc;
  }

  virtual HRESULT __stdcall MyMethod(BSTR input, BSTR * output) {

    return S_OK;
  }
private:
  ULONG _ref_count;
};

struct i_op {
    __declspec(noinline) virtual int f(int x, int y) = 0;
};

struct add_op : i_op {
    __declspec(noinline) virtual int f(int x, int y) {
        return x + y;
    }
};

struct multiplication_op : i_op {
    __declspec(noinline) virtual int f(int x, int y) {
        return x * y;
    }
};

__declspec(noinline) int f(int x, int y) {
    return x + y;
}

__declspec(noinline) int g(i_op & op, int x, int y) {
    return op.f(x, y);
}

int main() {
  volatile int x = 3;
  volatile int y = 4;


  auto z = f(x, y);
  auto op = multiplication_op();
  auto w = g(op, x, y);
  printf("Hello world: %d, %d\n", z, w);

  auto hr = CoInitialize(0);
  assert(SUCCEEDED(hr));

  auto co = new MyComObject();
  co->AddRef();
  MyInterface * my_interface = nullptr;
  auto qhr = co->QueryInterface(IID_MyInterface, reinterpret_cast<void**>(&my_interface));
  if (SUCCEEDED(qhr)) {
    auto myBstr = SysAllocString(L"Yello");
    BSTR otherBstr = nullptr;
    my_interface->MyMethod(myBstr, &otherBstr);
    SysFreeString(otherBstr);
    SysFreeString(myBstr);
    my_interface->Release();
  }
  co->Release();

  CoUninitialize();

  return 0;
}
