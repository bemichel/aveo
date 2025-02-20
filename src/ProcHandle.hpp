/**
 * @file ProcHandle.hpp
 * @brief VEO process handle
 */
#ifndef _VEO_PROC_HANDLE_HPP_
#define _VEO_PROC_HANDLE_HPP_
#include <unordered_map>
#include <utility>
#include <memory>
#include <mutex>
#include <iostream>

#include <urpc.h>
#include <ve_offload.h>
#include "CallArgs.hpp"
#include "Context.hpp"
#include "VEOException.hpp"

namespace std {
    template <>
    class hash<std::pair<uint64_t, std::string>> {
    public:
        size_t operator()(const std::pair<uint64_t, std::string>& x) const{
            return hash<uint64_t>()(x.first) ^ hash<std::string>()(x.second);
        }
    };
}

namespace veo {

  int _getProcIdentifier(ProcHandle *);

/**
 * @brief VEO process handle
 */
class ProcHandle {
private:
  std::unordered_map<std::pair<uint64_t, std::string>, uint64_t> sym_name;
  std::mutex sym_mtx;
  std::mutex main_mutex;		//!< acquire when opening a new context
  urpc_peer_t *up;			//!< ve-urpc peer pointer
  uint64_t ve_sp;       		//!< stack pointer on VE side
  Context *mctx;			//!< context also used for sync proc ops
  std::vector<std::unique_ptr<Context>> ctx;	//!< vector of opened contexts
  int ve_number;			//!< store the VE number

public:
  ProcHandle(int, char *);
  ~ProcHandle() { this->exitProc(); }

  static ProcHandle *getProcHandle(int);
  uint64_t loadLibrary(const char *);
  int unloadLibrary(const uint64_t libhdl);
  uint64_t getSym(const uint64_t, const char *);

  uint64_t allocBuff(const size_t);
  void freeBuff(const uint64_t);

  int readMem(void *, uint64_t, size_t);
  int writeMem(uint64_t, const void *, size_t);
  int exitProc(void);
  int getProcIdentifier(void);
  int numProcs(void);

  int numContexts(void);
  Context *getContext(int);

  int callSync(uint64_t, CallArgs &, uint64_t *);

  Context *mainContext() { return this->mctx; };
  Context *openContext(size_t stack_sz = VEO_DEFAULT_STACKSIZE);
  void delContext(Context *);

  veo_proc_handle *toCHandle() {
    return reinterpret_cast<veo_proc_handle *>(this);
  }

  int veNumber() { return this->ve_number; }
};
} // namespace veo
#endif
