#include "ocl.h"
#include <modload.h>

#define MAXARGS 64
namespace csnd {

  void err_msg(std::string s, void *uData) {
    Csound *cs = (Csound *)uData;
    cs->message(s);
  }
  
  
  struct Clopd : InPlug<MAXARGS*2 + 3> {
    OCl *ocl;
    bool is_audio[2*MAXARGS+3];
    int numin, numout;
  
    int init() {
      cl_device_id id, device_ids[32];
      int err;
      int arrays=0; 
      char name[128];
      unsigned int num; 
      numin = in_count();
      numout = out_count();
      if(numout > MAXARGS) numout = MAXARGS;
      if(numin > MAXARGS + 3) numin = MAXARGS + 3;

      for(int i  = 0; i < numout; i++) {
        if(csound->is_asig(args(i))) {
          is_audio[i] = true;
        } else is_audio[i] = false;
        arrays++;
      }

      const char *code =  args.str_data(arrays).data;
      int ndev = args[arrays+1];

      for(int i = arrays+3; i < numin+numout; i++) {
        if(csound->is_asig(args(i))) {
          arrays++;
          is_audio[i] = true;
        } else is_audio[i] = false;
      }

      err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 32, device_ids, &num);
      if (err != CL_SUCCESS)
        return csound->init_error("failed to find an OpenCL device!\n");
      id = device_ids[ndev];
      clGetDeviceInfo(id, CL_DEVICE_NAME, 128, name, NULL);
      csound->message("using device: ");
      csound->message(name);
    
      ocl = new OCl(id, 1, arrays, code, err_msg, csound);
      if (ocl->get_err() == CL_SUCCESS) {
        ocl->setKernel(0, "process");
        arrays = 0;
        for(int i = 0; i < numout; i++) 
          if(is_audio[i]) 
            ocl->alloc(arrays++, sizeof(MYFLT)*ksmps());
          else
            ocl->alloc(arrays++, sizeof(MYFLT));
        for(int i = arrays+3; i < numin+numout; i++) 
          if(is_audio[i]) 
            ocl->alloc(arrays++, sizeof(MYFLT)*ksmps());
  
        return OK;
      }
      return csound->init_error("failed to setup OpenCL!\n");
    }

    int deinit() {
      delete ocl;
      return OK;
    }
  
    int aperf() {
      int nargs = 0;
      int arrays = 0;
      size_t threads = 0;
      int err;
    
      for(int i = 0; i < numout; i++) {
        ocl->setArg(0, ocl->getData(arrays++), nargs++);
      }

      threads = args[arrays+2];
      for(int i = arrays+3; i < numin+numout; i++) 
        if(is_audio[i]) {
          ocl->setArg(0, args(i) + offset, nsmps, arrays++, nargs++);
        }
        else
          ocl->setArg(0, args[i], nargs++);

      err = ocl->execute(0, threads);
      if(err)
        return csound->perf_error(ocl->cl_error_string(err), this);

      arrays = 0;
      for(int i = 0; i < numout; i++)
        if(is_audio[i]) 
          ocl->getData(args(i) + offset, nsmps, arrays++);
        else
          ocl->getData(args(i), 1, arrays++);

      return OK;
    }

  };

  struct Clopf : InPlug<MAXARGS*2 + 3> {
    OCl *ocl;
    bool is_audio[2*MAXARGS+3];
    int numin, numout;
    AuxMem<float> cvt;
  
    int init() {
      cl_device_id id, device_ids[32];
      int err;
      int arrays=0; 
      char name[128];
      unsigned int num; 
      numin = in_count();
      numout = out_count();
      if(numout > MAXARGS) numout = MAXARGS;
      if(numin > MAXARGS + 3) numin = MAXARGS + 3;

      for(int i  = 0; i < numout; i++) {
        if(csound->is_asig(args(i))) {
          is_audio[i] = true;
        } else is_audio[i] = false;
        arrays++;
      }

      const char *code =  args.str_data(arrays).data;
      int ndev = args[arrays+1];

      for(int i = arrays+3; i < numin+numout; i++) {
        if(csound->is_asig(args(i))) {
          arrays++;
          is_audio[i] = true;
        } else is_audio[i] = false;
      }

      err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 32, device_ids, &num);
      if (err != CL_SUCCESS)
        return csound->init_error("failed to find an OpenCL device!\n");
      id = device_ids[ndev];
      clGetDeviceInfo(id, CL_DEVICE_NAME, 128, name, NULL);
      csound->message("using device: ");
      csound->message(name);
    
      ocl = new OCl(id, 1, arrays, code, err_msg, csound);
      if (ocl->get_err() == CL_SUCCESS) {
        ocl->setKernel(0, "process");
        arrays = 0;
        for(int i = 0; i < numout; i++) 
          if(is_audio[i]) 
            ocl->alloc(arrays++, sizeof(float)*ksmps());
          else
            ocl->alloc(arrays++, sizeof(float));
        for(int i = arrays+3; i < numin+numout; i++) 
          if(is_audio[i]) 
            ocl->alloc(arrays++, sizeof(float)*ksmps());
        
        cvt.allocate(csound, ksmps());
        return OK;
      }
      return csound->init_error("failed to setup OpenCL!\n");
    }

    int deinit() {
      delete ocl;
      return OK;
    }
  
    int aperf() {
      int nargs = 0;
      int arrays = 0;
      size_t threads = 0;
      int err;
    
      for(int i = 0; i < numout; i++) {
        ocl->setArg(0, ocl->getData(arrays++), nargs++);
      }

      threads = args[arrays+2];
      for(int i = arrays+3; i < numin+numout; i++) 
        if(is_audio[i]) {
          AudioSig asig(this, args(i));
          int j = offset;
          for(auto &k : cvt) k = asig[j++];
          ocl->setArg(0, cvt.data() + offset, nsmps, arrays++, nargs++);
        }
        else {
          ocl->setArg(0, (float) args[i], nargs++);
        }

      err = ocl->execute(0, threads);
      if(err)
        return csound->perf_error(ocl->cl_error_string(err), this);

      arrays = 0;
      for(int i = 0; i < numout; i++)
        if(is_audio[i]) {
          AudioSig asig(this, args(i));
          int j = offset;
          ocl->getData(cvt.data() + offset, nsmps, arrays++);
          for(auto &k : asig) k = cvt[j++];      
        }
        else {
          float val;
          ocl->getData(&val, 1, arrays++);
          args[i] = val;
        }     
      return OK;
    }
  };



  void on_load(Csound *csound) {
    plugin<Clopd>(csound, "clopd", "*", "SikM", csnd::thread::ia);
    plugin<Clopf>(csound, "clopf", "*", "SikM", csnd::thread::ia);
  }
  
}

