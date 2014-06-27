// http://jquery.org/license
#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#include <stdlib.h>
#include <cstdlib>
#else
#include <bfd.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#endif
#include "traceback.hpp"

// __________________________________________________________________________//

namespace traceback {

#ifndef _WIN32

// __________________________________________________________________________//

/*
 * @brief Demangling compiled C++ names
 *
 * @param Name to be demangled
 *
 * @return A pointer to the start of the NUL-terminated demangled name, or NULL
 * if the demangling fails. The caller is responsible for deallocating this
 * memory using free.
 */

static char* Demangler(const char *mangled_name) {
  int status = 0;

  char *unmangled_name = abi::__cxa_demangle(mangled_name, NULL, 0, &status);

  if (status == 0)
    return unmangled_name;

  free(unmangled_name);

  return NULL;
}

// __________________________________________________________________________//

// Callback function for bfd_map_over_sections
void FindAddressInSection(bfd *abfd, asection *section, void *data);

/**
 * @brief Retrieves symbol information for Linux.
 */
class LinuxSym {
private:
  bool found_;
  bfd *pbfd_;
  asymbol **syms_;
  unsigned int line_number_;
  unsigned long address_;
  const char* filename_;
  const char* function_;

  /**
   * @brief Reads symbols from file
   *
   * @param filename File to process
   *
   * @return true if symbols are loaded into memory otherwise false.
   */
  bool LoadFile(const char* filename) {
    char **matching;

    pbfd_ = bfd_openr(filename, NULL);
    if (pbfd_ == NULL)
      return false;

    //pbfd_->flags |= BFD_DECOMPRESS;

    if (!bfd_check_format_matches(pbfd_, bfd_object, &matching))
      return false;

    if ((bfd_get_file_flags(pbfd_) & HAS_SYMS) == 0)
      return false;

    long storage = bfd_get_symtab_upper_bound(pbfd_);
    if (storage <= 0)
      return false;

    syms_ = reinterpret_cast<asymbol**>(malloc(storage));

    return bfd_canonicalize_symtab(pbfd_, syms_);
  }

public:
  // Allows access to private members for the callback function
  friend void FindAddressInSection(bfd *abfd, asection *section, void *data);

  /**
   * @brief Default constructor
   *
   * @param filename File to process
   * @param addr Filled with backtrace address.
   */
  LinuxSym(const char* filename, const void* const addr) :
      found_(false), pbfd_(NULL), syms_(NULL) {
    if (LoadFile(filename)) {
      address_ = reinterpret_cast<unsigned long>(addr);
      bfd_map_over_sections(pbfd_, FindAddressInSection, this);
    }
  }

  ~LinuxSym() {
    if (pbfd_)
      bfd_close(pbfd_);

    free(syms_);
  }

  /**
   * @brief Returns the research status of line number
   *
   * @return true if we could find a valid line number
   */
  bool found() const {
    return found_ && line_number_ != 0;
  }

  /**
   * @brief Returns the line number
   *
   * @return the line number
   */
  unsigned int line_number() const {
    return line_number_;
  }

  /**
   * @brief Returns the source filename
   *
   * @return the path of the source
   */
  const char* filename() const {
    return filename_;
  }

  /**
   * @brief Returns the function name
   *
   * @return the function name
   */
  const char* function() const {
    return function_;
  }
};

// Look for an address in a section. This is called via bfd_map_over_sections.
void FindAddressInSection(bfd *abfd, asection *section, void *data) {
  LinuxSym* psym = reinterpret_cast<LinuxSym*>(data);

  if (psym->found_)
    return;

  if ((bfd_get_section_flags(abfd, section) & SEC_ALLOC) == 0)
    return;

  bfd_vma vma = bfd_get_section_vma(abfd, section);
  if (psym->address_ < vma)
    return;

  bfd_size_type size = bfd_section_size(abfd, section);
  if (psym->address_ >= vma + size)
    return;

  psym->found_ = bfd_find_nearest_line(
      abfd,
      section,
      psym->syms_,
      psym->address_ - vma - 1,
      const_cast<const char**>(&psym->filename_),
      const_cast<const char**>(&psym->function_),
      &psym->line_number_);
}
#endif

// ___________________________________________________________________________//

static const char *LookupSymbol(const void* addr) {
  std::ostringstream ss;

#ifdef _WIN32
  static HANDLE h_process = 0;
  static bool sym = false;

  if (h_process == 0) {
    h_process = GetCurrentProcess();
    SymSetOptions(SYMOPT_DEFERRED_LOADS|SYMOPT_LOAD_LINES);

    if (SymInitialize(h_process, NULL, TRUE))
    sym = true;
  }

  if (sym) {
    DWORD dw_displacement = 0;
    DWORD64 dw64_displacement = 0;
    DWORD64 dw64_address = (DWORD64)addr;

    std::vector<char> buffer(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR));

    IMAGEHLP_SYMBOL64 *pSym = NULL;
    pSym = (IMAGEHLP_SYMBOL64 *) &buffer.front();
    pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    pSym->MaxNameLength = MAX_PATH;

    // Locates the symbol for the specified address.
    if (SymGetSymFromAddr64(h_process, dw64_address, &dw64_displacement, pSym)) {
      char symbol_name[MAX_SYM_NAME + 1];
      strncpy_s(symbol_name, pSym->Name, MAX_SYM_NAME);

      IMAGEHLP_LINE64 line;
      memset(&line, 0, sizeof(line));
      line.SizeOfStruct = sizeof(line);

      // Locates the source line for the specified address.
      if (SymGetLineFromAddr64(h_process,
              dw64_address, &dw_displacement, &line)) {

        ss << "  File \"" << line.FileName << "\", line "
        << line.LineNumber << std::endl
        << "    " << symbol_name;
      }
      return _strdup(ss.str().c_str());
    }
  }
#else
  static bool init = false;

  if (!init) {
    bfd_init();
    init = true;
  }

  Dl_info info;
  if (dladdr(addr, &info)) {
    LinuxSym sym(info.dli_fname, addr);
    if (sym.found()) {
      ss << "  File \"" << sym.filename() << "\", line " << sym.line_number()
          << std::endl << "    ";
      char* function = Demangler(info.dli_sname);
      if (function) {
        ss << function;
        free(function);
      } else
        ss << sym.function();
    }
    return ss.str().length() == 0 ? NULL : strdup(ss.str().c_str());
  }
#endif
  return NULL;
}

// ___________________________________________________________________________//

#ifdef _WIN32
// Captures a stack back trace by walking up the stack and recording the
// information for each frame.
size_t backtrace(void** buffer, int size) {
  return RtlCaptureStackBackTrace(0, size, buffer, 0);
}
#endif

// ___________________________________________________________________________//

CallStack::CallStack() {
  static const int max_depth = 32;
  void **trace = new void*[max_depth];

  // This method of handling system functions can change the errno value. The
  // method saves the errno value to return the original value.
  int errno_bck = errno;

  size_t stack_depth = backtrace(trace, max_depth);

  // We pass the functions that generate the display and the calls to system
  // functions.
#ifdef _WIN32
  const size_t first = 2;
  const size_t last = stack_depth - 3;
#else
  const size_t first = 1;
  const size_t last = stack_depth - 2;
#endif

  for (size_t ix = first; ix < last; ++ix) {
    const char* symbol_name = LookupSymbol(trace[ix]);
    if (symbol_name == NULL)
      break;

    stack_.push_back(symbol_name);
  }
  delete[] trace;

  // Restore the value of the variable errno received.
  errno = errno_bck;
}

// ___________________________________________________________________________//

CallStack::operator std::string() const {
  std::ostringstream ss;
  if (stack_.size()) {
    std::vector<std::string>::const_reverse_iterator it;
    ss << "Traceback (most recent call last)" << std::endl;
    for (it = stack_.rbegin(); it != stack_.rend(); ++it) {
      ss << *it;
      if (it < stack_.rend() - 1)
        ss << std::endl;
    }
    ss << std::endl;
  }
  return std::string(ss.str());
}

} // namespace traceback
