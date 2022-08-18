#pragma once


// TODO: Think of a better method than this
/*#define DECLARE_MEMORY_BUFFER(_var, _name) friend class CEMemoryManager; \ this was a bad idea
							  private: \
							  void*& _CONFIG_MEMORY_GET_PTR_() {return (void*&)_var;} \ // user-defined
							  const char* _CONGIG_MEMORY_GET_NAME_() { return _name; }
*/